#include <stdio.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "event.h"
#include "input.h"
#include "mathutil.h"
#include "mode.h"
#include "nlsprite.h"
#include "preview.h"
#include "sound.h"
#include "sprite.h"
#include "window.h"

struct
{
    u32 unk0;
    s8 unk4;
    s8 unk5;
    s8 unk6;
    s8 unk7;
    u8 unk8;
    u8 unk9;
    s8 unkA;
    s8 unkB[4];
    s8 unkF;
    s8 unk10;
    u8 unk11[3];
//    u8 unk12;
//    u8 unk13;
    u8 unk14;
    u8 filler15[3];
    u32 unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    struct Preview unk2C;
} lbl_802C6180;

//struct Preview lbl_802C6180_2C;

void lbl_800A7A6C(void);
void func_800A916C(struct Sprite *arg0);
void func_800AB720(void *);

void submode_mini_ranking_init_func(void)
{
    int i;

    event_start(0x10);
    event_start(0x12);
    modeCtrl.unk10 = 0;
    func_800AB2A0(modeCtrl.unk10, 0);
    for (i = 0; i < 4; i++)
        lbl_802C6180.unkB[i] = -1;
    u_play_music(0x42U, 0);
    submodeFinishFunc = lbl_800A7A6C;
    gameSubmodeRequest = 0xA5;
}

char lbl_801D58A8[] = "\\]ABCDEFGHIJKLMNOPQRSTUVWXYZ.!?&-@0123456789[";

void submode_mini_ranking_main_func(void)
{
    u8 sp60[8];
    u8 sp58[8];
    u8 sp50[8];
    u8 sp48[8];
    u8 sp40[8];
    u8 sp38[8];
    u8 sp30[8];
    u8 sp28[8];
    u8 sp20[8];
    u8 sp18[8];
    u8 sp10[8];
    u8 sp8[8];
    int var_r28;
    int var_r4;

    window_set_cursor_pos(1, 1);
    window_printf_2("RANKING\n");
    window_printf_2(" [%d/%d]\n", modeCtrl.unk10, 12);

    if (debugFlags & 0xA)
        return;

    if (!(lbl_802C6180.unk0 & 1))
    {
        var_r28 = modeCtrl.unk10;
        if ((controllerInfo[0].repeat.button & 1) || (analogInputs[0].repeat & 1) || (((controllerInfo[0].held.button & 1) || (analogInputs[0].held & 1)) && (analogInputs[0].held & 0x200)))
        {
            var_r28--;
            if (var_r28 < 0)
                var_r28 = 11;
        }
        if ((controllerInfo[0].repeat.button & 2) || (analogInputs[0].repeat & 2) || (((controllerInfo[0].held.button & 2) || (analogInputs[0].held & 2)) && (analogInputs[0].held & 0x200)))
        {
            var_r28++;
            if (var_r28 >= 12)
                var_r28 = 0;
        }
        if (var_r28 != modeCtrl.unk10)
        {
            func_800AB2A0(var_r28, 0);
            modeCtrl.unk10 = var_r28;
        }
    }
    if (!(lbl_802C6180.unk0 & 1))
    {
        var_r4 = lbl_802C6180.unkB[0];
        if ((controllerInfo[0].repeat.button & 8) || (analogInputs[0].repeat & 8) || (((controllerInfo[0].held.button & 8) || (analogInputs[0].held & 8)) && (analogInputs[0].held & 0x200)))
        {
            var_r4--;
            if (var_r4 < 0)
                var_r4 = 4;
        }
        if ((controllerInfo[0].repeat.button & 4) || (analogInputs[0].repeat & 4) || (((controllerInfo[0].held.button & 4) || (analogInputs[0].held & 4)) && (analogInputs[0].held & 0x200)))
        {
            var_r4++;
            if (var_r4 >= 5)
                var_r4 = 0;
        }
        lbl_802C6180.unkB[0] = var_r4;
    }
    if ((controllerInfo[0].pressed.button & 0x100) && !(lbl_802C6180.unk0 & 1) && lbl_802C6180.unkB[0] >= 0)
        func_800AB2A0(modeCtrl.unk10, 1);
    if (controllerInfo[0].pressed.button & 0x800)
    {
        switch (modeCtrl.unk10)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            func_800AB720(&sp28);
            func_800AB720(&sp30);
            func_800AB720(&sp38);
            func_800AB720(&sp40);
            func_800AB720(&sp48);
            func_800AB720(&sp50);
            func_800AB720(&sp58);
            func_800AB720(&sp60);
            if (func_800AC43C(modeCtrl.unk10, &sp28, 4) != 0)
            {
                func_800AB2A0(modeCtrl.unk10, 1);
                printf("entry: TRUE\n");
            }
            else
            {
                func_800AB2A0(modeCtrl.unk10, 0);
                printf("entry: FALSE\n");
            }
            return;
        case 6:
        case 7:
        case 8:
            func_800AB830(&sp20);
            if (func_800AC43C(modeCtrl.unk10, &sp20, 1) != 0)
            {
                func_800AB2A0(modeCtrl.unk10, 1);
                printf("entry: TRUE\n");
            }
            else
            {
                func_800AB2A0(modeCtrl.unk10, 0);
                printf("entry: FALSE\n");
            }
            return;
        case 9:
            func_800AB8D8(&sp18);
            if (func_800AC43C(9, &sp18, 1) != 0)
            {
                func_800AB2A0(9, 1);
                printf("entry: TRUE\n");
            }
            else
            {
                func_800AB2A0(9, 0);
                printf("entry: FALSE\n");
            }
            break;
        case 10:
            func_800AB974(&sp10);
            if (func_800AC43C(0xA, &sp10, 1) != 0)
            {
                func_800AB2A0(0xA, 1);
                printf("entry: TRUE\n");
            }
            else
            {
                func_800AB2A0(10, 0);
                printf("entry: FALSE\n");
            }
            break;
        case 11:
            func_800ABA10(&sp8);
            if (func_800AC43C(11, &sp8, 1) != 0)
            {
                func_800AB2A0(11, 1);
                printf("entry: TRUE\n");
            }
            else
            {
                func_800AB2A0(11, 0);
                printf("entry: FALSE\n");
            }
            break;
        }
    }
}

void lbl_800A7A6C(void)
{
    func_800AB444();
}

void func_800A7A8C(int arg0, u32 arg1)
{
    int i;

    lbl_802C6180.unk4 = arg0;
    lbl_802C6180.unk0 |= arg1;
    lbl_802C6180.unkF = 0;
    lbl_802C6180.unk10 = 2;
    lbl_802C6180.unk8 = 0;
    lbl_802C6180.unk9 = 0;

    for (i = 0; i < 3; i++)
        lbl_802C6180.unk11[i] = 0;

    for (i = 0; i < 4; i++)
    {
        if (lbl_802C6180.unkB[i] >= 0)
        {
            lbl_802C6180.unkA = i;
            break;
        }
    }

    if (lbl_802C6180.unk0 & 1)
    {
        lbl_802C6180.unk7 = 8;
        lbl_802C6180.unk1C = 412.0f;
        lbl_802C6180.unk20 = 118.0f;
        lbl_802C6180.unk24 = 60.0f;
    }
    else
    {
        lbl_802C6180.unk7 = 0;
        lbl_802C6180.unk1C = 500.0f;
        lbl_802C6180.unk20 = 122.0f;
        lbl_802C6180.unk24 = 68.0f;
    }
    if ((lbl_802C6180.unk0 & 1) && (lbl_802C6180.unk0 & 6))
        lbl_802C6180.unk28 = 43.5f;
    else
        lbl_802C6180.unk28 = 60.0f;
}

void func_800A7B7C(void)
{
    BOOL bvar = FALSE;
    int i;
    
    for (i = lbl_802C6180.unkA + 1; i < 4; i++)
    {
        if (lbl_802C6180.unkB[i] >= 0)
        {
            lbl_802C6180.unkA = i;
            lbl_802C6180.unkF = 0;
            lbl_802C6180.unk10 = 2;
            lbl_802C6180.unk8 = 0;
            lbl_802C6180.unk9 = 0;
            lbl_802C6180.unk11[0] = 0;
            lbl_802C6180.unk11[1] = 0;
            lbl_802C6180.unk11[2] = 0;
            bvar = TRUE;
            break;
        }
    }
    if (!bvar)
        lbl_802C6180.unk0 &= ~1;
}

struct UnkStruct
{
    char unk0[3];
    u8 unk3;
    char unk4[3];
    u8 unk7;
};

struct Struct801D5960
{
    void (*unk0)();
    struct UnkStruct *unk4;
    u32 unk8;
    char *unkC;
};

void func_800AADC4();
void func_800AAE80();
void func_800AAF40();
void func_800AB174();

extern char lbl_801D5A20[];
extern char lbl_801D5A50[];
extern char lbl_801D5A80[];
extern char lbl_801D5AB0[];
extern char lbl_801D5AE0[];
extern char lbl_801D5B10[];
extern char lbl_801D5B40[];

extern char lbl_801D5B68[];
extern char lbl_801D5B90[];
extern char lbl_801D5BB8[];
//extern char lbl_801D5BD6[];
extern char lbl_801D5BF4[];

struct Struct801D5960 lbl_801D5960[] =  // 0xB8
{
    {func_800AADC4, (void *)lbl_801D5A20, 8, "spr_bg/bg_rac.gct"},
    {func_800AADC4, (void *)lbl_801D5A50, 8, "spr_bg/bg_rac.gct"},
    {func_800AADC4, (void *)lbl_801D5A80, 8, "spr_bg/bg_rac.gct"},
    {func_800AADC4, (void *)lbl_801D5AB0, 8, "spr_bg/bg_rac.gct"},
    {func_800AADC4, (void *)lbl_801D5AE0, 8, "spr_bg/bg_rac.gct"},
    {func_800AADC4, (void *)lbl_801D5B10, 8, "spr_bg/bg_rac.gct"},

    {func_800AAE80, (void *)lbl_801D5B40, 8, "spr_bg/bg_tgt.gct"},
    {func_800AAE80, (void *)lbl_801D5B68, 8, "spr_bg/bg_tgt.gct"},
    {func_800AAE80, (void *)lbl_801D5B90, 8, "spr_bg/bg_tgt.gct"},

    {func_800AAF40, (void *)lbl_801D5BB8, 6, "spr_bg/bg_bwl.gct"},
    {func_800AAF40, (void *)(lbl_801D5BB8+34), 6, "spr_bg/bg_bwl.gct"},

    {func_800AAF40, (void *)lbl_801D5BF4, 8, "spr_bg/bg_glf.gct"},
};

char lbl_801D5A20[] =
{
    0x4A, 0x41, 0x4D, 0x00,
    0x63, 0x3B, 0x63, 0x00,
    0x4D, 0x4B, 0x41, 0x01,
    0x63, 0x3B, 0x63, 0x00,
    0x40, 0x52, 0x49, 0x02,
    0x63, 0x3B, 0x63, 0x00,
    0x48, 0x49, 0x52, 0x03,
    0x63, 0x3B, 0x63, 0x00,
    0x55, 0x45, 0x44, 0x00,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5A48[] =
{
    0x4E, 0x41, 0x47, 0x01,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5A50[] =
{
    0x44, 0x41, 0x49, 0x00,
    0x63, 0x3B, 0x63, 0x00,
    0x4B, 0x4D, 0x4B, 0x01,
    0x63, 0x3B, 0x63, 0x00,
    0x54, 0x42, 0x43, 0x02,
    0x63, 0x3B, 0x63, 0x00,
    0x4F, 0x44, 0x41, 0x03,
    0x63, 0x3B, 0x63, 0x00,
    0x4D, 0x41, 0x53, 0x00,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5A78[] =
{
    0x4E, 0x41, 0x47, 0x01,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5A80[] =
{
    0x49, 0x5A, 0x41, 0x00,
    0x63, 0x3B, 0x63, 0x00,
    0x4D, 0x43, 0x49, 0x01,
    0x63, 0x3B, 0x63, 0x00,
    0x4D, 0x49, 0x43, 0x02,
    0x63, 0x3B, 0x63, 0x00,
    0x4D, 0x2E, 0x53, 0x03,
    0x63, 0x3B, 0x63, 0x00,
    0x4E, 0x49, 0x4E, 0x00,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5AA8[] =
{
    0x4E, 0x41, 0x47, 0x01,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5AB0[] =
{
    0x4D, 0x41, 0x32, 0x00,
    0x63, 0x3B, 0x63, 0x00,
    0x4E, 0x2E, 0x48, 0x01,
    0x63, 0x3B, 0x63, 0x00,
    0x59, 0x2E, 0x53, 0x02,
    0x63, 0x3B, 0x63, 0x00,
    0x41, 0x47, 0x45, 0x03,
    0x63, 0x3B, 0x63, 0x00,
    0x53, 0x41, 0x4B, 0x00,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5AD8[] =
{
    0x4E, 0x41, 0x47, 0x01,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5AE0[] =
{
    0x4B, 0x41, 0x4B, 0x00,
    0x63, 0x3B, 0x63, 0x00,
    0x59, 0x53, 0x4D, 0x01,
    0x63, 0x3B, 0x63, 0x00,
    0x59, 0x2E, 0x4D, 0x02,
    0x63, 0x3B, 0x63, 0x00,
    0x4D, 0x49, 0x45, 0x03,
    0x63, 0x3B, 0x63, 0x00,
    0x54, 0x4D, 0x59, 0x00,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5B08[] =
{
    0x4E, 0x41, 0x47, 0x01,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5B10[] =
{
    0x59, 0x41, 0x55, 0x00,
    0x63, 0x3B, 0x63, 0x00,
    0x48, 0x2E, 0x45, 0x01,
    0x63, 0x3B, 0x63, 0x00,
    0x4A, 0x55, 0x4E, 0x02,
    0x63, 0x3B, 0x63, 0x00,
    0x54, 0x4F, 0x4B, 0x03,
    0x63, 0x3B, 0x63, 0x00,
    0x53, 0x48, 0x4F, 0x00,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5B38[] =
{
    0x4E, 0x41, 0x47, 0x01,
    0x63, 0x3B, 0x63, 0x00,
};

char lbl_801D5B40[] =
{
    0x53, 0x41, 0x4B, 0x00,
    0x00, 0x32, 0x00, 0x00,
    0x4D, 0x2E, 0x53, 0x01,
    0x00, 0x28, 0x00, 0x00,
    0x4F, 0x44, 0x41, 0x02,
    0x00, 0x1E, 0x00, 0x00,
    0x4D, 0x4B, 0x41, 0x03,
    0x00, 0x14, 0x00, 0x00,
    0x4A, 0x41, 0x4D, 0x00,
    0x00, 0x0A, 0x00, 0x00,
};

char lbl_801D5B68[] =
{
    0x59, 0x53, 0x4D, 0x00,
    0x00, 0x32, 0x00, 0x00,
    0x4E, 0x2E, 0x48, 0x01,
    0x00, 0x28, 0x00, 0x00,
    0x54, 0x42, 0x43, 0x02,
    0x00, 0x1E, 0x00, 0x00,
    0x48, 0x49, 0x52, 0x03,
    0x00, 0x14, 0x00, 0x00,
    0x40, 0x52, 0x49, 0x00,
    0x00, 0x0A, 0x00, 0x00,
};

char lbl_801D5B90[] =
{
    0x53, 0x48, 0x4F, 0x00,
    0x00, 0x32, 0x00, 0x00,
    0x41, 0x47, 0x45, 0x01,
    0x00, 0x28, 0x00, 0x00,
    0x59, 0x2E, 0x53, 0x02,
    0x00, 0x1E, 0x00, 0x00,
    0x4A, 0x55, 0x4E, 0x03,
    0x00, 0x14, 0x00, 0x00,
    0x48, 0x2E, 0x45, 0x00,
    0x00, 0x0A, 0x00, 0x00,
};

char lbl_801D5BB8[] =
{
    0x59, 0x2E, 0x4D, 0x00,
    0x00, 0x32, 0x4D, 0x41,
    0x32, 0x01, 0x00, 0x28,
    0x4D, 0x43, 0x49, 0x02,
    0x00, 0x1E, 0x4D, 0x49,
    0x43, 0x03, 0x00, 0x14,
    0x4E, 0x49, 0x4E, 0x00,
    0x00, 0x0A,
/*};

char lbl_801D5BD6[] =
{*/
    0x49, 0x5A,
    0x41, 0x00, 0x00, 0x03,
    0x4B, 0x41, 0x4B, 0x01,
    0x00, 0x02, 0x59, 0x41,
    0x55, 0x02, 0x00, 0x02,
    0x4D, 0x49, 0x45, 0x03,
    0x00, 0x01, 0x4D, 0x41,
    0x53, 0x00, 0x00, 0x01,
};

char lbl_801D5BF4[] =
{
    0x54, 0x4F, 0x4B, 0x00,
    0x00, 0x63, 0x00, 0x63,
    0x44, 0x41, 0x49, 0x01,
    0x00, 0x63, 0x00, 0x63,
    0x4B, 0x4D, 0x4B, 0x02,
    0x00, 0x63, 0x00, 0x63,
    0x54, 0x4D, 0x59, 0x03,
    0x00, 0x63, 0x00, 0x63,
    0x55, 0x45, 0x44, 0x00,
    0x00, 0x63, 0x00, 0x63,
};

struct UnkStruct *lbl_801D5C1C[] = // 0x374
{
    (void *)lbl_801D5A48,
    (void *)lbl_801D5A78,
    (void *)lbl_801D5AA8,
    (void *)lbl_801D5AD8,
    (void *)lbl_801D5B08,
    (void *)lbl_801D5B38,
};

NLsprarg lbl_801D58A8_38C =
{
    0x319, 112.00000000, 23.00000000, 9.00000000, 1.00000000, 1.00000000, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_3DC =
{
    0x31A, 76.00000000, 0.00000000, 9.00000000, 1.00000000, 1.00000000, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_42C =
{
    0x324, 53.00000000, 91.00000000, 1.00000000, 0.49107143, 0.16071428, 0.00000000, 0.00000000, 0.49107143, 0.16071428, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_47C =
{
    0x324, 196.00000000, 91.00000000, 1.00000000, 0.50000000, 0.16071428, 0.00000000, 0.16071428, 0.50000000, 0.32142857, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_4CC =
{
    0x324, 425.00000000, 91.00000000, 1.00000000, 0.57142860, 0.16071428, 0.00000000, 0.32142857, 0.57142860, 0.48214287, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_51C =
{
    0x324, 372.00000000, 91.00000000, 1.00000000, 0.58035713, 0.16071428, 0.00000000, 0.48214287, 0.58035713, 0.64285713, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_56C =
{
    0x324, 372.00000000, 91.00000000, 1.00000000, 0.69642860, 0.16071428, 0.00000000, 0.80357140, 0.69642860, 0.96428573, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_5BC =
{
    0x324, 513.00000000, 91.00000000, 1.00000000, 0.69642860, 0.16071428, 0.00000000, 0.80357140, 0.69642860, 0.96428573, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_60C =
{
    0x324, 376.00000000, 91.00000000, 1.00000000, 0.44642857, 0.16071428, 0.49107143, 0.00000000, 0.93750000, 0.16071428, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_65C =
{
    0x324, 371.00000000, 91.00000000, 1.00000000, 0.38392857, 0.16071428, 0.50000000, 0.16071428, 0.88392860, 0.32142857, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_6AC =
{
    0x324, 315.00000000, 91.00000000, 1.00000000, 0.38392857, 0.16071428, 0.57142860, 0.32142857, 0.95535713, 0.48214287, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_6FC =
{
    0x314, 0.00000000, 0.00000000, 5.00000000, 2.50000000, 1.00000000, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

u32 lbl_801D58A8_74C[] =
{
    0xFFEF0101,
    0xFF4E7DFF,
    0xFFF1DE21,
    0xFF07A55B,
};

NLsprarg lbl_801D6004 =
{
    0x325, 0.00000000, 0.00000000, 1.00000000, 0.25000000, 0.25000000, 0.00000000, 0.50000000, 0.25000000, 0.75000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D6054 =
{
    0x325, 0.00000000, 0.00000000, 1.00000000, 0.25000000, 0.25000000, 0.00000000, 0.00000000, 0.25000000, 0.25000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D60A4 =
{
    0x325, 0.00000000, 0.00000000, 1.00000000, 0.25000000, 0.25000000, 0.25000000, 0.25000000, 0.50000000, 0.50000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D60F4 =
{
    0x325, 0.00000000, 0.00000000, 1.00000000, 0.25000000, 0.25000000, 0.50000000, 0.75000000, 0.75000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D6144 =
{
    0x325, 0.00000000, 0.00000000, 1.00000000, 0.25000000, 0.25000000, 0.75000000, 0.00000000, 1.00000000, 0.25000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_8EC =
{
    0x316, 0.00000000, 0.00000000, 3.00000000, 4.50000000, 4.50000000, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFF0000FF, 0x00000000
};

NLsprarg *lbl_801D61E4[] =
{
    &lbl_801D6004,
    &lbl_801D60A4,
    &lbl_801D60F4,
    &lbl_801D6144,
};

NLsprarg lbl_801D58A8_94C =
{
    0x31B, 12.00000000, 407.00000000, 1.00000000, 68.50000000, 1.00000000, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_99C =
{
    0x318, 58.00000000, 420.00000000, 1.00000000, 1.00000000, 1.00000000, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_9EC =
{
    0x317, 48.00000000, 424.00000000, 1.00000000, 1.00000000, 1.00000000, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 5, 0xFFFFFFFF, 0x00000000
};

NLsprarg lbl_801D58A8_A3C =
{
    0xF, 0.00000000, 0.00000000, 1.00000000, 0.50000000, 0.32812500, 0.00000000, 0.00000000, 1.00000000, 1.00000000, 0, 1.00000000, -1, 10, 0xFFFFFFFF, 0x00000000
};

static const u16 lbl_802F5C48[] = {0xF, 0x48, 0x48, 0x48};
static const u16 lbl_80171B80[] = {0x20, 0x20, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31, 0x31};
static const u16 lbl_80171B94[] = {0x33, 0x33, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41};
static const u16 lbl_80171BA4[] = {0x3, 0x5D, 0x5D, 0x5D, 0x5D};

// some other sdata2 here

struct Struct801D58A8_A8C
{
    const u16 *unk0;
    u16 unk4;
    u16 unk6;
} lbl_801D58A8_A8C[4] =
{
    {lbl_802F5C48, 4, 0},
    {lbl_80171B80, 9, 0},
    {lbl_80171B94, 8, 0},
    {lbl_80171BA4, 5, 0},
};

NLsprarg lbl_801D58A8_AAC =
{
    0x31C, 0, 0, 0.1, 1, 1, 0, 0, 1, 1, 0, 1, -1, 5, 0xFFFFFFFF, 0x00000000, 0, 0, 0, 0
};

NLsprarg lbl_801D58A8_AFC =
{
    0x320, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, -1, 5, 0xFFFFFFFF, 0x00000000, 0, 0, 0, 0
};

NLsprarg lbl_801D58A8_B4C =
{
    0x31F, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 1, -1, 5, 0xFFFFFFFF, 0x00000000, 0, 0, 0, 0
};

NLsprarg lbl_801D58A8_B9C =
{
    0x31F, 0, 0, 1, 0.84090906, 1, 0, 0, 0.84090906, 1, 0, 1, -1, 5, 0xFFFFFFFF, 0x00000000, 0, 0, 0, 0
};

NLsprarg lbl_801D58A8_BEC =
{
    0x320, 0, 0, 0.5, 1, 1, 0.5, 0, 1.5, 1, 0, 1, -1, 5, 0xFFFFFFFF, 0x00000000, 0, 0, 0, 0
};

float camPerspAngle = 60.0f;
u16 lbl_802F16BC = 0xF300;

char *lbl_801D58A8_C3C[6] =
{
    "SEX",
    "XXX",
    "XX ",
    "XX",
    " XX",
    "X X",
};

static void inlinetest(char *var_r24)
{
    u32 var_r22;

    for (var_r22 = 0; var_r22 < 6; var_r22++)
    {
        if (strncmp(var_r24, lbl_801D58A8_C3C[var_r22], 3) == 0)
        {
            strncpy(var_r24, "---", 3);
            break;
        }
    }
    func_800A7B7C();
}

void func_800A7C10(s8 *arg0, struct Sprite *arg1)
{
  u32 temp_r4_2;
  u32 var_r22;
  char *var_r24;
  char *var_r23;
  s32 var_r25;
  u32 temp;
  char sp10[4];
  if (lbl_802C6180.unk0 & 1)
  {
    if ((++lbl_802C6180.unk7) > 8)
    {
      lbl_802C6180.unk7 = 8;
    }
    lbl_802C6180.unk1C += 0.2 * (412.0 - lbl_802C6180.unk1C);
    lbl_802C6180.unk20 += 0.2 * (118.0 - lbl_802C6180.unk20);
    lbl_802C6180.unk24 += 0.2 * (60.0 - lbl_802C6180.unk24);
    lbl_802C6180.unk9 = 0;
  }
  else
  {
    if ((--lbl_802C6180.unk7) < 0)
    {
      lbl_802C6180.unk7 = 0;
    }
    lbl_802C6180.unk1C += 0.2 * (500.0 - lbl_802C6180.unk1C);
    lbl_802C6180.unk20 += 0.2 * (122.0 - lbl_802C6180.unk20);
    lbl_802C6180.unk24 += 0.2 * (68.0 - lbl_802C6180.unk24);
    if (((s8) lbl_802C6180.unk9) < 60)
    {
      lbl_802C6180.unk9 = lbl_802C6180.unk9 + 1;
    }
  }
  if ((lbl_802C6180.unk0 & 1) && (lbl_802C6180.unk0 & 6))
  {
    lbl_802C6180.unk28 += 0.2 * (43.5 - lbl_802C6180.unk28);
  }
  else
  {
    lbl_802C6180.unk28 += 0.2 * (60.0 - lbl_802C6180.unk28);
  }
  if (((s8) lbl_802C6180.unk8) < 8)
  {
    lbl_802C6180.unk8 = lbl_802C6180.unk8 + 1;
  }
  var_r22 = 255 - (u32) ((127.0f * lbl_802C6180.unk7) / 8.0f);
  lbl_802C6180.unk18 = RGBA(var_r22, var_r22, var_r22, 255);
  if (lbl_802C6180.unk0 & 1)
  {

    temp = temp_r4_2 = playerControllerIDs[lbl_802C6180.unkA];
    if ((lbl_802C6180.unk0 & 4) && (lbl_802C6180.unkA == lbl_802C6180.unk5))
    {
      var_r24 = lbl_801D5C1C[lbl_802C6180.unk4]->unk0;
    }
    else
    {
      var_r24 = lbl_801D5960[lbl_802C6180.unk4].unk4->unk0;
      var_r24 += lbl_802C6180.unkB[lbl_802C6180.unkA] * lbl_801D5960[lbl_802C6180.unk4].unk8;
    }
    if (lbl_802C6180.unkF < 3)
    {
      if ((lbl_802C6180.unk0 & 4) && (lbl_802C6180.unkA == lbl_802C6180.unk5))
      {
        var_r23 = var_r24 + lbl_802C6180.unkF;
      }
      else
      {
        var_r23 = var_r24 + lbl_802C6180.unkF;
      }
      var_r25 = lbl_802C6180.unk10;
      temp = temp_r4_2;
      if ((controllerInfo[(u32)temp_r4_2].repeat.button & 1) || (analogInputs[(u32)temp_r4_2].repeat & 1))
      {
        var_r25--;
        if (var_r25 < 0)
        {
          var_r25 = 0x2C;
        }
      }
      if ((controllerInfo[temp].repeat.button & 2) || (analogInputs[temp].repeat & 2))
      {
        var_r25++;
        if (var_r25 >= 0x2DU)
        {
          var_r25 = 0;
        }
      }
      if (var_r25 != lbl_802C6180.unk10)
      {
        u_play_sound_0(0x6C);
      }
      lbl_802C6180.unk10 = var_r25;
      *var_r23 = lbl_801D58A8[var_r25];

    }
    else
    {
      var_r23 = sp10;
      var_r25 = lbl_802C6180.unk10;
      if ((controllerInfo[temp].repeat.button & 1) || (analogInputs[temp].repeat & 1))
      {
        var_r25--;
        if (var_r25 < 0)
        {
          var_r25 = 1;
        }
      }
      if ((controllerInfo[temp].repeat.button & 2) || (analogInputs[temp].repeat & 2))
      {
        var_r25++;
        if (var_r25 >= 2)
        {
          var_r25 = 0;
        }
      }
      if (var_r25 != lbl_802C6180.unk10)
      {
        u_play_sound_0(0x6C);
      }
      lbl_802C6180.unk10 = var_r25;
      *var_r23 = lbl_801D58A8[var_r25];
    }
    if ((controllerInfo[temp].pressed.button & 0x100) && (((s8) lbl_802C6180.unk8) >= 8))
    {
      switch (*var_r23)
      {
        case 0x5C:
          if (lbl_802C6180.unkF > 0)
        {
          if (lbl_802C6180.unkF < 3)
          {
            *var_r23 = 0;
          }
          lbl_802C6180.unkF -= 1;
          u_play_sound_0(0x66);
        }
          break;

        case 0x5D:
          *var_r23 = 0;
            inlinetest(var_r24);
            /*
          for (var_r22 = 0; var_r22 < 6; var_r22++)
        {
          if (strncmp(var_r24, lbl_801D58A8_C3C[var_r22], 3) == 0)
          {
            strncpy(var_r24, "---", 3);
            break;
          }
        }*/

          u_play_sound_0(0x6A);
          break;

        case 0x5B:
          *var_r23 = 0x20;

        default:
          lbl_802C6180.unk11[lbl_802C6180.unkF] = lbl_802C6180.unk10;
          lbl_802C6180.unkF += 1;
          if (lbl_802C6180.unkF >= 3)
        {
          lbl_802C6180.unk10 = 1;
        }
        else
          if (lbl_802C6180.unkF >= 4)
        {
            inlinetest(var_r24);
        }
          if (lbl_802C6180.unkF < 4)
        {
          u_play_sound_0(0x65);
        }
        else
        {
          u_play_sound_0(0x6A);
        }
          break;

      }

    }
    if ((controllerInfo[temp].pressed.button & 0x200) && (lbl_802C6180.unk7 >= 8))
    {
      if (lbl_802C6180.unkF > 0)
      {
        if (lbl_802C6180.unkF < 3)
        {
          *var_r23 = 0;
        }
        lbl_802C6180.unkF -= 1;
        lbl_802C6180.unk10 = lbl_802C6180.unk11[lbl_802C6180.unkF];
        u_play_sound_0(0x66);
      }
    }
    if (lbl_802C6180.unk0 & 2)
    {
      int foo = lbl_802C6180.unk4;
      char *src = &lbl_801D5960[foo].unk4->unk0[lbl_802C6180.unkB[lbl_802C6180.unk5] * lbl_801D5960[lbl_802C6180.unk4].unk8];
      memcpy(lbl_801D5C1C[foo], src, 3);
    }
  }
}

void func_800A82D0(void)
{
    struct Sprite sp8;

    preview_draw(&lbl_802C6180.unk2C, -1U, 0U, 0.0f, 0.0f, 10.0f, 640.0f, 480.0f);
    nlSprPut(&lbl_801D58A8_38C);
    lbl_801D58A8_3DC.y = lbl_802C6180.unk1C;
    nlSprPut(&lbl_801D58A8_3DC);
    sp8.depth = 0.1f;
    sp8.scaleX = 1.0f;
    sp8.scaleY = 1.0f;
    sp8.fontId = 0xB3;
    sp8.addR = 0;
    sp8.addG = 0;
    sp8.addB = 0;
    sp8.flags = 0x200000;
    sp8.mulR = 0xFF;
    sp8.mulG = 0xFF;
    sp8.mulB = 0;
    strcpy(sp8.text, "Please enter your name.");
    sp8.x = 145.0f;
    sp8.y = 423.0 + lbl_802C6180.unk1C - 412.0;
    func_800A916C(&sp8);
}

float lbl_801D6514[] = {0.0f, 40.0f, 84.0f};

struct
{
    char *unk0;
    u32 unk4;
} lbl_801D58A8_CF4[] =
{
    {"a/-Jungle Circuit-", 10},
    {"a/-Aqua Offroad-", 8},
    {"a/-Frozen Highway-", 10},
    {"a/-Sky Downtown-", 9},
    {"a/-Pipe Warp Tunnel-", 10},
    {"a/-Speed Desert-", 8},
};

void func_800A83C4(s32 arg0, f32 arg8, f32 arg9)
{
    u8 unused[8];
    BOOL var_r7 = 0;
    int i;
    u32 origColor;
    u32 convertedColor;
    float alphaScale;
    u8 r;
    u8 g;
    u8 b;
    u8 a;
    NLsprarg* r4;
    for (i = 0; i < 4; i++) {
        if (arg0 == lbl_802C6180.unkB[i]) {
            var_r7 = 1;
            break;
        }
    }

    r4 = &lbl_801D58A8_6FC;
    if (var_r7) {
        if (lbl_802C6180.unk6 == 1) {
            origColor = lbl_801D58A8_74C[3];
        } else {
            origColor = lbl_801D58A8_74C[i];
        }
        r4->sprno = 0x326;
        if (arg0 != lbl_802C6180.unkB[lbl_802C6180.unkA]) {

            alphaScale = 1.0 - ((0.5 * lbl_802C6180.unk7) / 8.0f);
            a = (origColor >> 24) & 0xFF;
            r = (origColor >> 16) & 0xFF;
            g = (origColor >> 8) & 0xFF;
            b = origColor & 0xFF;
            convertedColor = (((u8)(a * alphaScale)) << 24) | (((u8)(r * alphaScale)) << 16);
            r4->base_color = (convertedColor | (((u8)(g * alphaScale)) << 8)) | ((u8)(b * alphaScale));

        } else {
            r4->base_color = origColor;
        }
    } else {
        r4->sprno = 0x314;
        r4->base_color = (arg0 != lbl_802C6180.unkB[lbl_802C6180.unkA])
            ? (lbl_802C6180.unk18)
            : ((((255 << 24) | (255 << 16)) | (255 << 8)) | 255);
    }
    r4->x = arg8;
    r4->y = arg9;
    nlSprPut(r4);
}

void func_800A8828(char *arg0, s32 arg1, f32 arg8, f32 arg9)
{
  u8 unused[4];
  int var_r29;
  u32 dumb;
  u32 var_r3;
  NLsprarg *params;

  reset_text_draw_settings();
  set_text_font(0x20);
  for (var_r29 = 0; *arg0 != 0 && var_r29 < 3; var_r29++, arg0++)
  {
    func_80071B1C(2.0f);
    set_text_mul_color(0xFF000000U);

    if (((lbl_802C6180.unk0 & 1) && (arg1 == lbl_802C6180.unkB[lbl_802C6180.unkA])) && (var_r29 == ((s8) lbl_802C6180.unkF)))
    {
      var_r3 = globalAnimTimer % 60;
      if (var_r3 > 30u)
      {
        var_r3 = 60 - var_r3;
      }
      var_r3 = var_r3 * 4;
      var_r3 = var_r3 | ((var_r3 << 0x10) | (var_r3 << 8));
      set_text_add_color(var_r3);
    }
    else if (!(lbl_802C6180.unk0 & 1))
    {
      if ((lbl_802C6180.unk0 & 1))
      {
        var_r3 = 0;
      }
      else if (!(lbl_802C6180.unk14 & (1 << arg1)))
      {
        var_r3 = 0;
      }
      else
      {
        var_r3 = globalAnimTimer % 60;
        if (var_r3 > 30u)
        {
          var_r3 = 60 - var_r3;
        }
        var_r3 = var_r3 * 8;
        var_r3 = var_r3 | ((var_r3 << 0x10) | (var_r3 << 8));
      }
      set_text_add_color(var_r3);
    }
    set_text_pos(arg8 + (var_r29 * 0x24) + 2.0f,  arg9 + 2.0f);
    dumb = *arg0;
    sprite_printf("%c", dumb);
    func_80071B1C(1.0f);
    if (arg1 != lbl_802C6180.unkB[lbl_802C6180.unkA])
    {
      var_r3 = lbl_802C6180.unk18;
    }
    else
    {
      var_r3 = -1U;
    }
    set_text_mul_color(var_r3);
    set_text_pos(arg8 + (var_r29 * 0x24), arg9);
    sprite_printf("%c", *arg0);
  }

  if ((lbl_802C6180.unk0 & 1) && (arg1 == lbl_802C6180.unkB[lbl_802C6180.unkA]))
  {
    params = &lbl_801D58A8_8EC;
    params->x = arg8 + (lbl_802C6180.unkF * 0x24) - 2.0f;
    params->y = arg9 - 2.0f;
    var_r3 = (0, globalAnimTimer % 60);
    if (var_r3 > 30u)
    {
      var_r3 = 60 - var_r3;
    }
    var_r3 = var_r3 * 8;
    var_r3 = (((0 << 24) | (var_r3 << 16)) | (var_r3 << 8)) | var_r3;
    params->offset_color = var_r3;
    nlSprPut(params);
    if (lbl_802C6180.unkF >= 3)
    {
      set_text_scale(1.0f, 1.0f);
      func_80071B1C(2.0f);
      set_text_mul_color(0xFF000000U);
      if (var_r29 == lbl_802C6180.unkF)
      {
        var_r3 = globalAnimTimer % 60;
        if (var_r3 > 30u)
        {
          var_r3 = 60 - var_r3;
        }
        var_r3 = var_r3 * 4;
        var_r3 = (((0 << 24) | (var_r3 << 16)) | (var_r3 << 8)) | var_r3;
        set_text_add_color(var_r3);
      }
      set_text_pos(arg8 + (var_r29 * 0x24) + 2.0f,  arg9 + 2.0f);
      sprite_printf("%c", lbl_801D58A8[lbl_802C6180.unk10]);
      func_80071B1C(1.0f);
      if (arg1 != lbl_802C6180.unkB[lbl_802C6180.unkA])
      {
        var_r3 = lbl_802C6180.unk18;
      }
      else
      {
        var_r3 = -1U;
      }
      set_text_mul_color(var_r3);
      set_text_pos(arg8 + (var_r29 * 0x24), arg9);
      sprite_printf("%c", lbl_801D58A8[lbl_802C6180.unk10]);
    }
  }
}

void func_800A8C44(char *arg0, s32 arg1, float arg8, float arg9)
{
    s32 temp_r24;
    u32 temp_r28;
    s32 var_r27;
    u32 var_r3;
    int temp_r0;
    int var_r23;

    reset_text_draw_settings();
    set_text_font(0x21);
    if (lbl_802C6180.unk0 & 1)
    {
        var_r3 = 0;
    }
    else if (!(lbl_802C6180.unk14 & (1 << arg1)))
    {
        var_r3 = 0;
    }
    else
    {
        var_r3 = globalAnimTimer % 60;
        if (var_r3 > 0x1EU)
        {
            var_r3 = 60 - var_r3;
        }
        var_r3 = var_r3 * 8;
        var_r3 = RGBA(var_r3, var_r3, var_r3, 0);
    }
    set_text_add_color(var_r3);
    for (var_r27 = 0; var_r27 < 3; var_r27++, arg0++)
    {
        temp_r0 = (u8)*arg0;
        if (var_r27 == 1)
        {
            if (temp_r0 < 0)
            {
                var_r23 = 0;
            }
            else if (temp_r0 > 0x3B)
            {
                var_r23 = 0x3B;
            }
            else
            {
                var_r23 = temp_r0;
            }
        }
        else if (temp_r0 < 0)
        {
            var_r23 = 0;
        }
        else if (temp_r0 > 0x63)
        {
            var_r23 = 0x63;
        }
        else
        {
            var_r23 = temp_r0;
        }
        func_80071B1C(2.0f);
        set_text_mul_color(0xFF000000U);
        set_text_pos((2.0 + (arg8 + lbl_801D6514[var_r27])), arg9 + 2.0);
        temp_r28 = var_r23 / 10;
        sprite_printf("%d", temp_r28);
        set_text_pos((2.0 + (17.0 + (arg8 + lbl_801D6514[var_r27]))), arg9 + 2.0);
        temp_r24 = var_r23 % 10;
        sprite_printf("%d", temp_r24);
        func_80071B1C(1.0f);
        if (arg1 != lbl_802C6180.unkB[lbl_802C6180.unkA])
        {
            var_r3 = lbl_802C6180.unk18;
        }
        else
        {
            var_r3 = -1U;
        }
        set_text_mul_color(var_r3);
        set_text_pos(arg8 + lbl_801D6514[var_r27], arg9);
        sprite_printf("%d", temp_r28);
        set_text_pos((17.0 + (arg8 + lbl_801D6514[var_r27])), arg9);
        sprite_printf("%d", temp_r24);
    }
    func_80071B1C(2.0f);
    set_text_mul_color(0xFF000000U);
    set_text_pos(arg8 + 380.0 - 346.0 + 2.0, arg9);
    sprite_puts(":");
    func_80071B1C(1.0f);
    if (arg1 != lbl_802C6180.unkB[lbl_802C6180.unkA])
    {
        var_r3 = lbl_802C6180.unk18;
    }
    else
    {
        var_r3 = -1U;
    }
    set_text_mul_color(var_r3);
    set_text_pos((380.0 + arg8) - 346.0, arg9 - 2.0);
    sprite_puts(":");
    func_80071B1C(2.0f);
    set_text_mul_color(0xFF000000U);
    set_text_pos(2.0 + ((420.0 + arg8) - 346.0), arg9);
    sprite_puts(";");
    func_80071B1C(1.0f);
    if (arg1 != lbl_802C6180.unkB[lbl_802C6180.unkA])
    {
        var_r3 = lbl_802C6180.unk18;
    }
    else
    {
        var_r3 = -1U;
    }
    set_text_mul_color(var_r3);
    set_text_pos(((420.0 + arg8) - 346.0), arg9 - 2.0);
    sprite_puts(";");
}

void func_800A8FCC(s32 arg0, s32 arg1, float arg8, float arg9)
{
    u32 temp_r0;
    u32 temp_r0_2;
    s32 var_r0;
    s32 var_r9;
    u32 temp_r7;
    u16 new_var;
    struct Struct801D58A8_A8C *r29;
    NLsprarg *params;
    s32 var_r28;
    u8 unused[8];

    if (arg0 < 0 || arg0 >= 4)
        return;
    r29 = &lbl_801D58A8_A8C[arg0];
    new_var = r29->unk4;
    temp_r7 = (r29->unk4 * 2) - 2;
    temp_r0 = ((u32) globalAnimTimer >> 3U) % temp_r7;
    var_r9 = temp_r0;
    if ((s32)temp_r0 >= (s32) new_var)
    {
        var_r9 = temp_r7 - var_r9;
    }
    temp_r0_2 = (((u32) globalAnimTimer >> 3U) - 1) % temp_r7;
    var_r28 = temp_r0_2;
    if ((s32)temp_r0_2 >= (s32) new_var)
    {
        var_r28 = temp_r7 - var_r28;
    }
    params = &lbl_801D58A8_A3C;
    params->sprno = r29->unk0[var_r9];
    params->x = 32.0 + arg8;
    params->y = 21.0 + arg9;
    params->z = 1.0f;
    if (arg1 != lbl_802C6180.unkB[lbl_802C6180.unkA])
    {
        var_r0 = lbl_802C6180.unk18;
    }
    else
    {
        var_r0 = -1;
    }
    params->base_color = var_r0;
    params->trnsl = 1.0f;
    params->ang = 4096.0f * mathutil_sin((globalAnimTimer * 0x190) + 0x4000);
    nlSprPut(params);
    params = &lbl_801D58A8_A3C;
    params->sprno = r29->unk0[var_r28];
    params->z = 0.5f;
    params->trnsl = 1.0 - ((f64)(globalAnimTimer & 7) / 8.0f);
    nlSprPut(params);
}

void func_800A916C(struct Sprite *arg0)
{
    reset_text_draw_settings();
    set_text_font(arg0->fontId);
    set_text_scale(arg0->scaleX, arg0->scaleY);
    func_80071B50(arg0->flags);
    func_80071B1C(0.1 + arg0->depth);
    set_text_mul_color(RGBA(0, 0, 0, 0));
    set_text_add_color(RGBA(0, 0, 0, 0));
    set_text_pos(2.0 + arg0->x, 2.0 + arg0->y);
    sprite_puts(arg0->text);
    func_80071B1C(arg0->depth);
    set_text_mul_color(RGBA(arg0->mulR, arg0->mulG, arg0->mulB, 0));
    set_text_add_color(RGBA(arg0->addR, arg0->addG, arg0->addB, 0));
    set_text_pos(arg0->x, arg0->y);
    sprite_puts(arg0->text);
}

void func_800A9254(struct Sprite *arg0, f64 arg1, f64 arg2)
{
    struct Sprite sp20;
    float f30;
    float temp_f29;
    float f21;
    float var_f20;
    s32 var_r0;
    s32 var_r0_2;
    s32 var_r0_3;
    s32 var_r26;
    u32 var_r5;
    struct UnkStruct *temp_r27;
    NLsprarg *params;
    float f1;
    float f0;
    float textWidth;
    float temp_f19;

    func_800A82D0();
    sp20.depth = 0.1f;
    sp20.fontId = 0xB3;
    sp20.addR = 0;
    sp20.addG = 0;
    sp20.addB = 0;
    sp20.mulR = 0xEF;
    sp20.mulG = 0x9A;
    sp20.mulB = 0;
    sp20.flags = 0x200000;
    sp20.scaleX = 0.8f;
    sp20.scaleY = 0.8f;
    strcpy(sp20.text, lbl_801D58A8_CF4[lbl_802C6180.unk4].unk0);
    reset_text_draw_settings();
    set_text_font(sp20.fontId);
    set_text_scale(sp20.scaleX, sp20.scaleY);
    func_80071B50(sp20.flags);
    func_80071B1C(sp20.depth);
    set_text_mul_color(RGBA(sp20.mulR, sp20.mulG, sp20.mulB, 0));
    set_text_add_color(RGBA(sp20.addR, sp20.addG, sp20.addB, 0));
    textWidth = u_get_text_width(&sp20.text[0]);
    sp20.x = 374.0 - (0.5 * textWidth);
    sp20.y = 37.0f;
    reset_text_draw_settings();
    set_text_font((s32) sp20.fontId);
    set_text_scale(sp20.scaleX, sp20.scaleY);
    func_80071B50(sp20.flags);
    func_80071B1C(0.1 + sp20.depth);
    set_text_mul_color(0U);
    set_text_add_color(0U);
    set_text_pos(2.0 + sp20.x, 2.0 + sp20.y);
    sprite_puts(&sp20.text[0]);
    func_80071B1C(sp20.depth);
    set_text_mul_color(RGBA(sp20.mulR, sp20.mulG, sp20.mulB, 0));
    set_text_add_color(RGBA(sp20.addR, sp20.addG, sp20.addB, 0));
    set_text_pos(sp20.x, sp20.y);
    sprite_puts(&sp20.text[0]);
    nlSprPut(&lbl_801D58A8_42C);
    nlSprPut(&lbl_801D58A8_47C);
    nlSprPut(&lbl_801D58A8_60C);
    nlSprPut(&lbl_801D58A8_5BC);
    f21 = 0.0f;
    var_f20 = 118.0f;
    temp_r27 = (void *)lbl_801D5960[lbl_802C6180.unk4].unk4;
    for (var_r26 = 0; var_r26 < 5; var_r26++, temp_r27++)
    {
        func_800A83C4(var_r26, f21, var_f20);
        func_800A85F8(var_r26, 50.0f + f21, var_f20 - 1.0f);
        func_800A8828(temp_r27->unk0, var_r26, 172.0 + f21, var_f20 - 1.0);
        func_800A8C44(temp_r27->unk4, var_r26, 346.0 + f21, 2.0 + var_f20);
        func_800A8FCC(temp_r27->unk3, var_r26, 520.0 + f21, var_f20 - 5.0);
        f1 = 568.0 + f21;
        f0 = var_f20 - 5.0;
        if (temp_r27->unk7 & 1)
        {
            params = &lbl_801D58A8_AAC;
            params->sprno = 0x31C;
            params->x = f1;
            params->y = f0;
            if (var_r26 != lbl_802C6180.unkB[lbl_802C6180.unkA])
            {
                var_r0 = lbl_802C6180.unk18;
            }
            else
            {
                var_r0 = -1;
            }
            params->base_color = var_r0;
            nlSprPut(params);
        }
        var_f20 += lbl_802C6180.unk28;
    }
    if (lbl_802C6180.unk7 < 8 || (lbl_802C6180.unk0 & 6))
    {
        temp_r27 = (void *)lbl_801D5C1C[lbl_802C6180.unk4];
        temp_f19 = 4.0 * (60.0 - lbl_802C6180.unk28);
        temp_f29 = 424.0 - temp_f19;
        f30 = 0.0f;
        if (!(lbl_802C6180.unk0 & 6))
        {
            mathutil_mtxA_from_identity();
            mathutil_mtxA_translate_xyz(320.0f, 224.0f, 0.0f);
            mathutil_mtxA_scale_xyz(1.0 - (lbl_802C6180.unk7 / 8.0f), 1.0f, 1.0f);
            mathutil_mtxA_translate_xyz(-320.0f, -224.0f, 0.0f);
            GXLoadPosMtxImm(mathutilData->mtxA, 0U);
        }
        lbl_801D58A8_94C.y = 407.0 - temp_f19;
        nlSprPut(&lbl_801D58A8_94C);
        params = &lbl_801D58A8_9EC;
        params->x = 48.0f;
        params->y = 423.0 - temp_f19;
        if (var_r26 != lbl_802C6180.unkB[lbl_802C6180.unkA])
        {
            var_r0_2 = lbl_802C6180.unk18;
        }
        else
        {
            var_r0_2 = -1;
        }
        params->base_color = var_r0_2;
        nlSprPut(params);
        if (lbl_802C6180.unk0 & 6)
        {
            NLsprarg *new_var;
            params = lbl_801D61E4[lbl_802C6180.unk5];
            new_var = params;
            params->x = 22.0f;
            params->y = 423.0 - temp_f19 - 10.0;
            var_r5 = globalAnimTimer % 60;
            if (var_r5 > 0x1EU)
            {
                var_r5 = 0x3C - var_r5;
            }
            var_r5 = var_r5 * 4;
            var_r5 = var_r5 | ((var_r5 << 0x10) | (var_r5 << 8));
            params->offset_color = var_r5;
            nlSprPut(new_var);
        }
        func_800A8828(temp_r27->unk0, var_r26, 172.0 + f30, temp_f29 - 1.0);
        func_800A8C44(temp_r27->unk4, var_r26, 346.0 + f30, temp_f29 + 2.0);
        func_800A8FCC(temp_r27->unk3, var_r26, 520.0 + f30, temp_f29 - 6.0);

        f1 = 568.0 + f21;
        f0 = temp_f29 - 5.0;
        if (temp_r27->unk7 & 1)
        {
            params = &lbl_801D58A8_AAC;
            params->sprno = 0x31C;
            params->x = f1;
            params->y = f0;
            if (var_r26 != lbl_802C6180.unkB[lbl_802C6180.unkA])
            {
                var_r0_3 = lbl_802C6180.unk18;
            }
            else
            {
                var_r0_3 = -1;
            }
            params->base_color = var_r0_3;
            nlSprPut(params);
        }
        if (!(lbl_802C6180.unk0 & 6))
        {
            mathutil_mtxA_from_identity();
            GXLoadPosMtxImm(mathutilData->mtxA, 0U);
        }
    }
}
