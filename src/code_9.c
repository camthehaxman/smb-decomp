#include <dolphin.h>

#include "global.h"

void func_80094750(int arg0)
{
    lbl_802C5D60.unk0 = arg0;
    DCFlushRange(&lbl_802C5D60, sizeof(lbl_802C5D60));
    HIOWrite(0x1000, &lbl_802C5D60, sizeof(lbl_802C5D60));
    HIOWriteMailbox(arg0);
}

void func_800947B0(void)
{
    HIORead(0x1000, &lbl_802C5D60, sizeof(lbl_802C5D60));
    DCInvalidateRange(&lbl_802C5D60, sizeof(lbl_802C5D60));
}

int func_800947F8(void)
{
    u32 spC;
    u8 filler[4];

    OSGetTick();
    while (1)
    {
        HIOReadMailbox(&spC);
        if (spC == 2)
            return 0;
        if (spC == 1)
            return 1;
    }
}

void func_80094840(void)
{
    u32 sp8;

    do
    {
        HIOReadMailbox(&sp8);
    } while (sp8 != 0);
}

static s32 lbl_802F2180 ATTRIBUTE_ALIGN(8);
static u8 *lbl_802F2184;
static u32 lbl_802F2188;

void *func_80094870(void)
{
    int i;

    if (lbl_802F2180 == 0)
        return 0;
    for (i = 0; i < 16; i++)
    {
        u32 bit = 1 << i;
        if (!(lbl_802F2188 & bit))
        {
            lbl_802F2188 |= bit;
            return lbl_802F2184 + i * 0xC;
        }
    }
    return 0;
}

void func_800948D0(struct Struct80094870 *arg0)
{
    lbl_802F2188 &= ~(1 << arg0->unk8);
}

#pragma force_active on
int func_800948EC(void)
{
    return lbl_802F2180;
}
#pragma force_active reset
