#include <dolphin/card.h>

#include <string.h>

#include "__card.h"

#define CARD_READ_SIZE 512
#define CARD_COMMENT_SIZE 64

#define CARD_ICON_WIDTH 32
#define CARD_ICON_HEIGHT 32

#define CARD_BANNER_WIDTH 96
#define CARD_BANNER_HEIGHT 32

#define CARD_STAT_ICON_NONE 0
#define CARD_STAT_ICON_C8 1
#define CARD_STAT_ICON_RGB5A3 2
#define CARD_STAT_ICON_MASK 3

#define CARD_STAT_BANNER_NONE 0
#define CARD_STAT_BANNER_C8 1
#define CARD_STAT_BANNER_RGB5A3 2
#define CARD_STAT_BANNER_MASK 3

#define CARD_STAT_SPEED_END 0
#define CARD_STAT_SPEED_FAST 1
#define CARD_STAT_SPEED_MIDDLE 2
#define CARD_STAT_SPEED_SLOW 3
#define CARD_STAT_SPEED_MASK 3

#define CARDGetBannerFormat(stat) (((stat)->bannerFormat) & CARD_STAT_BANNER_MASK)
#define CARDGetIconFormat(stat, n) (((stat)->iconFormat >> (2 * (n))) & CARD_STAT_ICON_MASK)
#define CARDSetIconSpeed(stat, n, f)                                                                                   \
    ((stat)->iconSpeed = (u16)(((stat)->iconSpeed & ~(CARD_STAT_SPEED_MASK << (2 * (n)))) | ((f) << (2 * (n)))))

static void UpdateIconOffsets(CARDDir *ent, CARDStat *stat)
{
    u32 offset;
    BOOL iconTlut;
    int i;

    offset = ent->iconAddr;
    if (offset == 0xffffffff)
    {
        stat->bannerFormat = 0;
        stat->iconFormat = 0;
        stat->iconSpeed = 0;
        offset = 0;
    }

    iconTlut = FALSE;
    switch (CARDGetBannerFormat(ent))
    {
    case CARD_STAT_BANNER_C8:
        stat->offsetBanner = offset;
        offset += CARD_BANNER_WIDTH * CARD_BANNER_HEIGHT;
        stat->offsetBannerTlut = offset;
        offset += 2 * 256;
        break;
    case CARD_STAT_BANNER_RGB5A3:
        stat->offsetBanner = offset;
        offset += 2 * CARD_BANNER_WIDTH * CARD_BANNER_HEIGHT;
        stat->offsetBannerTlut = 0xffffffff;
        break;
    default:
        stat->offsetBanner = 0xffffffff;
        stat->offsetBannerTlut = 0xffffffff;
        break;
    }
    for (i = 0; i < CARD_ICON_MAX; ++i)
    {
        switch (CARDGetIconFormat(ent, i))
        {
        case CARD_STAT_ICON_C8:
            stat->offsetIcon[i] = offset;
            offset += CARD_ICON_WIDTH * CARD_ICON_HEIGHT;
            iconTlut = TRUE;
            break;
        case CARD_STAT_ICON_RGB5A3:
            stat->offsetIcon[i] = offset;
            offset += 2 * CARD_ICON_WIDTH * CARD_ICON_HEIGHT;
            break;
        default:
            stat->offsetIcon[i] = 0xffffffff;
            break;
        }
    }
    if (iconTlut)
    {
        stat->offsetIconTlut = offset;
        offset += 2 * 256;
    }
    else
    {
        stat->offsetIconTlut = 0xffffffff;
    }
    stat->offsetData = offset;
}

s32 CARDGetStatus(s32 chan, s32 fileNo, CARDStat *stat)
{
    CARDControl *card;
    CARDDir *dir;
    CARDDir *ent;
    s32 result;

    if (fileNo < 0 || CARD_MAX_FILE <= fileNo)
        return CARD_RESULT_FATAL_ERROR;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0)
        return result;

    dir = __CARDGetDirBlock(card);
    ent = &dir[fileNo];
    result = __CARDAccess(ent);
    if (result == CARD_RESULT_NOPERM)
        result = __CARDIsPublic(ent);

    if (result >= 0)
    {
        memcpy(stat->gameName, ent->gameName, sizeof(stat->gameName));
        memcpy(stat->company, ent->company, sizeof(stat->company));
        stat->length = (u32)ent->length * card->sectorSize;
        memcpy(stat->fileName, ent->fileName, CARD_FILENAME_MAX);
        stat->time = ent->time;

        stat->bannerFormat = ent->bannerFormat;
        stat->iconAddr = ent->iconAddr;
        stat->iconFormat = ent->iconFormat;
        stat->iconSpeed = ent->iconSpeed;
        stat->commentAddr = ent->commentAddr;

        UpdateIconOffsets(ent, stat);
    }
    return __CARDPutControlBlock(card, result);
}

s32 CARDSetStatusAsync(s32 chan, s32 fileNo, CARDStat *stat, CARDCallback callback)
{
    CARDControl *card;
    CARDDir *dir;
    CARDDir *ent;
    s32 result;

    if (fileNo < 0 || CARD_MAX_FILE <= fileNo || (stat->iconAddr != 0xffffffff && CARD_READ_SIZE <= stat->iconAddr)
        || (stat->commentAddr != 0xffffffff
            && CARD_SYSTEM_BLOCK_SIZE - CARD_COMMENT_SIZE < stat->commentAddr % CARD_SYSTEM_BLOCK_SIZE))
        return CARD_RESULT_FATAL_ERROR;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0)
        return result;

    dir = __CARDGetDirBlock(card);
    ent = &dir[fileNo];
    result = __CARDAccess(ent);
    if (result < 0)
        return __CARDPutControlBlock(card, result);

    ent->bannerFormat = stat->bannerFormat;
    ent->iconAddr = stat->iconAddr;
    ent->iconFormat = stat->iconFormat;
    ent->iconSpeed = stat->iconSpeed;
    ent->commentAddr = stat->commentAddr;
    UpdateIconOffsets(ent, stat);

    if (ent->iconAddr == 0xffffffff)
        CARDSetIconSpeed(ent, 0, CARD_STAT_SPEED_FAST);

    ent->time = (u32)OSTicksToSeconds(OSGetTime());
    result = __CARDUpdateDir(chan, callback);
    if (result < 0)
        __CARDPutControlBlock(card, result);
    return result;
}
