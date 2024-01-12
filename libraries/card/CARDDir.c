#include <dolphin/card.h>

#include <string.h>

#include "__card.h"

typedef struct CARDDirCheck
{
    u8 padding0[64 - 2 * 4];
    u16 padding1;
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

static CARDDirCheck *__CARDGetDirCheck(CARDDir *dir)
{
    return ((CARDDirCheck*)&(dir)[CARD_MAX_FILE]);
}

CARDDir *__CARDGetDirBlock(CARDControl* card)
{
    return card->currentDir;
}

static void WriteCallback(s32 chan, s32 result)
{
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;

    if (result >= 0)
    {
        CARDDir *dir0 = (CARDDir*)((u8*)card->workArea + 0x2000);
        CARDDir *dir1 = (CARDDir*)((u8*)card->workArea + 0x4000);

        if (card->currentDir == dir0)
        {
            card->currentDir = dir1;
            memcpy(dir1, dir0, 0x2000);
        }
        else
        {
            card->currentDir = dir0;
            memcpy(dir0, dir1, 0x2000);
        }
    }

    if (card->apiCallback == NULL)
        __CARDPutControlBlock(card, result);
    callback = card->eraseCallback;
    if (callback)
    {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

static void EraseCallback(s32 chan, s32 result)
{
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;
    CARDDir *dir;
    u32 tmp[2];
    u32 addr;

    if (result >= 0)
    {
        dir = __CARDGetDirBlock(card);
        addr = ((u32)dir - (u32)card->workArea) / 0x2000 * card->sectorSize;
        result = __CARDWrite(chan, addr, 0x2000, dir, WriteCallback);
        if (result >= 0)
            return;
    }

    if (card->apiCallback == NULL)
        __CARDPutControlBlock(card, result);
    callback = card->eraseCallback;
    if (callback)
    {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

s32 __CARDUpdateDir(s32 chan, CARDCallback callback)
{
    CARDControl *card = &__CARDBlock[chan];
    CARDDirCheck *check;
    u32 tmp[2];
    u32 addr;
    CARDDir *dir;

    if (!card->attached)
        return CARD_RESULT_NOCARD;

    dir = __CARDGetDirBlock(card);
    check = __CARDGetDirCheck(dir);
    ++check->checkCode;
    __CARDUpdateSum(dir, 0x2000 - sizeof(u32), &check->checkSum, &check->checkSumInv);
    DCStoreRange(dir, 0x2000);

    card->eraseCallback = callback;
    addr = ((u32)dir - (u32)card->workArea) / 0x2000 * card->sectorSize;
    return __CARDEraseSector(chan, addr, EraseCallback);
}
