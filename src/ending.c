#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "event.h"
#include "gxutil.h"
#include "input.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "ord_tbl.h"
#include "rend_efc.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "textbox.h"

struct SomeStruct2
{
    s16 unk0;
    s16 unk2;
    s16 unk4;
};

struct SomeBigEndingStruct_sub2_sub
{
    u8 filler0[0x38];
    u16 unk38;
    u16 unk3A;
    u8 filler3C[0x8EC8-0x3C];
    Mtx unk8EC8;
};

struct SomeBigEndingStruct_sub2  // probably Ape
{
    struct SomeBigEndingStruct_sub2_sub *unk0;
    u8 filler4[0x10-0x4];
    u32 unk10;
    u32 unk14;
    u8 filler18[0x30-0x18];
    Vec unk30;
    Vec unk3C;
    u8 filler48[0x58-0x48];
    float unk58;
    u8 filler5C[0x60-0x5C];
    Quaternion unk60;
    u8 filler70[0x9c-0x70];
    u32 unk9C;
    u8 fillerA0[0xB4-0xA0];
    u32 unkB4;
};

struct SomeBigEndingStruct_sub3
{
    struct SomeBigEndingStruct_sub2 *unk0;  // -0x57fc
    s16 unk4;
    s16 unk6;
    s16 unk8;
    /*0x0A*/ u16 unkA;
    /*0x0C*/ Vec unkC;
    /*0x18*/ Vec unk18;
             u8 filler24[0x30-0x24];
             /*
             s16 unk30;
             s16 unk32;
             s16 unk34;
             */
             struct SomeStruct2 unk30;
             u16 unk36;
    /*0x38*/ s16 unk38;
    /*0x3A*/ s16 unk3A;
    void *unk3C;
    u32 unk40;
    float unk44;
    float unk48;
    float unk4C;
    s16 unk50;
    s16 unk52;
    s16 unk54;
    s16 unk56;
    s16 unk58;
    s16 unk5A;
    s16 unk5C;
    u8 filler5E[0x70-0x5E];
    /*0x70*/ u32 unk70;  // -0x578c
    Vec unk74;
};  // size = 0x80

static_assert(offsetof(struct SomeBigEndingStruct_sub3, unk38) == 0x38, moo);

struct SomeBigEndingStruct_sub
{
    u16 unk0;
    u16 unk2;
    Vec unk4;
    Vec unk10;
    s16 unk1C;
    s16 unk1E;
    Vec unk20;
    Vec unk2C;
    Vec unk38;
    s16 unk44;
    u16 unk46;
    Vec unk48;
};  // size = 0x54

struct SomeStruct
{
    s8 unk0;
    s16 unk2;
    s16 unk4;
    Vec unk8;
    Vec unk14;
    Vec unk20;
    Vec unk2C;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    Vec unk44;
};  // size = 0x50

static_assert(sizeof(struct SomeStruct) == 0x50, bad);

struct SomeBigEndingStruct
{
    float unk0;
    u8 filler4[0xA804-0x4];
    struct SomeBigEndingStruct_sub3 unkA804[4];
    struct SomeStruct unkAA04;
    struct SomeBigEndingStruct_sub unkAA54;
    float unkAAA8;
    float unkAAAC;
    s32 unkAAB0;
    char *unkAAB4[2];
    u8 fillerAABC[0xAACC-0xAABC];
    u32 unkAACC;
    s32 unkAAD0;
};

static_assert(offsetof(struct SomeBigEndingStruct, unkAA54) == 0xAA54, bad);
static_assert(offsetof(struct SomeBigEndingStruct, unkAAB0) == 0xAAB0, bad);

struct
{
    s16 unk0;
    s16 unk2;
    u32 unk4;
    struct SomeBigEndingStruct *unk8;
} lbl_802C6BD8;

void func_800B6430(void);

void func_800B65B4(void);
void func_800B6608(void);
void func_800B6848(void);
void func_800B69F8(void);
void func_800B6C14(void);
void func_800B6EA4(void);
void func_800B6F9C(void);
void func_800B7078(void);
void func_800B72D4(void);
void func_800B7B6C(void);
void func_800B7D38(void);
void func_800B7F6C(void);
void func_800B7FB4(void);
void func_800B83EC(void);
void func_800B86E0(void);
void func_800B8780(void);
void func_800B8AA4(void);
void func_800B8D7C(void);
void func_800B8E1C(void);
void func_800B9444(void);
void func_800B9724(void);
void func_800B9920(void);
void func_800B9A8C(void);
void func_800B9EA4(void);
void func_800B9FDC(void);
void func_800BA124(void);
void func_800BA160(void);
void func_800BA928(void);
void func_800BA950(void);
void func_800BA4F4(void);
void func_800BA51C(void);
void func_800BB050(void);
void func_800BB064(void);

struct Struct801E2A98
{
    Vec unk0;
    struct SomeStruct2 unkC;
};

struct Struct801E2A98 lbl_801E2A98[] =
{
    {{0.7f, -0.4f, 0.7f}, {0, 0x8000, 0}},
    {{-0.675, 0.0f, 0.75f}, {0, 0x8000, 0}},
    {{0.0200000014156, -0.4f, 1.1f}, {0, 0x8000, 0}},
};

Vec lbl_801E2AD4[] =
{
    {  0.0f, 10.0f, 5.5f },
    { -1.2f, 10.0f, 5.0f },
    {  1.2f, 10.0f, 5.0f },
};

Vec lbl_801E2AD4_2[] =
{
    {  0.0f, 10.0f, 4.0f },
    { -0.1f, 10.0f, 5.5f },
    {  0.1f, 10.0f, 6.5f },
    {  0.0f, 10.0f, 9.5f },
};

s16 lbl_801E2B28[][2] =
{
    { 0x00E8, 0x00E8 },
    { 0x0101, 0x0101 },
    { 0x016D, 0x016D },
    { 0x0186, 0x0187 },
};

void (*lbl_801E2B38[])(void) =
{
    func_800B65B4,
    func_800B6608,
    func_800B6848,
    func_800B69F8,
    func_800B6C14,
    func_800B6EA4,
    func_800B6F9C,
    func_800B7078,
    func_800B72D4,
    func_800B7B6C,
    func_800B7D38,
    func_800B7F6C,
    func_800B7FB4,
    func_800B83EC,
    func_800B86E0,
    func_800B8780,
    func_800B8AA4,
    func_800B8D7C,
    func_800B8E1C,
    func_800B9444,
    func_800B9724,
    func_800B9920,
    func_800B9A8C,
    func_800B9EA4,
    func_800B9FDC,
    func_800BA124,
    func_800BA160,
    func_800BA928,
    func_800BA950,
    func_800BA4F4,
    func_800BA51C,
    func_800BB050,
    func_800BB064,
};

// doesn't actually match as an inline function
#define SOME_CAMERA_MACRO() \
do \
{ \
    struct Camera *camera = &cameraInfo[0]; \
    struct Camera *cameraBackup = currentCameraStructPtr; \
    int i; \
    for (i = 0; i < 4; i++, camera++) \
    { \
        currentCameraStructPtr = camera; \
        camera->unk1F = 0; \
    } \
    currentCameraStructPtr = cameraBackup; \
} while (0)

void func_800B6234(void)
{
    memset(&lbl_802C6BD8, 0, sizeof(lbl_802C6BD8));
    lbl_802C6BD8.unk0 = 0;
    lbl_802C6BD8.unk2 = -1;
    event_finish_all();
    lbl_802C6BD8.unk8 = OSAllocFromHeap(stageHeap, 0xC2D0);
    if (lbl_802C6BD8.unk8 == NULL)
    {
        printf("fail to alloc memory for ending-management!\n");
        lbl_802C6BD8.unk0 = 31;
        return;
    }
    memset(lbl_802C6BD8.unk8, 0, 0xC2D0);
    SoundGroupLoad(0x10);
    func_800BBB78();
    func_800BD470();
    func_800BB068();
    func_800C008C();
    func_800C09B0();
    lbl_802F1B7C = func_800B6430;
}

#define lbl_802F64C8 0.01666666753590107f
#define lbl_802F64CC 400.0f
#define lbl_802F64D0 0.0f
#define lbl_802F64D4 20.0f
#define lbl_802F64D8 -64.0f
#define lbl_802F64DC 250.0f
#define lbl_802F64E0 100.0f
#define lbl_802F64F8 0.5f
#define lbl_802F64FC 20.75f
#define lbl_802F64E8 -61.0f

int func_800B62FC(void)
{
    struct SomeBigEndingStruct *r3;

    if (lbl_802C6BD8.unk8 == NULL)
        return 0;
    if (gamePauseStatus & 0xA)
        return 1;
    if (lbl_802C6BD8.unk2 != -1)
        lbl_802C6BD8.unk0 = lbl_802C6BD8.unk2;
    lbl_801E2B38[lbl_802C6BD8.unk0]();
    func_800BBBA8();
    func_800BD4B8();
    func_800BB084();
    if (lbl_802C6BD8.unk4 & 2)
        lbl_802C6BD8.unk8->unk0 += lbl_802F64C8;

    r3 = lbl_802C6BD8.unk8;

    r3->unkAA54.unk4.x += r3->unkAA54.unk10.x;
    r3->unkAA54.unk4.y += r3->unkAA54.unk10.y;
    r3->unkAA54.unk4.z += r3->unkAA54.unk10.z;

    r3->unkAA54.unk2C.x += r3->unkAA54.unk38.x;
    r3->unkAA54.unk2C.y += r3->unkAA54.unk38.y;
    r3->unkAA54.unk2C.z += r3->unkAA54.unk38.z;

    return lbl_802C6BD8.unk0 != 32;
}

void func_800B6430(void)
{
    func_800C09B4();
    func_800C00F0();
    func_800BBA54();
    func_800BF2D0();
    func_800BC820();
    OSFreeToHeap(stageHeap, lbl_802C6BD8.unk8);
    lbl_802C6BD8.unk8 = NULL;
    event_finish_all();
    SoundGroupFree();
    if (lbl_802F1B7C == func_800B6430)
        lbl_802F1B7C = NULL;
}

void func_800B64B0(void)
{
    if (lbl_802C6BD8.unk8 == 0)
        return;
    u_call_camera_apply_viewport(modeCtrl.currPlayer);
    u_draw_ball_shadow();
    func_80054FF0();
    func_800BF2D4();
    if (lbl_802C6BD8.unk4 & 1)
    {
        mathutil_mtxA_from_mtxB();
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_culled_sort_translucent(decodedBgGma->modelEntries[4].model);
    }
    func_800BC8B8();
    func_800BBA58();
    if (eventInfo[EVENT_STAGE].state == EV_STATE_RUNNING
     || eventInfo[EVENT_STAGE].state == EV_STATE_SUSPENDED)
        stage_draw();
    if (eventInfo[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
    {
        ord_tbl_set_depth_offset(lbl_802F64CC);
        background_draw();
        ord_tbl_set_depth_offset(lbl_802F64D0);
    }
    if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
        effect_draw();
    draw_monkey();
    ord_tbl_draw_nodes();
    if (lbl_802C6BD8.unk4 & 0x20)
        func_800BFC2C();
}

void func_800B65B4(void)
{
    switch (modeCtrl.difficulty)
    {
    default:
    case DIFFICULTY_BEGINNER: lbl_802C6BD8.unk2 = 1;  break;
    case DIFFICULTY_ADVANCED: lbl_802C6BD8.unk2 = 7;  break;
    case DIFFICULTY_EXPERT:   lbl_802C6BD8.unk2 = 13; break;
    }
}

void func_800B6608(void)
{
    int stageId;
    struct SomeBigEndingStruct_sub *r28;
    Vec sp8;

    lbl_802C6BD8.unk2 = 2;
    modeCtrl.submodeTimer = 420;
    event_finish_all();
    switch (modeCtrl.difficulty)
    {
    case DIFFICULTY_BEGINNER:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    case DIFFICULTY_ADVANCED:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    default:
    case DIFFICULTY_EXPERT:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    }
    load_stage(stageId);
    event_start(EVENT_STAGE);
    event_start(EVENT_STOBJ);
    event_start(EVENT_ITEM);
    event_start(EVENT_OBJ_COLLISION);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    event_start(EVENT_EFFECT);
    event_start(EVENT_REND_EFC);
    event_start(EVENT_BACKGROUND);
    event_start(EVENT_VIBRATION);
    light_init(currStageId);
    rend_efc_mirror_enable();
    func_800BCE54(0, playerCharacterSelection[modeCtrl.currPlayer]);
    lbl_802C6BD8.unk8->unkA804[0].unk70 = 1;
    SOME_CAMERA_MACRO();
    r28 = (void *)&lbl_802C6BD8.unk8->unkAA54;
    memset(r28, 0, sizeof(*r28));
    r28->unk0 = 1;
    r28->unk4.x = lbl_802F64D0;
    r28->unk4.y = lbl_802F64D4;
    r28->unk4.z = lbl_802F64D8;
    r28->unk2 = 1;
    mathutil_mtxA_from_rotate_z(rand() & 0x7FFF);
    mathutil_mtxA_tf_vec_xyz(&sp8, lbl_802F64DC, lbl_802F64D0, lbl_802F64D0);
    r28->unk2C.x = sp8.x;
    r28->unk2C.y = sp8.y + lbl_802F64D4;
    r28->unk2C.z = sp8.z + lbl_802F64E0;
    if (lbl_802C6BD8.unk8->unkA804[0].unk0 != NULL)
        lbl_802C6BD8.unk8->unkA804[0].unk0->unkB4 = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0, 0x1E);
    u_play_music(0x44, 0);
}

s16 lbl_802F18C0[4] = { 0x00EA, 0x0102, 0x0142, 0x0173 };

void func_800B6848(void)
{
    float temp_f2;
    struct SomeBigEndingStruct_sub3 *temp_r4;
    struct SomeBigEndingStruct_sub *temp_r6;

    temp_r6 = &lbl_802C6BD8.unk8->unkAA54;
    temp_r4 = &lbl_802C6BD8.unk8->unkA804[0];
    switch (modeCtrl.submodeTimer)
    {
    case 0x19E:
        func_800BF9CC(modeCtrl.submodeTimer - 0xB4);
        break;
    case 0x134:
        u_play_sound_0(0x230);
        break;
    case 0xF0:
        temp_r4->unk4 = 1;
        temp_r4->unk6 = modeCtrl.submodeTimer - 0x78;
        break;
    case 0xB4:
        temp_f2 = 1.0f / modeCtrl.submodeTimer;
        temp_r6->unk10.x = -temp_r6->unk4.x * temp_f2;
        temp_r6->unk10.y = (20.0f - temp_r6->unk4.y) * temp_f2;
        temp_r6->unk10.z = (-61.0f - temp_r6->unk4.z) * temp_f2;
        break;
    case 0x87:
        SoundReq(lbl_802F18C0[temp_r4->unk0->unk10]);
        break;
    case 0x78:
        func_800BFA2C();
        u_play_sound_0(0x231);
        break;
    case 0x69:
        temp_r6->unk2 = 2;
        temp_r6->unk44 = 0;
        temp_r6->unk46 = 0;
        temp_r6->unk48.x = 0.0f;
        temp_r6->unk48.y = 0.0f;
        temp_r6->unk48.z = 0.0f;
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_802C6BD8.unk2 = 3;
}

void func_800B69F8(void)
{
    struct SomeBigEndingStruct *temp_r30;
    struct SomeBigEndingStruct_sub *temp_r29;
    Vec sp30;
    struct TextBox tbox;
    struct SomeBigEndingStruct_sub3 *temp_r4;
    int i;

    lbl_802C6BD8.unk2 = 4;
    modeCtrl.submodeTimer = 60;
    modeCtrl.unk18 = 0x1E;
    SOME_CAMERA_MACRO();
    temp_r29 = &lbl_802C6BD8.unk8->unkAA54;
    memset(temp_r29, 0, 0x54U);
    temp_r29->unk0 = 1;
    mathutil_mtxA_from_rotate_z((s16) (unpausedFrameCounter << 6));
    mathutil_mtxA_tf_vec_xyz(&sp30, 0.0f, 0.5f, 0.0f);
    temp_r29->unk4.x = sp30.x;
    temp_r29->unk4.y = sp30.y * lbl_802F64F8 + lbl_802F64FC;
    temp_r29->unk4.z = sp30.z + lbl_802F64E8;
    temp_r29->unk2 = 2;
    temp_r29->unk44 = 0;
    temp_r29->unk46 = 0;
    temp_r29->unk48.x = 0.0f;
    temp_r29->unk48.y = 0.0f;
    temp_r29->unk48.z = 0.0f;

    temp_r30 = lbl_802C6BD8.unk8;
    temp_r30->unkAACC = 0;

    func_800C0DC0(&temp_r30->unkAAB0, &temp_r30->unkAAD0);
    if (temp_r30->unkAAB0 > 0)
    {
        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_UP;
        tbox.x = 320;
        tbox.y = 322;
        tbox.numRows = MIN(temp_r30->unkAAB0, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, 1, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, temp_r30->unkAAB4[temp_r30->unkAACC]);
            temp_r30->unkAAB0--;
            temp_r30->unkAACC++;
        }
    }
    temp_r4 = &lbl_802C6BD8.unk8->unkA804[0];
    temp_r4->unkA = 1;
    temp_r4->unk38 = 0;
}

void func_800B6C14(void)
{
    Vec sp30;
    struct TextBox tbox;
    struct SomeBigEndingStruct *temp_r30_2;
    struct SomeBigEndingStruct_sub *temp_r29;
    struct SomeBigEndingStruct_sub3 *temp_r30;
    int i2;

    temp_r29 = &lbl_802C6BD8.unk8->unkAA54;
    temp_r30 = &lbl_802C6BD8.unk8->unkA804[0];
    mathutil_mtxA_from_rotate_z(unpausedFrameCounter << 6);
    mathutil_mtxA_tf_vec_xyz(&sp30, 0.0f, 0.5f, 0.0f);
    temp_r29->unk4.x = sp30.x;
    temp_r29->unk4.y = 20.75f + (0.5f * sp30.y);
    temp_r29->unk4.z = -61.0f + sp30.z;
    temp_r30->unk38--;
    if (temp_r30->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[temp_r30->unk0->unk10][temp_r30->unk3A & 1]);
        temp_r30->unk38 = 240.0f * (0.8f + (0.4f * (rand() / 32767.0f)));
        temp_r30->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & 0x100))
    {
        modeCtrl.unk18 = 0x1E;
        temp_r30_2 = lbl_802C6BD8.unk8;
        if (temp_r30_2->unkAAB0 != 0)
        {
            textbox_set_properties(0, 0x14, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = TEXTBOX_STYLE_CENTER_UP;
            tbox.x = 320;
            tbox.y = 322;
            tbox.numRows = temp_r30_2->unkAAB0;
            tbox.callback = NULL;
            textbox_set_properties(0, 1, &tbox);
            for (i2 = 0; i2 < tbox.numRows; i2++)
            {
                textbox_add_text(0, temp_r30_2->unkAAB4[temp_r30_2->unkAACC]);
                temp_r30_2->unkAAB0--;
                temp_r30_2->unkAACC++;
            }
        }
        else
        {
            lbl_802C6BD8.unk2 = 5;
            textbox_set_properties(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

void func_800B6EA4(void)
{
    struct SomeBigEndingStruct_sub *temp_r29;

    lbl_802C6BD8.unk2 = 6;
    modeCtrl.submodeTimer = 0x168;
    SOME_CAMERA_MACRO();
    temp_r29 = &lbl_802C6BD8.unk8->unkAA54;
    memset(temp_r29, 0, sizeof(*temp_r29));
    temp_r29->unk0 = 1;
    temp_r29->unk4.x = 0.0f;
    temp_r29->unk4.y = 20.0f;
    temp_r29->unk4.z = -62.0f;
    temp_r29->unk2 = 2;
    temp_r29->unk44 = 0;
    temp_r29->unk46 = 0;
    temp_r29->unk48.x = 0.0f;
    temp_r29->unk48.y = 0.0f;
    temp_r29->unk48.z = 0.0f;
    lbl_802C6BD8.unk8->unkA804[0].unkA = 2;
}

s16 lbl_802F18C8[4] = { 0x00B7, 0x0102, 0x0141, 0x018B };

void func_800B6F9C(void)
{
    switch (modeCtrl.submodeTimer)
    {
    case 0xF0:
        lbl_802C6BD8.unk8->unkA804[0].unk4 = 4;
        break;
    case 0x117:
        SoundReq(lbl_802F18C8[playerCharacterSelection[modeCtrl.currPlayer]]);
        break;
    case 0x1E:
        start_screen_fade(1, 0xFFFFFFU, modeCtrl.submodeTimer);
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_802C6BD8.unk2 = 0x1F;
}

void func_800B7078(void)
{
    int stageId;
    float temp_f31;
    struct SomeBigEndingStruct_sub *temp_r31;
    struct SomeBigEndingStruct_sub2 *temp_r3;

    lbl_802C6BD8.unk2 = 8;
    modeCtrl.submodeTimer = 0x186;
    event_finish_all();
    switch (modeCtrl.difficulty)
    {
    case DIFFICULTY_BEGINNER:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    case DIFFICULTY_ADVANCED:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    default:
    case DIFFICULTY_EXPERT:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    }
    load_stage(stageId);
    event_start(EVENT_STAGE);
    event_start(EVENT_STOBJ);
    event_start(EVENT_ITEM);
    event_start(EVENT_OBJ_COLLISION);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    event_start(EVENT_EFFECT);
    event_start(EVENT_REND_EFC);
    event_start(EVENT_BACKGROUND);
    event_start(EVENT_VIBRATION);
    light_init(currStageId);
    rend_efc_mirror_enable();
    func_800BCE54(0, playerCharacterSelection[modeCtrl.currPlayer]);
    lbl_802C6BD8.unk8->unkA804[0].unk4 = 6;
    lbl_802C6BD8.unk8->unkA804[0].unk6 = modeCtrl.submodeTimer - 0xB4;
    SOME_CAMERA_MACRO();
    temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x12C);
    temp_r31 = &lbl_802C6BD8.unk8->unkAA54;
    memset(temp_r31, 0, sizeof(*temp_r31));
    temp_r31->unk0 = 1;
    temp_r31->unk4.x = -1.0f;
    temp_r31->unk4.y = 20.0f;
    temp_r31->unk4.z = -1.0f;
    temp_r31->unk10.y = 10.0f * temp_f31;
    temp_r31->unk2 = 1;
    temp_r31->unk2C.x = 0.0f;
    temp_r31->unk2C.y = 0.0f;
    temp_r31->unk2C.z = 0.0f;
    temp_r3 = lbl_802C6BD8.unk8->unkA804[0].unk0;
    if (temp_r3 != NULL)
        temp_r3->unkB4 = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0U, 0x1E);
    u_play_music(0x44U, 0);
    u_play_sound_0(0x232);
}

s16 lbl_802F18D0[4] = { 0x00EC, 0x0115, 0x016D, 0x0173 };
s16 lbl_802F18D8[4] = { 0x00B5, 0x00FF, 0x0138, 0x0192 };

void func_800B72D4(void)
{
    Vec spB4;
    struct Effect sp8;
    float temp_f30;
    float temp_f31;
    struct SomeBigEndingStruct_sub3 *temp_r30;
    struct SomeBigEndingStruct_sub *temp_r29;

    temp_r29 = &lbl_802C6BD8.unk8->unkAA54;
    temp_r30 = &lbl_802C6BD8.unk8->unkA804[0];
    switch (modeCtrl.submodeTimer)
    {
    case 0x177:
        temp_r29->unk2 = 2;
        temp_r29->unk44 = 0;
        temp_r29->unk46 = 0;
        temp_r29->unk48.x = 0.0f;
        temp_r29->unk48.y = 0.0f;
        temp_r29->unk48.z = 0.0f;
        SoundReq(lbl_802F18D0[playerCharacterSelection[modeCtrl.currPlayer]]);
        break;
    case 0x12C:
        temp_r29->unk0 = 2;
        temp_r29->unk1C = 0;
        temp_r29->unk1E = 0;
        temp_r29->unk20.x = 0.0f;
        temp_r29->unk20.y = 0.0f;
        temp_r29->unk20.z = -2.0f;
        break;
    case 0xB4:
        break;
    case 0x43:
        temp_r30->unk0->unk14 &= 0xFFFFCFFF;
        if (modeCtrl.playerCount > 1)
            temp_r30->unk56 = modeCtrl.currPlayer;
        else
            temp_r30->unk56 = 3;
        func_8004CFF0(0x2E);
        SOME_CAMERA_MACRO();
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x38);
        temp_f30 = 1.25f * (0.9f + (0.2f * (rand() / 32767.0f)));
        temp_r29->unk0 = 1;
        mathutil_mtxA_from_rotate_y(rand() & 0x7FFF);
        mathutil_mtxA_rotate_x(rand() & 0x7FFF);
        mathutil_mtxA_tf_vec_xyz(&temp_r29->unk4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk4.x += temp_r30->unkC.x;
        temp_r29->unk4.y += temp_r30->unkC.y;
        temp_r29->unk4.z += temp_r30->unkC.z;
        mathutil_mtxA_rotate_y((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_rotate_x((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_tf_vec_xyz(&spB4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk10.x = temp_f31 * ((spB4.x + temp_r30->unkC.x) - temp_r29->unk4.x);
        temp_r29->unk10.y = temp_f31 * ((spB4.y + temp_r30->unkC.y) - temp_r29->unk4.y);
        temp_r29->unk10.z = temp_f31 * ((spB4.z + temp_r30->unkC.z) - temp_r29->unk4.z);
        break;
    case 0x3C:
        temp_r30->unk0->unk14 &= 0xFFFFCFFF;
        if (modeCtrl.playerCount > 1)
            temp_r30->unk56 = modeCtrl.currPlayer;
        else
            temp_r30->unk56 = 3;
        func_8004CFF0(0x2E);
        SOME_CAMERA_MACRO();
        temp_f31 = 1.0f / modeCtrl.submodeTimer;
        temp_f30 = 10.0f * (0.9f + (0.2f * (rand() / 32767.0f)));
        temp_r29->unk0 = 1;
        mathutil_mtxA_from_rotate_y(rand() & 0x7FFF);
        mathutil_mtxA_rotate_x(rand() & 0x7FFF);
        mathutil_mtxA_tf_vec_xyz(&temp_r29->unk4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk4.x += temp_r30->unkC.x;
        temp_r29->unk4.y += temp_r30->unkC.y;
        temp_r29->unk4.z += temp_r30->unkC.z;
        mathutil_mtxA_rotate_y((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_rotate_x((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_tf_vec_xyz(&spB4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk10.x = temp_f31 * ((spB4.x + temp_r30->unkC.x) - temp_r29->unk4.x);
        temp_r29->unk10.y = temp_f31 * ((spB4.y + temp_r30->unkC.y) - temp_r29->unk4.y);
        temp_r29->unk10.z = temp_f31 * ((spB4.z + temp_r30->unkC.z) - temp_r29->unk4.z);
        break;
    case 0x69:
    case 0x42:
    case 0x3B:
        temp_r30->unkA = 4;
        break;
    case 0x68:
    case 0x41:
    case 0x3A:
        if (temp_r30->unk0->unk9C != 4U)
            modeCtrl.submodeTimer++;
        else
            temp_r30->unk0->unk14 |= 0x1000;
        break;
    case 0x4B:
    case 0x40:
    case 0x39:
        if (temp_r30->unk0->unk0->unk38 == (temp_r30->unk0->unk0->unk3A >> 1))
            SoundReq(lbl_802F18D8[playerCharacterSelection[modeCtrl.currPlayer]]);
        if (temp_r30->unk0->unk0->unk38 > temp_r30->unk0->unk0->unk3A - 0x10)
        {
            memset(&sp8, 0, sizeof(sp8));
            sp8.type = 0x2E;
            sp8.pos.x = temp_r30->unkC.x;
            sp8.pos.y = 0.1f + temp_r30->unkC.y;
            sp8.pos.z = temp_r30->unkC.z;
            sp8.rotX = temp_r30->unk58;
            sp8.rotY = temp_r30->unk5A;
            sp8.rotZ = temp_r30->unk5C;
            mathutil_mtxA_from_rotate_y(sp8.rotY);
            mathutil_mtxA_rotate_x(sp8.rotX);
            mathutil_mtxA_rotate_z(sp8.rotZ);
            sp8.unkA0 = -1;
            sp8.unk88.x = 0.0f;
            sp8.unk88.y = 0.0f;
            sp8.unk88.z = -1.0f;
            mathutil_mtxA_tf_vec(&sp8.unk88, &sp8.vel);
            temp_f31 = 0.2f * (1.0f + (0.2f * (rand() / 32767.0f)));
            sp8.vel.x *= temp_f31;
            sp8.vel.y *= temp_f31;
            sp8.vel.z *= temp_f31;
            sp8.vel.x += temp_r30->unk18.x;
            sp8.vel.y += temp_r30->unk18.y;
            sp8.vel.z += temp_r30->unk18.z;
            spawn_effect(&sp8);
            sp8.unkA0 = temp_r30->unk56;
            sp8.unk88.x = 0.0f;
            sp8.unk88.y = 0.0f;
            sp8.unk88.z = 1.0f;
            mathutil_mtxA_tf_vec(&sp8.unk88, &sp8.vel);
            sp8.vel.x *= temp_f31;
            sp8.vel.y *= temp_f31;
            sp8.vel.z *= temp_f31;
            sp8.vel.x += temp_r30->unk18.x;
            sp8.vel.y += temp_r30->unk18.y;
            sp8.vel.z += temp_r30->unk18.z;
            spawn_effect(&sp8);
            temp_r30->unk56 = -1;
            u_play_sound_0(0x233);
        }
        else
            modeCtrl.submodeTimer++;
        break;
    case 0x4A:
    case 0x3F:
    case 0x38:
        if (temp_r30->unk0->unk0->unk38 != temp_r30->unk0->unk0->unk3A - 1)
            modeCtrl.submodeTimer++;
        else
        {
            temp_r29->unk0 = 0;
            temp_r29->unk4 = cameraInfo[modeCtrl.currPlayer].eye;
            temp_r29->unk10 = cameraInfo[modeCtrl.currPlayer].eyeVel;
        }
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
    {
        lbl_802C6BD8.unk2 = 9;
        lbl_802C6BD8.unk4 &= 0xFFFFFFF7;
    }
}

void func_800B7B6C(void)
{
    struct TextBox tbox;
    int i;
    u8 unused[4];
    struct SomeBigEndingStruct *temp_r30;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct SomeBigEndingStruct_sub3 *temp_r3_2;

    lbl_802C6BD8.unk2 = 0xA;
    modeCtrl.submodeTimer = 0x3C;
    modeCtrl.unk18 = 0x1E;
    temp_r30 = lbl_802C6BD8.unk8;
    temp_r30->unkAACC = 0;
    func_800C0DC0(&temp_r30->unkAAB0, &temp_r30->unkAAD0);
    if (temp_r30->unkAAB0 > 0)
    {
        memset(&tbox, 0, 0x28U);
        tbox.style = 0xB;
        tbox.x = 0x140;
        tbox.y = 0xA0;
        tbox.numRows = MIN(temp_r30->unkAAB0, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, 1, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, temp_r30->unkAAB4[temp_r30->unkAACC]);
            temp_r30->unkAAB0--;
            temp_r30->unkAACC++;
        }
    }
    temp_r28 = &lbl_802C6BD8.unk8->unkAA54;
    memset(temp_r28, 0, sizeof(*temp_r28));
    temp_r28->unk0 = 2;
    temp_r28->unk1C = 0;
    temp_r28->unk1E = 5;
    temp_r28->unk20.x = 1.5f;
    temp_r28->unk20.y = 0.0f;
    temp_r28->unk20.z = 0.3f;
    temp_r28->unk2 = 2;
    temp_r28->unk44 = 0;
    temp_r28->unk46 = 5;
    temp_r28->unk48.x = 0.0f;
    temp_r28->unk48.y = 0.0f;
    temp_r28->unk48.z = 0.3f;

    temp_r3_2 = &lbl_802C6BD8.unk8->unkA804[0];
    temp_r3_2->unkA = 6;
    temp_r3_2->unk0->unk14 &= 0xFFFFEFFF;
    temp_r3_2->unk38 = 0;
}

void func_800B7D38(void)
{
    struct TextBox tbox;
    int i;
    struct SomeBigEndingStruct *temp_r30_2;
    struct SomeBigEndingStruct_sub3 *temp_r30;

    temp_r30 = &lbl_802C6BD8.unk8->unkA804[0];
    temp_r30->unk38--;
    if (temp_r30->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[temp_r30->unk0->unk10][temp_r30->unk3A & 1]);
        temp_r30->unk38 = 240.0f * (0.8f + (0.4f * (rand() / 32767.0f)));
        temp_r30->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & 0x100))
    {
        modeCtrl.unk18 = 0x1E;
        temp_r30_2 = lbl_802C6BD8.unk8;
        if (temp_r30_2->unkAAB0 != 0)
        {
            textbox_set_properties(0, 0x14, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = 0xB;
            tbox.x = 0x140;
            tbox.y = 0xA0;
            tbox.numRows = temp_r30_2->unkAAB0;
            tbox.callback = NULL;
            textbox_set_properties(0, 1, &tbox);
            for (i = 0; i < tbox.numRows; i++)
            {
                textbox_add_text(0, temp_r30_2->unkAAB4[temp_r30_2->unkAACC]);
                temp_r30_2->unkAAB0--;
                temp_r30_2->unkAACC++;
            }
        }
        else
        {
            lbl_802C6BD8.unk2 = 0xB;
            textbox_set_properties(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

void func_800B7F6C(void)
{
    struct SomeBigEndingStruct_sub2 *temp_r3;

    lbl_802C6BD8.unk2 = 0xC;
    modeCtrl.submodeTimer = 0x21C;
    lbl_802C6BD8.unk8->unkA804[0].unkA = 7;
    temp_r3 = lbl_802C6BD8.unk8->unkA804[0].unk0;
    temp_r3->unk14 &= 0xFFFFBFFF;
}

s16 lbl_802F18E0[4] = { 0x00EA, 0x0102, 0x016D, 0x0173 };

void func_800B7FB4(void)
{
    struct SomeBigEndingStruct_sub *temp_r28;
    struct SomeBigEndingStruct_sub3 *temp_r7;
    void *temp_r4;
    int tm;
    u8 unused[8];

    temp_r28 = &lbl_802C6BD8.unk8->unkAA54;
    temp_r7 = &lbl_802C6BD8.unk8->unkA804[0];
    switch (modeCtrl.submodeTimer)
    {
    case 0x21B:
        if (temp_r7->unk0->unk9C != 7)
            modeCtrl.submodeTimer += 1;
        else
        {
            temp_r7->unk0->unk14 |= 0x1000;
            temp_r28->unk2 = 2;
            temp_r28->unk44 = 0;
            temp_r28->unk46 = 5;
            temp_r28->unk48.x = 0.0f;
            temp_r28->unk48.y = 0.0f;
            temp_r28->unk48.z = 0.0f;
        }
        break;
    case 0x21A:
        if (temp_r7->unk0->unk0->unk38 > temp_r7->unk0->unk0->unk3A - 8)
        {
            temp_r7->unk4 = 8;
            temp_r7->unk6 = modeCtrl.submodeTimer - 0x1A4;
            temp_r28->unk0 = 1;
            temp_r28->unk4 = cameraInfo[modeCtrl.currPlayer].eye;
            temp_r28->unk10 = cameraInfo[modeCtrl.currPlayer].eyeVel;
            u_play_sound_0(0x232);
            SoundReq(lbl_802F18E0[playerCharacterSelection[modeCtrl.currPlayer]]);
        }
        else
            modeCtrl.submodeTimer++;
        break;
    case 0x1A4:
        lbl_802C6BD8.unk4 |= 0x20;
        break;
    case 0xF0:
        lbl_802C6BD8.unk4 &= 0xFFFFFFDF;
        break;
    case 0x10E:
        func_800BF948(1, modeCtrl.submodeTimer - 0x1E);
        break;
    case 0x1E:
        start_screen_fade(1, 0xFFFFFFU, modeCtrl.submodeTimer);
        u_play_sound_0(0x22F);
        break;
    }
    tm = modeCtrl.submodeTimer;
    if (tm < 420 && tm > 360)
        lbl_802C6BD8.unk8->unkAAA8 = 0.5f + 0.5f * (-0.016666668f * (float)(360 - tm));
    else if (tm < 300 && tm > 240)
        lbl_802C6BD8.unk8->unkAAA8 = 0.5f + 0.5f * (1.0f - (-0.016666668f * (float)(240 - tm)));
    if (modeCtrl.submodeTimer < 420 && modeCtrl.submodeTimer > 240)
        lbl_802C6BD8.unk8->unkAAAC = 0.6f + 0.19999999f * (-0.0055555557f * (float)(240 - modeCtrl.submodeTimer));
    if (modeCtrl.submodeTimer < 300 && modeCtrl.submodeTimer > 0)
    {
        float t = -0.0033333334f * -modeCtrl.submodeTimer;
        t = 1.0f - t;
        if ((rand() / 32767.0f) < t)
            func_800BF8C4(1);
        if (unpausedFrameCounter % 90 == 0)
            u_play_sound_0(0x22E);
    }
    temp_r28->unk10.x *= 0.99f;
    temp_r28->unk10.y *= 0.99f;
    temp_r28->unk10.z *= 0.99f;
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_802C6BD8.unk2 = 0x1F;
}

void func_800B83EC(void)
{
    int temp_r0;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct SomeBigEndingStruct_sub2 *temp_r3;
    int stageId;

    lbl_802C6BD8.unk2 = 14;
    modeCtrl.submodeTimer = 0xB4;
    event_finish_all();
    switch (modeCtrl.difficulty)
    {
    case DIFFICULTY_BEGINNER:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    case DIFFICULTY_ADVANCED:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    default:
    case DIFFICULTY_EXPERT:
        stageId = ST_198_ENDING_CUTSCENE_BG;
        break;
    }
    load_stage(stageId);
    event_start(EVENT_STAGE);
    event_start(EVENT_STOBJ);
    event_start(EVENT_ITEM);
    event_start(EVENT_OBJ_COLLISION);
    event_start(EVENT_CAMERA);
    event_start(EVENT_SPRITE);
    event_start(EVENT_SOUND);
    event_start(EVENT_EFFECT);
    event_start(EVENT_REND_EFC);
    event_start(EVENT_BACKGROUND);
    event_start(EVENT_VIBRATION);
    light_init(currStageId);
    rend_efc_mirror_enable();
    func_800BCE54(0, 0);
    func_800BCE54(1, 1);
    func_800BCE54(2, 2);
    func_800BCE54(3, 3);
    lbl_802C6BD8.unk8->unkA804[0].unk4 = 9;
    lbl_802C6BD8.unk8->unkA804[1].unk4 = 9;
    lbl_802C6BD8.unk8->unkA804[2].unk4 = 9;
    SOME_CAMERA_MACRO();
    temp_r28 = &lbl_802C6BD8.unk8->unkAA54;
    memset(temp_r28, 0, sizeof(*temp_r28));
    temp_r28->unk0 = 1;
    mathutil_mtxA_from_rotate_y(modeCtrl.submodeTimer * 4);
    mathutil_mtxA_tf_vec_xyz(&temp_r28->unk4, 0.0f, 20.0f, 10.0f);
    temp_r28->unk2 = 1;
    temp_r28->unk2C.x = 0.0f;
    temp_r28->unk2C.y = 0.0f;
    temp_r28->unk2C.z = 0.0f;
    temp_r0 = playerCharacterSelection[modeCtrl.currPlayer];
    if (temp_r0 == 3)
    {
        struct SomeBigEndingStruct_sub3 *temp_r28_2 = &lbl_802C6BD8.unk8->unkA804[temp_r0];
        temp_r28_2->unk0->unk14 &= 0xFFFFFFDF;
        temp_r28_2->unkA = 0x12;
        temp_r28_2->unkC.x = -20.38f;
        temp_r28_2->unkC.y = -2.34f;
        temp_r28_2->unkC.z = -20.39f;
        if (modeCtrl.playerCount == 1)
            temp_r28_2->unk56 = 3;
        else
            temp_r28_2->unk56 = modeCtrl.currPlayer;
        temp_r28_2->unk58 = (rand() & 0x7FFF);
        temp_r28_2->unk5A = (rand() & 0x7FFF);
        temp_r28_2->unk5C = (rand() & 0x7FFF);
    }
    temp_r3 = lbl_802C6BD8.unk8->unkA804[modeCtrl.currPlayer].unk0;
    if (temp_r3 != NULL)
        temp_r3->unkB4 = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0U, 0x1E);
    u_play_music(0x44U, 0);
}

void func_800B86E0(void)
{
    struct SomeBigEndingStruct_sub *temp_r30 = &lbl_802C6BD8.unk8->unkAA54;

    mathutil_mtxA_from_rotate_y(modeCtrl.submodeTimer * 4);
    mathutil_mtxA_tf_vec_xyz(&temp_r30->unk4, 0.0f, 20.0f, 12.0f);
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        lbl_802C6BD8.unk2 = 0xF;
}

void func_800B8780(void)
{
    struct TextBox tbox;
    float temp_f31;
    struct SomeBigEndingStruct *temp_r28;
    struct SomeBigEndingStruct_sub *temp_r26;
    struct SomeBigEndingStruct_sub3 *temp_r27;
    struct SomeBigEndingStruct_sub3 *temp_r6;
    int i;

    lbl_802C6BD8.unk2 = 0x10;
    modeCtrl.submodeTimer = 0x12C;
    modeCtrl.unk18 = 0x1E;
    SOME_CAMERA_MACRO();
    temp_r26 = &lbl_802C6BD8.unk8->unkAA54;
    temp_r27 = &lbl_802C6BD8.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]];
    temp_f31 = modeCtrl.submodeTimer / 300.0f;
    mathutil_mtxA_from_translate(&temp_r27->unkC);
    mathutil_mtxA_rotate_y((s32) temp_r27->unk30.unk2);
    mathutil_mtxA_rotate_x((s32) temp_r27->unk30.unk0);
    mathutil_mtxA_rotate_z((s32) temp_r27->unk30.unk4);
    temp_r26->unk0 = 1;
    mathutil_mtxA_tf_point_xyz(&temp_r26->unk4, -4.0f * (temp_f31 - 0.5f), -0.2f, -2.0f);
    temp_r26->unk10.x = 0.0f;
    temp_r26->unk10.y = 0.0f;
    temp_r26->unk10.z = 0.0f;
    temp_r26->unk2 = 1;
    mathutil_mtxA_tf_point_xyz(&temp_r26->unk2C, 0.0f, 0.2f, 0.0f);
    temp_r26->unk38.x = 0.0f;
    temp_r26->unk38.y = 0.0f;
    temp_r26->unk38.z = 0.0f;
    lbl_802C6BD8.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]].unk70 = 1;
    temp_r28 = lbl_802C6BD8.unk8;
    temp_r28->unkAACC = 0;
    func_800C0DC0(&temp_r28->unkAAB0, &temp_r28->unkAAD0);
    if (temp_r28->unkAAB0 > 0)
    {
        memset(&tbox, 0, sizeof(tbox));
        tbox.style = 8;
        tbox.x = 0x140;
        tbox.y = 0x142;
        tbox.numRows = MIN(temp_r28->unkAAB0, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, 1, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, temp_r28->unkAAB4[temp_r28->unkAACC]);
            temp_r28->unkAAB0--;
            temp_r28->unkAACC++;
        }
    }
    temp_r6 = &lbl_802C6BD8.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]];
    temp_r6->unk38 = 0;
    if (playerCharacterSelection[modeCtrl.currPlayer] != 3)
        temp_r6->unkA = 0xB;
    else
        temp_r6->unkA = 0xC;
    lbl_802C6BD8.unk8->unkA804[0].unk36 |= 1;
    lbl_802C6BD8.unk8->unkA804[1].unk36 |= 1;
    lbl_802C6BD8.unk8->unkA804[2].unk36 |= 1;
    lbl_802C6BD8.unk8->unkA804[3].unk36 |= 1;
}

void func_800B8AA4(void)
{
    struct TextBox tbox;
    float temp_f31;
    struct SomeBigEndingStruct *temp_r30_2;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct SomeBigEndingStruct_sub3 *temp_r30;
    int i;

    temp_r28 = &lbl_802C6BD8.unk8->unkAA54;
    temp_r30 = &lbl_802C6BD8.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]];
    temp_f31 = modeCtrl.submodeTimer / 300.0f;
    mathutil_mtxA_from_translate(&temp_r30->unkC);
    mathutil_mtxA_rotate_y(temp_r30->unk30.unk2);
    mathutil_mtxA_rotate_x(temp_r30->unk30.unk0);
    mathutil_mtxA_rotate_z(temp_r30->unk30.unk4);
    mathutil_mtxA_tf_point_xyz(&temp_r28->unk4, -4.0f * (temp_f31 - 0.5f), -0.2f, -2.0f);
    temp_r30->unk38--;
    if (temp_r30->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[temp_r30->unk0->unk10][temp_r30->unk3A & 1]);
        temp_r30->unk38 = 240.0f * (0.8f + (0.4f * (rand() / 32767.0f)));
        temp_r30->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & 0x100))
    {
        modeCtrl.unk18 = 0x1E;
        temp_r30_2 = lbl_802C6BD8.unk8;
        if (temp_r30_2->unkAAB0 != 0)
        {
            textbox_set_properties(0, 0x14, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = 8;
            tbox.x = 0x140;
            tbox.y = 0x142;
            tbox.numRows = temp_r30_2->unkAAB0;
            tbox.callback = NULL;
            textbox_set_properties(0, 1, &tbox);
            for (i = 0; i < tbox.numRows; i++)
            {
                textbox_add_text(0, temp_r30_2->unkAAB4[temp_r30_2->unkAACC]);
                temp_r30_2->unkAAB0--;
                temp_r30_2->unkAACC++;
            }
        }
        else
        {
            lbl_802C6BD8.unk2 = 0x11;
            textbox_set_properties(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

void func_800B8D7C(void)
{
    lbl_802C6BD8.unk2 = 0x12;
    modeCtrl.submodeTimer = 0x1E0;
    func_800BBB10(modeCtrl.submodeTimer - 0x1A4);
    lbl_802C6BD8.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]].unk74 = cameraInfo[0].eye;
}

s16 lbl_802F18E8[3] = { 0x00E4, 0x0127, 0x014A };

void func_800B8E1C(void)
{
    f32 temp_f2;
    f32 temp_f3;
    struct SomeBigEndingStruct_sub *temp_r26;
    struct SomeBigEndingStruct_sub3 *temp_r27;
    struct SomeBigEndingStruct_sub2 *temp_r4;
    struct SomeBigEndingStruct *temp;
    Vec sp8;

    switch (modeCtrl.submodeTimer)
    {
    case 0x1DF:
        SOME_CAMERA_MACRO();
        temp_f3 = 1.0f / (modeCtrl.submodeTimer - 0x168);
        temp = lbl_802C6BD8.unk8;
        temp->unkAA54.unk0 = 1;
        temp->unkAA54.unk4.x = 0.0f;
        temp->unkAA54.unk4.y = 0.0f;
        temp->unkAA54.unk4.z = 6.5f;
        temp->unkAA54.unk10.x = -temp->unkAA54.unk4.x * temp_f3;
        temp->unkAA54.unk10.y = (1.5f - temp->unkAA54.unk4.y) * temp_f3;
        temp->unkAA54.unk10.z = (6.0f - temp->unkAA54.unk4.z) * temp_f3;
        temp->unkAA54.unk2 = 1;
        temp->unkAA54.unk2C = lbl_802C6BD8.unk8->unkAA04.unk8;
        temp->unkAA54.unk38.x = 0.0f;
        temp->unkAA54.unk38.y = 0.0f;
        temp->unkAA54.unk38.z = 0.0f;
        break;
    case 0x1A4:
        temp_r4 = lbl_802C6BD8.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]].unk0;
        temp_r4->unk14 &= 0xFFFFFFBF;
        u_play_sound_0(0x234);
        break;
    case 0x168:
        lbl_802C6BD8.unk8->unkA804[0].unkA = 8;
        lbl_802C6BD8.unk8->unkA804[1].unkA = 8;
        lbl_802C6BD8.unk8->unkA804[2].unkA = 8;
        SoundReq(lbl_802F18E8[playerCharacterSelection[modeCtrl.currPlayer] % 3]);
        SOME_CAMERA_MACRO();
        temp = lbl_802C6BD8.unk8;
        temp_r26 = &temp->unkAA54;
        temp_r27 = &temp->unkA804[playerCharacterSelection[modeCtrl.currPlayer] % 3];
        memset(temp_r26, 0, sizeof(*temp_r26));
        mathutil_mtxA_from_translate(&temp_r27->unkC);
        mathutil_mtxA_rotate_y(temp_r27->unk30.unk2);
        mathutil_mtxA_rotate_x(temp_r27->unk30.unk0);
        mathutil_mtxA_rotate_z(temp_r27->unk30.unk4);
        temp_r26->unk0 = 1;
        mathutil_mtxA_tf_point_xyz(&temp_r26->unk4, 1.0f, 3.0f, -4.0f);
        temp_r26->unk10.y = (8.0f - temp_r26->unk4.y) * (1.0f / modeCtrl.submodeTimer);
        mathutil_mtxA_tf_point_xyz(&temp_r26->unk2C, 0.0f, 0.2f, 0.0f);
        break;
    case 0x12C:
        func_800BF948(1, modeCtrl.submodeTimer - 0x1E);
        temp_r26 = &lbl_802C6BD8.unk8->unkAA54;
        temp_f2 = 1.0f / modeCtrl.submodeTimer;
        temp_r26->unk10.x = temp_r26->unk4.x * temp_f2;
        temp_r26->unk10.z = temp_r26->unk4.z * temp_f2;
        temp_r26->unk38.x = -temp_r26->unk2C.x * temp_f2;
        temp_r26->unk38.y = (40.0f - temp_r26->unk2C.y) * temp_f2;
        temp_r26->unk38.z = -temp_r26->unk2C.z * temp_f2;
        break;
    case 0x78:
        lbl_802C6BD8.unk8->unkA804[0].unk4 = 0x10;
        lbl_802C6BD8.unk8->unkA804[1].unk4 = 0x10;
        lbl_802C6BD8.unk8->unkA804[2].unk4 = 0x10;
        lbl_802C6BD8.unk8->unkA804[3].unk56 = -1;
        sp8.x = 0.0f;
        sp8.y = 4.0f;
        sp8.z = 0.0f;
        lbl_802C6BD8.unk8->unkA804[0].unk70 = 2;
        lbl_802C6BD8.unk8->unkA804[1].unk70 = 2;
        lbl_802C6BD8.unk8->unkA804[2].unk70 = 2;
        lbl_802C6BD8.unk8->unkA804[0].unk74 = sp8;
        lbl_802C6BD8.unk8->unkA804[1].unk74 = sp8;
        lbl_802C6BD8.unk8->unkA804[2].unk74 = sp8;
        lbl_802C6BD8.unk8->unkA804[0].unkA = 0xC;
        lbl_802C6BD8.unk8->unkA804[1].unkA = 0xC;
        lbl_802C6BD8.unk8->unkA804[2].unkA = 0xC;
        break;
    case 0x1E:
        start_screen_fade(1, 0xFFFFFFU, modeCtrl.submodeTimer);
        u_play_sound_0(0x22F);
        break;
    }
    if (modeCtrl.submodeTimer < 0x1DF && modeCtrl.submodeTimer > 0x1A4)
        lbl_802C6BD8.unk8->unkAA54.unk2C = lbl_802C6BD8.unk8->unkAA04.unk8;
    if (modeCtrl.submodeTimer < 0x1A4 && modeCtrl.submodeTimer > 0.0f)
    {
        float t = -0.0023809525f * -(float)modeCtrl.submodeTimer;
        t = 1.0f - t;
        if ((rand() / 32767.0f) < t)
            func_800BF8C4(1);
        if (unpausedFrameCounter % 90 == 0)
            u_play_sound_0(0x22E);
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        lbl_802C6BD8.unk2 = 0x13;
}

void func_800B9444(void)
{
    float temp_f4;
    struct SomeBigEndingStruct_sub *temp_r28;
    u8 unused[8];

    lbl_802C6BD8.unk2 = 0x14;
    modeCtrl.submodeTimer = 0x168;
    start_screen_fade(0, 0U, 0x1E);
    func_800BD470();
    func_800BF4F4();
    SOME_CAMERA_MACRO();
    temp_r28 = &lbl_802C6BD8.unk8->unkAA54;
    memset(temp_r28, 0, sizeof(*temp_r28));
    temp_f4 = 1.0f / (modeCtrl.submodeTimer - 0xF0);
    temp_r28->unk0 = 1;
    temp_r28->unk4.x = 0.0f;
    temp_r28->unk4.y = 0.5f;
    temp_r28->unk4.z = 5.0f;
    temp_r28->unk10.x = (-temp_r28->unk4.x * 2.0f) * temp_f4;
    temp_r28->unk10.y = (2.1f - temp_r28->unk4.y) * temp_f4;
    temp_r28->unk10.z = (2.0f - temp_r28->unk4.z) * temp_f4;
    temp_r28->unk2 = 1;
    temp_r28->unk2C = lbl_801E2AD4[playerCharacterSelection[modeCtrl.currPlayer] % 3];
    temp_r28->unk2C.y = 0.0f;
    temp_r28->unk38.x = 2.0f * -temp_r28->unk2C.x * temp_f4;
    temp_r28->unk38.y = (0.4f - temp_r28->unk2C.y) * temp_f4;
    temp_r28->unk38.z = (6.0f - temp_r28->unk2C.z) * temp_f4;
    lbl_802C6BD8.unk8->unkA804[0].unk38 = (s16) (1.0f + (120.0f * ((f32) rand() / 32767.0f)));
    lbl_802C6BD8.unk8->unkA804[1].unk38 = (s16) (1.0f + (120.0f * ((f32) rand() / 32767.0f)));
    lbl_802C6BD8.unk8->unkA804[2].unk38 = (s16) (1.0f + (120.0f * ((f32) rand() / 32767.0f)));
}

s16 lbl_802F18F0[4] = { 0x00E8, 0x0101, 0x016B, 0x0000 };

void func_800B9724(void)
{
    float temp_f3;
    struct SomeBigEndingStruct_sub *temp_r4;
    int i;
    struct SomeBigEndingStruct_sub3 *var_r28;

    switch (modeCtrl.submodeTimer)
    {
    case 0x0:
        break;
    case 0xF0:
        SOME_CAMERA_MACRO();
        temp_r4 = &lbl_802C6BD8.unk8->unkAA54;
        temp_f3 = 1.0f / modeCtrl.submodeTimer;
        temp_r4->unk0 = 1;
        temp_r4->unk4.x = -0.4f;
        temp_r4->unk4.y = -0.35f;
        temp_r4->unk4.z = 4.0f;
        temp_r4->unk10.x = (-0.6f - temp_r4->unk4.x) * temp_f3;
        temp_r4->unk10.y = (-0.3f - temp_r4->unk4.y) * temp_f3;
        temp_r4->unk10.z = (14.8f - temp_r4->unk4.z) * temp_f3;
        temp_r4->unk2 = 1;
        temp_r4->unk2C.x = 0.0f;
        temp_r4->unk2C.y = 2.0f;
        temp_r4->unk2C.z = 2.0f;
        temp_r4->unk38.x = -temp_r4->unk2C.x * temp_f3;
        temp_r4->unk38.y = (2.5f - temp_r4->unk2C.y) * temp_f3;
        temp_r4->unk38.z = (2.0f - temp_r4->unk2C.z) * temp_f3;
        break;
    }
    var_r28 = lbl_802C6BD8.unk8->unkA804;
    for (i = 3; i > 0; i--, var_r28++)  // BUG: maybe should be >= 0 instead?
    {
        if (var_r28->unk38 != 0)
        {
            var_r28->unk38--;
            if (var_r28->unk38 == 0)
                SoundReq(lbl_802F18F0[var_r28->unk0->unk10]);
        }
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        lbl_802C6BD8.unk2 = 0x15;
}

void func_800B9920(void)
{
    struct SomeBigEndingStruct *temp_r3;
    float temp_f3;
    struct SomeBigEndingStruct_sub *temp_r4;

    lbl_802C6BD8.unk2 = 0x16;
    modeCtrl.submodeTimer = 0x2D0;
    lbl_802C6BD8.unk8->unkA804[0].unk0->unk14 |= 0x20;
    lbl_802C6BD8.unk8->unkA804[1].unk0->unk14 |= 0x20;
    lbl_802C6BD8.unk8->unkA804[2].unk0->unk14 |= 0x20;
    lbl_802C6BD8.unk8->unkA804[3].unk0->unk14 |= 0x20;
    SOME_CAMERA_MACRO();
    temp_r4 = &lbl_802C6BD8.unk8->unkAA54;
    temp_f3 = 0.008333334f;
    temp_r4->unk0 = 1;
    temp_r4->unk4.x = 0.0f;
    temp_r4->unk4.y = 0.2f;
    temp_r4->unk4.z = 5.0f;
    temp_r4->unk10.x = -temp_r4->unk4.x * temp_f3;
    temp_r4->unk10.y = (0.2f - temp_r4->unk4.y) * temp_f3;
    temp_r4->unk10.z = (4.5f - temp_r4->unk4.z) * temp_f3;
    temp_r4->unk2 = 1;
    temp_r4->unk2C.x = 0.0f;
    temp_r4->unk2C.y = 0.2f;
    temp_r4->unk2C.z = 3.5f;
    temp_r4->unk38.x = -temp_r4->unk2C.x * temp_f3;
    temp_r4->unk38.y = (0.25f - temp_r4->unk2C.y) * temp_f3;
    temp_r4->unk38.z = (3.5f - temp_r4->unk2C.z) * temp_f3;
    !temp_r4;  // needed to match
}

void func_800B9A8C(void)
{
    float temp_f2;
    struct SomeBigEndingStruct_sub *temp;

    switch (modeCtrl.submodeTimer)
    {
    case 0x2B2:
        lbl_802C6BD8.unk4 |= 4;
        u_play_sound_0(0x235);
        break;
    case 0x258:
        lbl_802C6BD8.unk8->unkA804[0].unk4 = 0x11;
        lbl_802C6BD8.unk8->unkA804[1].unk4 = 0x11;
        lbl_802C6BD8.unk8->unkA804[2].unk4 = 0x11;
        break;
    case 0x257:
        lbl_802C6BD8.unk8->unkA804[0].unk0->unk14 &= ~0x20;
        lbl_802C6BD8.unk8->unkA804[1].unk0->unk14 &= ~0x20;
        lbl_802C6BD8.unk8->unkA804[2].unk0->unk14 &= ~0x20;
        SOME_CAMERA_MACRO();
        temp = &lbl_802C6BD8.unk8->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0xD8);
        temp->unk0 = 1;
        temp->unk4.x = -2.0f;
        temp->unk4.y = 0.3f;
        temp->unk4.z = 5.0f;
        temp->unk10.x = (-2.5f - temp->unk4.x) * temp_f2;
        temp->unk10.y = (0.4f - temp->unk4.y) * temp_f2;
        temp->unk10.z = (4.5f - temp->unk4.z) * temp_f2;
        temp->unk2 = 2;
        temp->unk44 = playerCharacterSelection[modeCtrl.currPlayer] % 3;
        temp->unk46 = 0;
        temp->unk48.x = 0.0f;
        temp->unk48.y = 0.0f;
        temp->unk48.z = 0.0f;
        break;
    case 0x1E0:
        lbl_802C6BD8.unk8->unkA804[3].unk0->unk14 &= ~0x20;
        lbl_802C6BD8.unk8->unkA804[3].unk4 = 0x13;
        break;
    case 0xD8:
        temp = &lbl_802C6BD8.unk8->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0x3C);
        temp->unk0 = 1;
        temp->unk10.x = (2.0f - temp->unk4.x) * temp_f2;
        temp->unk10.y = (0.3f - temp->unk4.y) * temp_f2;
        temp->unk10.z = (3.5f - temp->unk4.z) * temp_f2;
        temp->unk2 = 2;
        temp->unk44 = 3;
        temp->unk46 = 0;
        temp->unk48.x = 0.0f;
        temp->unk48.y = 0.0f;
        temp->unk48.z = 0.0f;
        SoundReq(0x199U);
        break;
    case 0x8A:
        temp = &lbl_802C6BD8.unk8->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0x3C);
        temp->unk0 = 1;
        temp->unk10.x = (2.0f - temp->unk4.x) * temp_f2;
        temp->unk10.y = (0.3f - temp->unk4.y) * temp_f2;
        temp->unk10.z = (4.5f - temp->unk4.z) * temp_f2;
        temp->unk2 = 2;
        temp->unk44 = 3;
        temp->unk46 = 0;
        temp->unk48.x = 0.0f;
        temp->unk48.y = 0.0f;
        temp->unk48.z = 0.0f;
        break;
    case 0x3C:
        lbl_802C6BD8.unk4 &= 0xFFFFFFFB;
        u_play_sound_0(0x236);
        temp = &lbl_802C6BD8.unk8->unkAA54;
        temp->unk0 = 1;
        temp->unk10.x = 0.0f;
        temp->unk10.y = 0.0f;
        temp->unk10.z = 0.0f;
        break;
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        lbl_802C6BD8.unk2 = 0x17;
}

void func_800B9EA4(void)
{
    struct SomeBigEndingStruct_sub *temp;

    lbl_802C6BD8.unk2 = 0x18;
    modeCtrl.submodeTimer = 0x12C;
    lbl_802C6BD8.unk8->unkA804[0].unk4 = 0x15;
    lbl_802C6BD8.unk8->unkA804[1].unk4 = 0x15;
    lbl_802C6BD8.unk8->unkA804[2].unk4 = 0x15;
    lbl_802C6BD8.unk8->unkA804[0].unk70 = 0;
    lbl_802C6BD8.unk8->unkA804[1].unk70 = 0;
    lbl_802C6BD8.unk8->unkA804[2].unk70 = 0;
    temp = &lbl_802C6BD8.unk8->unkAA54;
    temp->unk0 = 1;
    temp->unk4.x = 2.0f;
    temp->unk4.y = 0.3f;
    temp->unk4.z = 4.5f;
    temp->unk10.x = 0.0f;
    temp->unk10.y = 0.0f;
    temp->unk10.z = 0.0f;
    temp->unk2 = 2;
    temp->unk44 = 3;
    temp->unk46 = 0;
    temp->unk48.x = 0.0f;
    temp->unk48.y = 0.0f;
    temp->unk48.z = 0.0f;
    u_play_sound_0(0x238);
    SoundReq(0x17BU);
    lbl_802C6BD8.unk8->unkA804[0].unk36 &= ~0x1;
    lbl_802C6BD8.unk8->unkA804[1].unk36 &= ~0x1;
    lbl_802C6BD8.unk8->unkA804[2].unk36 &= ~0x1;
}

void func_800B9FDC(void)
{
    struct SomeBigEndingStruct_sub3 *temp_r29 = &lbl_802C6BD8.unk8->unkA804[3];
    int temp_r28 = temp_r29->unk0->unk0->unk38;

    if (temp_r29->unk0->unk9C == 8 && temp_r28 == 0x2A)
        SoundReq(0x181U);
    if (temp_r29->unk0->unk9C == 0xA)
    {
        if (temp_r28 == 1)
            SoundReq(0x185U);
        if (temp_r28 == 0x14 || temp_r28 == 0x24 || temp_r28 == 0x34 || temp_r28 == 0x44)
            u_play_sound_0(0x238);
    }
    if (temp_r29->unk0->unk9C == 0xBU && temp_r28 == 0xB2)
        SoundReq(0x199U);
    if (temp_r29->unk0->unk9C == 0xEU && (temp_r28 == 0x10 || temp_r28 == 0x2E))
        SoundReq(0x186U);
    if (temp_r29->unk8 == 0xB && temp_r28 > 0xC3)
    {
        func_800BF658();
        lbl_802C6BD8.unk2 = 0x19;
        lbl_802C6BD8.unk4 |= 2;
        lbl_802C6BD8.unk8->unk0 = 0.0f;
        u_play_sound_0(0x238);
    }
}

void func_800BA124(void)
{
    lbl_802C6BD8.unk2 = 0x1A;
    modeCtrl.submodeTimer = 0x1E0;
    modeCtrl.unk18 = 0;
    lbl_802C6BD8.unk8->unkAA04.unk2 = 4;
}

void func_800BA160(void)
{
    struct SomeBigEndingStruct_sub *temp_r4 = &lbl_802C6BD8.unk8->unkAA54;
    float temp_f2;

    switch (modeCtrl.submodeTimer)
    {
    case 0x1D1:
        SOME_CAMERA_MACRO();
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0x12C);
        temp_r4->unk0 = 0;
        temp_r4->unk4.x = 0.0f;
        temp_r4->unk4.y = -0.34f;
        temp_r4->unk4.z = 5.0f;
        temp_r4->unk10.x = -temp_r4->unk4.x * temp_f2;
        temp_r4->unk10.y = (-0.25f - temp_r4->unk4.y) * temp_f2;
        temp_r4->unk10.z = (7.8f - temp_r4->unk4.z) * temp_f2;
        temp_r4->unk2 = 2;
        temp_r4->unk44 = 3;
        temp_r4->unk46 = 5;
        temp_r4->unk48.x = 0.0f;
        temp_r4->unk48.y = 0.0f;
        temp_r4->unk48.z = 0.6f;
        break;
    case 0x195:
        temp_r4->unk0 = 1;
        break;
    case 0x186:
        func_800BF774();
        break;
    case 0x12C:
        temp_r4->unk10.x = 0.0f;
        temp_r4->unk10.y = 0.0f;
        temp_r4->unk10.z = 0.0f;
        break;
    case 0xD2:
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0x96);
        temp_r4->unk0 = 1;
        temp_r4->unk10.x = -temp_r4->unk4.x * temp_f2;
        temp_r4->unk10.y = (0.7f - temp_r4->unk4.y) * temp_f2;
        temp_r4->unk10.z = (4.2f - temp_r4->unk4.z) * temp_f2;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0x5A);
        temp_r4->unk2 = 1;
        temp_r4->unk2C = cameraInfo[modeCtrl.currPlayer].lookAt;
        temp_r4->unk38.x = -temp_r4->unk2C.x * temp_f2;
        temp_r4->unk38.y = (0.1f - temp_r4->unk2C.y) * temp_f2;
        temp_r4->unk38.z = -temp_r4->unk2C.z * temp_f2;
        break;
    case 0x96:
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0x5A);
        temp_r4->unk10.x = -temp_r4->unk4.x * temp_f2;
        temp_r4->unk10.y = (-0.4f - temp_r4->unk4.y) * temp_f2;
        temp_r4->unk10.z = (2.6f - temp_r4->unk4.z) * temp_f2;
        SoundReq(0xEEU);
        SoundReq(0x102U);
        SoundReq(0x16DU);
        break;
    case 0x5A:
        temp_r4->unk10.x = 0.0f;
        temp_r4->unk10.y = 0.0f;
        temp_r4->unk10.z = 0.0f;
        temp_r4->unk38.x = 0.0f;
        temp_r4->unk38.y = 0.0f;
        temp_r4->unk38.z = 0.0f;
        break;
    }
    if ( modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
    {
        if (playerCharacterSelection[modeCtrl.currPlayer] != 3)
            lbl_802C6BD8.unk2 = 0x1B;
        else
            lbl_802C6BD8.unk2 = 0x1D;
    }
}

void func_800BA4F4(void)
{
    lbl_802C6BD8.unk2 = 0x1E;
    modeCtrl.submodeTimer = 0x14A;
    modeCtrl.unk18 = 0;
}

void func_800BA51C(void)
{
    float temp_f2;
    int charaId;
    struct SomeBigEndingStruct_sub *var_r31 = &lbl_802C6BD8.unk8->unkAA54;
    struct SomeBigEndingStruct_sub3 *temp_r6;
    Vec sp8;

    switch (modeCtrl.submodeTimer)
    {
    case 0x14A:
        SOME_CAMERA_MACRO();
        var_r31 = &lbl_802C6BD8.unk8->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0xD2);
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        temp_r6 = &lbl_802C6BD8.unk8->unkA804[charaId];
        var_r31->unk0 = 1;
        var_r31->unk4.x = temp_r6->unkC.x;
        var_r31->unk4.y = temp_r6->unkC.y - 0.5f;
        var_r31->unk4.z = temp_r6->unkC.z - 2.0f;
        var_r31->unk10.x = 0.0f;
        var_r31->unk10.y = 0.15f * temp_f2;
        var_r31->unk10.z = temp_f2;
        var_r31->unk2 = 2;
        var_r31->unk44 = charaId;
        var_r31->unk46 = 5;
        var_r31->unk48.x = 0.0f;
        var_r31->unk48.y = 0.0f;
        var_r31->unk48.z = 0.6f;
        SoundReq(0x199U);
        break;
    case 0xD2:
        var_r31->unk10.x = 0.0f;
        var_r31->unk10.y = 0.0f;
        var_r31->unk10.z = 0.0f;
        break;
    case 0xB4:
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        lbl_802C6BD8.unk8->unkA804[3].unkA = 0x11;
        temp_r6 = &lbl_802C6BD8.unk8->unkA804[charaId];
        SOME_CAMERA_MACRO();
        var_r31->unk0 = 1;
        var_r31->unk4.x = temp_r6->unkC.x;
        var_r31->unk4.y = 0.5f + temp_r6->unkC.y;
        var_r31->unk4.z = 2.5f + temp_r6->unkC.z;
        var_r31->unk10.x = 0.0f;
        var_r31->unk10.y = 0.0f;
        var_r31->unk10.z = 0.0f;
        var_r31->unk2 = 1;
        var_r31->unk2C.x = temp_r6->unkC.x;
        var_r31->unk2C.y = 0.25f + temp_r6->unkC.y;
        var_r31->unk2C.z = temp_r6->unkC.z;
        var_r31->unk38.x = 0.0f;
        var_r31->unk38.y = 0.0f;
        var_r31->unk38.z = 0.0f;
        break;
    case 0x42:
        SoundReq(0x188U);
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0 && modeCtrl.unk18 == 0)
    {
        modeCtrl.unk18 = 0x78;
        start_screen_fade(1, 0xFFFFFFU, modeCtrl.unk18);
        temp_f2 = 1.0f / modeCtrl.unk18;
        var_r31->unk0 = 1;
        var_r31->unk4 = cameraInfo[modeCtrl.currPlayer].eye;
        sp8.x = var_r31->unk4.x;
        sp8.y = 0.0f;
        sp8.z = var_r31->unk4.z;
        mathutil_vec_set_len(&sp8, &sp8, 8.0f);
        var_r31->unk10.x = temp_f2 * (sp8.x - var_r31->unk4.y);
        var_r31->unk10.y = (4.5f - var_r31->unk4.y) * temp_f2;
        var_r31->unk10.z = temp_f2 * (sp8.z - var_r31->unk4.y);
    }
    if (modeCtrl.unk18 != 0)
    {
        modeCtrl.unk18--;
        if (modeCtrl.unk18 == 0)
            lbl_802C6BD8.unk2 = 0x1F;
    }
}

void func_800BA928(void)
{
    lbl_802C6BD8.unk2 = 0x1C;
    modeCtrl.submodeTimer = 0x276;
    modeCtrl.unk18 = 0;
}

s16 lbl_802F18F8[4] = { 0x00E8, 0x0102, 0x016D, 0x0000 };

void func_800BA950(void)
{
    struct SomeBigEndingStruct_sub2 *temp_r27_4;
    struct SomeBigEndingStruct_sub3 *temp_r27;
    int charaId;
    struct SomeBigEndingStruct_sub *temp_r30;
    Vec sp38;
    Mtx sp8;
    float temp_f31;

    temp_r30 = &lbl_802C6BD8.unk8->unkAA54;
    switch (modeCtrl.submodeTimer)
    {
    case 0x276:
        lbl_802C6BD8.unk4 |= 0x10;
        SOME_CAMERA_MACRO();
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x1FE);
        charaId = (playerCharacterSelection[modeCtrl.currPlayer] + 2) % 3;
        temp_r30 = &lbl_802C6BD8.unk8->unkAA54;
        temp_r27 = &lbl_802C6BD8.unk8->unkA804[charaId];
        lbl_802C6BD8.unk8->unkAA54.unk0 = 1;
        sp38.x = temp_r27->unkC.x;
        sp38.y = 0.0f;
        sp38.z = temp_r27->unkC.z;
        mathutil_vec_set_len(&sp38, &sp38, 1.25f);
        temp_r30->unk4.x = temp_r27->unkC.x + sp38.x;
        temp_r30->unk4.y = temp_r27->unkC.y;
        temp_r30->unk4.z = temp_r27->unkC.z + sp38.z;
        temp_r30->unk10.x = -0.5f * sp38.x * temp_f31;
        temp_r30->unk10.y = 0.5f * temp_f31;
        temp_r30->unk10.z = temp_f31 * (1.0f + temp_r27->unkC.z - temp_r30->unk4.z);
        temp_r30->unk2 = 2;
        temp_r30->unk44 = charaId;
        temp_r30->unk46 = 0;
        temp_r30->unk48.x = 0.0f;
        temp_r30->unk48.y = 0.0f;
        temp_r30->unk48.z = 0.0f;
        break;
    case 0x1E0:
        SOME_CAMERA_MACRO();
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x168);
        charaId = (playerCharacterSelection[modeCtrl.currPlayer] + 1) % 3;
        temp_r30 = &lbl_802C6BD8.unk8->unkAA54;
        temp_r27 = &lbl_802C6BD8.unk8->unkA804[charaId];
        lbl_802C6BD8.unk8->unkAA54.unk0 = 1;
        sp38.x = temp_r27->unkC.x;
        sp38.y = 0.0f;
        sp38.z = temp_r27->unkC.z;
        mathutil_vec_set_len(&sp38, &sp38, 1.25f);
        temp_r30->unk4.x = temp_r27->unkC.x + sp38.x;
        temp_r30->unk4.y = temp_r27->unkC.y;
        temp_r30->unk4.z = temp_r27->unkC.z + sp38.z;
        temp_r30->unk10.x = -0.5f * sp38.x * temp_f31;
        temp_r30->unk10.y = 0.5f * temp_f31;
        temp_r30->unk10.z = temp_f31 * (1.0f + temp_r27->unkC.z - temp_r30->unk4.z);
        temp_r30->unk2 = 2;
        temp_r30->unk44 = charaId;
        temp_r30->unk46 = 0;
        temp_r30->unk48.x = 0.0f;
        temp_r30->unk48.y = 0.0f;
        temp_r30->unk48.z = 0.0f;
        break;
    case 0x14A:
        SOME_CAMERA_MACRO();
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0xD2);
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        temp_r30 = &lbl_802C6BD8.unk8->unkAA54;
        temp_r27 = &lbl_802C6BD8.unk8->unkA804[charaId];
        lbl_802C6BD8.unk8->unkAA54.unk0 = 1;
        sp38.x = temp_r27->unkC.x;
        sp38.y = 0.0f;
        sp38.z = temp_r27->unkC.z;
        mathutil_vec_set_len(&sp38, &sp38, 1.25f);
        temp_r30->unk4.x = temp_r27->unkC.x + sp38.x;
        temp_r30->unk4.y = temp_r27->unkC.y;
        temp_r30->unk4.z = temp_r27->unkC.z + sp38.z;
        temp_r30->unk10.x = -0.5f * sp38.x * temp_f31;
        temp_r30->unk10.y = 0.5f * temp_f31;
        temp_r30->unk10.z = temp_f31 * (1.0f + temp_r27->unkC.z - temp_r30->unk4.z);
        temp_r30->unk2 = 2;
        temp_r30->unk44 = charaId;
        temp_r30->unk46 = 0;
        temp_r30->unk48.x = 0.0f;
        temp_r30->unk48.y = 0.0f;
        temp_r30->unk48.z = 0.0f;
        break;
    case 0x168:
    case 0x1FE:
    case 0xD2:
        temp_r30->unk10.x = 0.0f;
        temp_r30->unk10.y = 0.0f;
        temp_r30->unk10.z = 0.0f;
        break;
    case 0xB4:
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        lbl_802C6BD8.unk8->unkA804[charaId].unkA = 0x10;
        temp_r27 = &lbl_802C6BD8.unk8->unkA804[charaId];
        temp_r30->unk0 = 1;
        temp_r27_4 = temp_r27->unk0;
        mathutil_mtxA_from_quat(&temp_r27_4->unk60);
        mathutil_mtxA_to_mtx(sp8);
        mathutil_mtxA_from_translate(&temp_r27_4->unk30);
        mathutil_mtxA_scale_s(temp_r27_4->unk58);
        mathutil_mtxA_translate(&temp_r27_4->unk3C);
        mathutil_mtxA_mult_right(sp8);
        mathutil_mtxA_mult_right(temp_r27_4->unk0->unk8EC8);
        mathutil_mtxA_tf_point_xyz(&sp38, 0.0f, 0.0f, 0.3f);
        sp38.y += -0.3f;
        sp38.z += 1.0f;
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x42);
        temp_r30->unk10.x = temp_f31 * (sp38.x - temp_r30->unk4.x);
        temp_r30->unk10.y = temp_f31 * (sp38.y - temp_r30->unk4.y);
        temp_r30->unk10.z = temp_f31 * (sp38.z - temp_r30->unk4.z);
        break;
    case 0x42:
        SoundReq(lbl_802F18F8[playerCharacterSelection[modeCtrl.currPlayer]]);
        temp_r30->unk10.x = 0.0f;
        temp_r30->unk10.y = 0.0f;
        temp_r30->unk10.z = 0.0f;
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0 && modeCtrl.unk18 == 0)
    {
        modeCtrl.unk18 = 0x78;
        start_screen_fade(1, 0xFFFFFFU, modeCtrl.unk18);
    }
    if (modeCtrl.unk18 != 0)
    {
        modeCtrl.unk18--;
        if (modeCtrl.unk18 == 0)
            lbl_802C6BD8.unk2 = 0x1F;
    }
}

void func_800BB050(void)
{
    lbl_802C6BD8.unk2 = 0x20;
}

void func_800BB064(void) {}

void func_800BB068(void)
{
    lbl_802C6BD8.unk8->unkAA04.unk0 = 0;
}

void func_800BB084(void)
{
    struct SomeStruct *temp_r30 = &lbl_802C6BD8.unk8->unkAA04;
    float temp_f5;

    if (temp_r30->unk0 != 0)
    {
        switch (temp_r30->unk2)
        {
        case 1:
            temp_r30->unk2 = 2;
            temp_r30->unk14.x = 0.0f;
            temp_r30->unk14.y = -0.1f;
            temp_r30->unk14.z = 0.0f;
            temp_f5 = temp_r30->unk4;
            temp_r30->unk8.x = 0.0f;
            temp_r30->unk8.y = -0.5f * (temp_f5 * ((2.0f * temp_r30->unk14.y) + (-0.002f * temp_f5)));
            temp_r30->unk8.z = 0.0f;
            /* fallthrough */
        case 2:
            temp_r30->unk14.y += -0.002f;
            temp_r30->unk8.y += temp_r30->unk14.y;
            if (temp_r30->unk8.y <= 0.0f)
            {
                temp_r30->unk8.y = 0.0f;
                temp_r30->unk14.y = 0.0f;
                temp_r30->unk2 = 3;
                temp_r30->unk44.x += 0.1f * ((rand() / 32767.0f) - 0.5f);
                temp_r30->unk44.y += 0.1f * ((rand() / 32767.0f) - 0.5f);
                temp_r30->unk44.z += 0.1f * ((rand() / 32767.0f) - 0.5f);
            }
            temp_r30->unk20.x = 0.4f + temp_r30->unk8.x;
            temp_r30->unk20.y = temp_r30->unk8.y;
            temp_r30->unk20.z = 2.95f + temp_r30->unk8.z;
            break;
        case 3:
            if (lbl_802C6BD8.unk4 & 4)
            {
                temp_r30->unk40 = (-0x4000 - temp_r30->unk3A) >> 7;
                temp_r30->unk3A += temp_r30->unk40;
            }
            else
            {
                temp_r30->unk40 += (int)(-0.05f * temp_r30->unk3A);
                temp_r30->unk3A += temp_r30->unk40;
                if (temp_r30->unk3A > 0)
                {
                    temp_r30->unk3A = 0;
                    if (temp_r30->unk40 > 0)
                    {
                        if (temp_r30->unk40 > 0x800)
                        {
                            temp_r30->unk44.x += 0.1f * ((rand() / 32767.0f) - 0.5f);
                            temp_r30->unk44.y += 0.1f * ((rand() / 32767.0f) - 0.5f);
                            temp_r30->unk44.z += 0.1f * ((rand() / 32767.0f) - 0.5f);
                        }
                        temp_r30->unk40 = temp_r30->unk40 * -0.4f;
                    }
                }
            }
            break;
        case 4:
            temp_r30->unk2 = 5;
            temp_r30->unk4 = 0x3C;
            temp_r30->unk3E += 768.0f * ((rand() / 32767.0f) - 0.5f);
            temp_r30->unk40 += 768.0f * ((rand() / 32767.0f) - 0.5f);
            temp_r30->unk42 += 2304.0f * ((rand() / 32767.0f) - 0.5f);
            temp_r30->unk44.x += 0.1f * ((rand() / 32767.0f) - 0.5f);
            temp_r30->unk44.y += 0.1f * ((rand() / 32767.0f) - 0.5f);
            temp_r30->unk44.z += 0.1f * ((rand() / 32767.0f) - 0.5f);
            /* fallthrough */
        case 5:
            temp_r30->unk3E = 0.8f * temp_r30->unk3E;
            temp_r30->unk40 = 0.8f * temp_r30->unk3E;  // BUG: should be unk40?
            temp_r30->unk42 = 0.9f * temp_r30->unk3E;  // BUG: should be unk42?
            temp_r30->unk38 += temp_r30->unk3E;
            temp_r30->unk3A += temp_r30->unk40;
            temp_r30->unk3C += temp_r30->unk42;
            temp_r30->unk4--;
            if (temp_r30->unk4 < 0)
            {
                temp_r30->unk2 = 6;
                temp_r30->unk3E += 2304.0f * ((rand() / 32767.0f) - 0.5f);
                temp_r30->unk40 += -2304.0f * -(rand() / 32767.0f);
                temp_r30->unk42 += 2304.0f * ((rand() / 32767.0f) - 0.5f);
                temp_r30->unk44.x += 0.1f * ((rand() / 32767.0f) - 0.5f);
                temp_r30->unk44.y += 0.1f * ((rand() / 32767.0f) - 0.5f);
                temp_r30->unk44.z += 0.1f * ((rand() / 32767.0f) - 0.5f);
            }
            break;
        case 6:
            temp_r30->unk2C.x *= 0.98f;
            temp_r30->unk2C.y *= 0.98f;
            temp_r30->unk2C.z *= 0.98f;
            temp_r30->unk2C.y += -0.001f;
            temp_r30->unk20.x += temp_r30->unk2C.x;
            temp_r30->unk20.y += temp_r30->unk2C.y;
            temp_r30->unk20.z += temp_r30->unk2C.z;
            temp_r30->unk3E = 0.95f * temp_r30->unk3E;
            temp_r30->unk40 = 0.95f * temp_r30->unk3E;  // BUG: should be unk40?
            temp_r30->unk42 = 0.95f * temp_r30->unk3E;  // BUG: should be unk42?
            temp_r30->unk38 += temp_r30->unk3E;
            temp_r30->unk3A += temp_r30->unk40;
            temp_r30->unk3C += temp_r30->unk42;
            break;
        }
        temp_r30->unk44.x *= -0.85f;
        temp_r30->unk44.y *= -0.85f;
        temp_r30->unk44.z *= -0.85f;
    }
}

void func_800BBA54(void) {}

void func_800BBA58(void)
{
    struct GMAModel *model;
    struct SomeStruct *temp_r31 = &lbl_802C6BD8.unk8->unkAA04;

    if (temp_r31->unk0 != 0)
    {
        model = decodedBgGma->modelEntries[6].model;
        mathutil_mtxA_from_mtxB_translate(&temp_r31->unk8);
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_unculled_sort_translucent(model);
        model = decodedBgGma->modelEntries[30].model;
        mathutil_mtxA_from_mtxB_translate(&temp_r31->unk20);
        mathutil_mtxA_translate(&temp_r31->unk44);
        mathutil_mtxA_rotate_y(temp_r31->unk3A);
        mathutil_mtxA_rotate_x(temp_r31->unk38);
        mathutil_mtxA_rotate_z(temp_r31->unk3C);
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_unculled_sort_translucent(model);
    }
}

void func_800BBB10(int arg0)
{
    struct SomeStruct *temp_r31 = &lbl_802C6BD8.unk8->unkAA04;

    memset(temp_r31, 0, sizeof(*temp_r31));
    temp_r31->unk0 = 1;
    temp_r31->unk2 = 1;
    temp_r31->unk4 = arg0;
}

void func_800BBB78(void)
{
    struct SomeBigEndingStruct *temp = lbl_802C6BD8.unk8;

    if (temp != NULL)
    {
        temp->unkA804[0].unk56 = -1;
        temp->unkA804[1].unk56 = -1;
        temp->unkA804[2].unk56 = -1;
        temp->unkA804[3].unk56 = -1;
    }
}

#define lbl_802F6644 -4.5f
#define lbl_802F664C 0.12999999523162842f
#define lbl_802F6650 0.12000000476837158f
#define lbl_802F6654 -0.094000004231929779f
#define lbl_802F65E4 -0.10000000149011612f
#define lbl_802F65A0 -0.60000002384185791f
#define lbl_802F6528 0.20000000298023224f
#define lbl_802F6634 30.0f
#define lbl_802F6568 -0.20000000298023224f
#define lbl_802F6638 -15.0f
#define lbl_802F663C 0.012000000104308128f
#define lbl_802F6640 -0.0080000003799796104f

void func_800BBBA8(void)
{
    int i;
    struct Struct801E2A98 spD0;
    struct Effect effect;
    struct RaycastHit sp8;
    int playerId = modeCtrl.currPlayer;
    struct SomeBigEndingStruct_sub3 *var_r16 = lbl_802C6BD8.unk8->unkA804;
    float temp_f0;

    for (i = 0; i < 4; i++, var_r16++)
    {
        if (var_r16->unk0 != NULL)
        {
            switch (var_r16->unk4)
            {
            case 1:
                var_r16->unk4 = 2;
                var_r16->unk0->unk14 &= 0xFFFFFFDF;
                var_r16->unkA = 0;
                var_r16->unk30.unk0 = 0;
                var_r16->unk30.unk2 = 0;
                var_r16->unk30.unk4 = 0;
                temp_f0 = var_r16->unk6;
                var_r16->unkC.x = 0.0f;
                var_r16->unkC.y = 20.0f - (0.5f * (-0.004f * (temp_f0 * temp_f0)));
                var_r16->unkC.z = -60.0f;
                var_r16->unk18.x = 0.0f;
                var_r16->unk18.y = 0.0f;
                var_r16->unk18.z = 0.0f;
                break;
            case 2:
                var_r16->unk18.y += -0.004f;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unk6--;
                if (var_r16->unk6 <= 0)
                {
                    var_r16->unk4 = 3;
                    var_r16->unkC.x = 0.0f;
                    var_r16->unkC.y = 20.0f;
                    var_r16->unkC.z = -60.0f;
                }
                break;
            case 3:
                var_r16->unk18.x *= 0.92f;
                var_r16->unk18.y *= 0.92f;
                var_r16->unk18.z *= 0.92f;
                var_r16->unk18.y += -0.004f;
                var_r16->unk18.x += 0.05f * -var_r16->unkC.x;
                var_r16->unk18.y += 0.05f * (20.0f - var_r16->unkC.y);
                var_r16->unk18.z += 0.05f * (-60.0f - var_r16->unkC.z);
                var_r16->unkC.x += var_r16->unk18.x;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unkC.z += var_r16->unk18.z;
                break;
            case 4:
                var_r16->unk4 = 5;
                var_r16->unk6 = 0;
                var_r16->unk18.z = 0.0f;
                /* fallthrough */
            case 5:
                var_r16->unk18.z += 0.000075f * var_r16->unk6;
                var_r16->unkC.z += var_r16->unk18.z;
                var_r16->unk6++;
                break;
            case 6:
                var_r16->unk4 = 7;
                //temp_r3 = var_r16->unk0;
                var_r16->unk0->unk14 &= 0xFFFFFFDF;
                var_r16->unkA = 3;
                var_r16->unk30.unk0 = 0;
                var_r16->unk30.unk2 = (rand() / 32767.0f);
                var_r16->unk30.unk4 = 0;
                var_r16->unkC.x = 0.0f;
                var_r16->unkC.y = 0.0f;
                var_r16->unkC.z = 0.0f;
                var_r16->unk18.x = 0.0f;
                var_r16->unk18.y = 0.0f;
                var_r16->unk18.z = 0.0f;
                if (modeCtrl.playerCount == 1)
                    var_r16->unk56 = 3;
                else
                    var_r16->unk56 = modeCtrl.currPlayer;
                var_r16->unk58 = rand() & 0x7FFF;
                var_r16->unk5A = rand() & 0x7FFF;
                var_r16->unk5C = rand() & 0x7FFF;
                /* fallthrough */
            case 7:
                if (var_r16->unk6 > 0)
                {
                    var_r16->unk6--;
                    var_r16->unk18.y += 0.009799992;
                    memset(&effect, 0, sizeof(effect));
                    effect.type = 0xA;
                    effect.playerId = playerId;
                    effect.pos = var_r16->unkC;
                    spawn_effect(&effect);
                }
                else
                {
                    var_r16->unk18.x *= 0.92f;
                    var_r16->unk18.y *= 0.92f;
                    var_r16->unk18.z *= 0.92f;
                }
                var_r16->unkC.x += var_r16->unk18.x;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unkC.z += var_r16->unk18.z;
                var_r16->unk30.unk2 -= 0x30;
                var_r16->unk5A -= 0x40;
                break;
            case 8:
                var_r16->unk18.y += 0.009799992;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unk6--;
                if (var_r16->unk6 < 0)
                {
                    var_r16->unk4 = 0;
                    var_r16->unk0->unk14 |= 0x20;
                }
                memset(&effect, 0, sizeof(effect));
                effect.type = 0xA;
                effect.playerId = playerId;
                effect.pos = var_r16->unkC;
                spawn_effect(&effect);
                break;
            case 9:
                var_r16->unk4 = 0xA;
                var_r16->unk0->unk14 &= 0xFFFFFFDF;
                var_r16->unkA = 0x12;
                var_r16->unk30.unk0 = 0;
                var_r16->unk30.unk2 = i * 0x10000 / 3;
                var_r16->unk30.unk4 = 0;
                mathutil_mtxA_from_rotate_y(var_r16->unk30.unk2 + 0x8000);
                mathutil_mtxA_tf_vec_xyz(&var_r16->unkC, 0.0f, 0.0f, -5.0f);
                var_r16->unkC.y += lbl_802F6634 + (5.0f * (rand() / 32767.0f));
                var_r16->unk18.x = 0.0f;
                var_r16->unk18.y = lbl_802F6568;
                var_r16->unk18.z = 0.0f;
                if (i == playerCharacterSelection[playerId])
                {
                    if (modeCtrl.playerCount == 1)
                        var_r16->unk56 = 3;
                    else
                        var_r16->unk56 = i;
                }
                else
                    var_r16->unk56 = rand() & 3;
                var_r16->unk58 = rand() & 0x7FFF;
                var_r16->unk5A = rand() & 0x7FFF;
                var_r16->unk5C = rand() & 0x7FFF;
                /* fallthrough */
            case 10:
                raycast_stage_down(&var_r16->unkC, &sp8, NULL);
                if (var_r16->unk18.y < 0.0f
                 && (0.5f + (var_r16->unkC.y - sp8.pos.y) < lbl_802F6638 * var_r16->unk18.y))
                    var_r16->unk18.y += lbl_802F663C;
                memset(&effect, 0, sizeof(effect));
                effect.type = 0xA;
                effect.playerId = playerId;
                effect.pos = var_r16->unkC;
                spawn_effect(&effect);
                /* fallthrough */
            case 11:
                raycast_stage_down(&var_r16->unkC, &sp8, NULL);
                var_r16->unk18.x *= 0.99f;
                var_r16->unk18.y *= 0.99f;
                var_r16->unk18.z *= 0.99f;
                var_r16->unk18.y += lbl_802F6640;
                var_r16->unkC.x += var_r16->unk18.x;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unkC.z += var_r16->unk18.z;
                if (sp8.pos.y > var_r16->unkC.y - 0.5f - 0.1f)
                {
                    if (var_r16->unk18.y < lbl_802F65E4)
                        u_play_sound_0(0x69);
                    var_r16->unkC.y = 0.5f + sp8.pos.y - 0.1f;
                    if (var_r16->unk18.y < 0.0f)
                        var_r16->unk18.y *= lbl_802F65A0;
                    if (var_r16->unk4 != 0xB)
                    {
                        var_r16->unk4 = 0xB;
                        memset(&effect, 0, sizeof(effect));
                        effect.playerId = playerId;
                        effect.type = 0x1B;
                        effect.pos = var_r16->unkC;
                        effect.pos.y -= 0.4f;
                        effect.unk88.x = 0.0f;
                        effect.unk88.y = 1.0f;
                        effect.unk88.z = 0.0f;
                        effect.colorFactor = lbl_802F6528;
                        spawn_effect(&effect);
                    }
                }
                break;
            case 16:
                var_r16->unk4 = 0;
                var_r16->unkA = 0xC;
                var_r16->unkC = lbl_801E2AD4[i];
                if (raycast_stage_down(&var_r16->unkC, &sp8, NULL) != 0U)
                {
                    var_r16->unkC = sp8.pos;
                    var_r16->unkC.y += 0.4f;
                }
                var_r16->unk30.unk0 = 0;
                var_r16->unk30.unk2 = mathutil_atan2(var_r16->unkC.x, var_r16->unkC.z);
                var_r16->unk30.unk4 = 0;
                var_r16->unk56 = -1;
                break;
            case 17:
                var_r16->unk4 = 0x12;
                var_r16->unkA = 0xD;
                if (playerCharacterSelection[playerId] == 3)
                    var_r16->unkC = lbl_801E2AD4_2[i];
                else
                    var_r16->unkC = lbl_801E2AD4_2[(i + 3 - playerCharacterSelection[playerId]) % 3];
                if (raycast_stage_down(&var_r16->unkC, &sp8, NULL) != 0U)
                {
                    var_r16->unkC = sp8.pos;
                    var_r16->unkC.y += 0.4f;
                }
                var_r16->unk30.unk0 = 0;
                var_r16->unk30.unk2 = 0;
                var_r16->unk30.unk4 = 0;
                temp_f0 = 1.0f / modeCtrl.submodeTimer;
                var_r16->unk18.x = -var_r16->unkC.x * temp_f0;
                var_r16->unk18.y = 0.0f;
                var_r16->unk18.z = lbl_802F6644 * temp_f0 * (1.0f + (0.1f * (rand() / 32767.0f)));
                break;
            case 18:
                var_r16->unkC.x += var_r16->unk18.x;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unkC.z += var_r16->unk18.z;
                break;
            case 19:
                var_r16->unk4 = 0x14;
                var_r16->unk6 = modeCtrl.submodeTimer;
                var_r16->unkA = 0xD;
                var_r16->unkC = lbl_801E2AD4_2[i];
                var_r16->unkC.y = 0.0f;
                var_r16->unk30.unk0 = 0;
                var_r16->unk30.unk2 = 0;
                var_r16->unk30.unk4 = 0;
                temp_f0 = 1.0f / var_r16->unk6;
                var_r16->unk18.x = -var_r16->unkC.x * temp_f0;
                var_r16->unk18.y = 0.0f;
                var_r16->unk18.z = (3.45f - var_r16->unkC.z) * temp_f0;
                break;
            case 20:
                var_r16->unkC.x += var_r16->unk18.x;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unkC.z += var_r16->unk18.z;
                if (var_r16->unkC.z < 3.45f)
                {
                    var_r16->unk4 = 0;
                    var_r16->unkA = 9;
                    var_r16->unkC.z = 3.45f;
                }
                break;
            case 21:
                var_r16->unk4 = 0;
                var_r16->unkA = 0xE;
                spD0 = lbl_801E2A98[i];
                var_r16->unkC = spD0.unk0;
                var_r16->unk30 = spD0.unkC;
                switch (i)
                {
                case 0:
                    var_r16->unk3C = decodedBgGma->modelEntries[1].model;
                    var_r16->unk40 = 0xA;
                    var_r16->unk44 = 0.05f;
                    var_r16->unk48 = lbl_802F664C;
                    var_r16->unk4C = lbl_802F6650;
                    var_r16->unk50 = -0x58E3;
                    var_r16->unk52 = -0x1555;
                    var_r16->unk54 = 0x438E;
                    break;
                case 1:
                    var_r16->unk3C = decodedBgGma->modelEntries[0x17].model;
                    var_r16->unk40 = 0xF;
                    var_r16->unk44 = 0.05f;
                    var_r16->unk48 = 0.0f;
                    var_r16->unk4C = lbl_802F6654;
                    var_r16->unk50 = 0x20B7;
                    var_r16->unk52 = -0x6BE1;
                    var_r16->unk54 = -0x26E9;
                    break;
                }
                break;
            }
        }
    }
}

/*
const float lbl_802F64C8 = 0.01666666753590107f;
const float lbl_802F64CC = 400f;
const float lbl_802F64D0 = 0f;
const float lbl_802F64D4 = 20f;
const float lbl_802F64D8 = -64f;
const float lbl_802F64DC = 250f;
const float lbl_802F64E0 = 100f;
const float lbl_802F64E4 = 1f;
const float lbl_802F64E8 = -61f;
const double lbl_802F64F0 = 4503601774854144;
const float lbl_802F64F8 = 0.5f;
const float lbl_802F64FC = 20.75f;
const float lbl_802F6500 = 240f;
const float lbl_802F6504 = 0.80000001192092896f;
const float lbl_802F6508 = 0.40000000596046448f;
const float lbl_802F650C = 32767f;
const float lbl_802F6510 = -62f;
const float lbl_802F6514 = -1f;
const float lbl_802F6518 = 10f;
const float lbl_802F651C = -2f;
const float lbl_802F6520 = 1.25f;
const float lbl_802F6524 = 0.89999997615814209f;
const float lbl_802F6528 = 0.20000000298023224f;
const float lbl_802F652C = 0.10000000149011612f;
const float lbl_802F6530 = 1.5f;
const float lbl_802F6534 = 0.30000001192092896f;
const float lbl_802F6538 = -0.01666666753590107f;
const float lbl_802F653C = 0.60000002384185791f;
const float lbl_802F6540 = 0.19999998807907104f;
const float lbl_802F6544 = -0.0055555556900799274f;
const float lbl_802F6548 = -0.0033333334140479565f;
const float lbl_802F654C = 0.99000000953674316f;
const float lbl_802F6550 = -20.379999160766602f;
const float lbl_802F6554 = -2.3399999141693115f;
const float lbl_802F6558 = -20.389999389648438f;
const float lbl_802F655C = 12f;
const float lbl_802F6560 = 300f;
const float lbl_802F6564 = -4f;
const float lbl_802F6568 = -0.20000000298023224f;
const float lbl_802F656C = 6.5f;
const float lbl_802F6570 = 6f;
const float lbl_802F6574 = 3f;
const float lbl_802F6578 = 8f;
const float lbl_802F657C = 40f;
const float lbl_802F6580 = 4f;
const float lbl_802F6584 = -0.0023809524718672037f;
const float lbl_802F6588 = 5f;
const float lbl_802F658C = 2f;
const float lbl_802F6590 = 2.0999999046325684f;
const float lbl_802F6594 = 120f;
const float lbl_802F6598 = -0.40000000596046448f;
const float lbl_802F659C = -0.34999999403953552f;
const float lbl_802F65A0 = -0.60000002384185791f;
const float lbl_802F65A4 = -0.30000001192092896f;
const float lbl_802F65A8 = 14.800000190734863f;
const float lbl_802F65AC = 2.5f;
const float lbl_802F65B0 = 0.0083333337679505348f;
const float lbl_802F65B4 = 4.5f;
const float lbl_802F65B8 = 3.5f;
const float lbl_802F65BC = 0.25f;
const float lbl_802F65C0 = -2.5f;
const float lbl_802F65C4 = -0.34000000357627869f;
const float lbl_802F65C8 = -0.25f;
const float lbl_802F65CC = 7.8000001907348633f;
const float lbl_802F65D0 = 0.69999998807907104f;
const float lbl_802F65D4 = 4.1999998092651367f;
const float lbl_802F65D8 = 2.5999999046325684f;
const float lbl_802F65DC = 0.15000000596046448f;
const float lbl_802F65E0 = -0.5f;
const float lbl_802F65E4 = -0.10000000149011612f;
const float lbl_802F65E8 = -0.0020000000949949026f;
const float lbl_802F65EC = 2.9500000476837158f;
const float lbl_802F65F0 = -0.05000000074505806f;
const float lbl_802F65F4 = 768f;
const float lbl_802F65F8 = 2304f;
const float lbl_802F65FC = -2304f;
const float lbl_802F6600 = 0.98000001907348633f;
const float lbl_802F6604 = -0.0010000000474974513f;
const float lbl_802F6608 = 0.94999998807907104f;
const float lbl_802F660C = -0.85000002384185791f;
const float lbl_802F6610 = -0.0040000001899898052f;
const float lbl_802F6614 = -60f;
const float lbl_802F6618 = 0.92000001668930054f;
const float lbl_802F661C = 0.05000000074505806f;
const float lbl_802F6620 = 7.5000003562308848e-05f;
const double lbl_802F6628 = 0.0097999920000000004;
const float lbl_802F6630 = -5f;
const float lbl_802F6634 = 30f;
const float lbl_802F6638 = -15f;
const float lbl_802F663C = 0.012000000104308128f;
const float lbl_802F6640 = -0.0080000003799796104f;
const float lbl_802F6644 = -4.5f;
const float lbl_802F6648 = 3.4500000476837158f;
const float lbl_802F664C = 0.12999999523162842f;
const float lbl_802F6650 = 0.12000000476837158f;
const float lbl_802F6654 = -0.094000004231929779f;
const float lbl_802F6658 = -0.0099999997764825821f;
const float lbl_802F665C = 1.6666666269302368f;
const float lbl_802F6660 = 0.0039215688593685627f;
const float lbl_802F6664 = 0.004999999888241291f;
const double lbl_802F6668 = 4503599627370496;
const float lbl_802F6670 = 1024f;
const float lbl_802F6674 = 0.0010000000474974513f;
const float lbl_802F6678 = 2048f;
const float lbl_802F667C = 60f;
const float lbl_802F6680 = 80f;
const float lbl_802F6684 = 1.1000000238418579f;
const float lbl_802F6688 = 4096f;
const float lbl_802F668C = -0.125f;
const float lbl_802F6690 = 3.2000000476837158f;
const float lbl_802F6694 = 256f;
const float lbl_802F6698 = 16f;
const float lbl_802F669C = 0.15015999972820282f;
const float lbl_802F66A0 = 20.069999694824219f;
const float lbl_802F66A4 = 0.02500000037252903f;
const float lbl_802F66A8 = 0.070000000298023224f;
const float lbl_802F66AC = 0.984375f;
const float lbl_802F66B0 = 0.0099999997764825821f;
const float lbl_802F66B4 = 0.70710676908493042f;
const double lbl_802F66B8 = 0.30000001192092896;
const double lbl_802F66C0 = 0.69999998807907104;
const float lbl_802F66C8 = 0.96875f;
const double lbl_802F66D0 = 256;
const float lbl_802F66D8 = 16384f;
*/
