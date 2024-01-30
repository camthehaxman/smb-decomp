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
#include "textbox.h"

struct SomeBigEndingStruct_sub2
{
    u8 filler0[0x10];
    u32 unk10;
    u8 filler14[0xB4-0x14];
    u32 unkB4;
};

struct SomeBigEndingStruct_sub3
{
    struct SomeBigEndingStruct_sub2 *unkA804;  // -0x57fc
    u16 unk4;
    u16 unk6;
    /*0x08*/ u8 fillerA808[2];
    /*0x0A*/ u16 unkA80E;
    /*0x0C*/ u8 fillerA810[0xA83C-0xA810];
    /*0x38*/ s16 unkA83C;
    /*0x3A*/ s16 unk3A;
    u8 fillerA83E[0xA874-0xA83E - 2];
    u32 unkA874;  // -0x578c
    u8 fillerA878[0xAA54-0xA878];
};

static_assert(offsetof(struct SomeBigEndingStruct_sub3, unkA83C) == 0x38, moo);

struct SomeBigEndingStruct_sub
{
    u16 unk0;
    u16 unk2;
    Vec unk4;
    Vec unk10;
    u8 filler1C[0x10];
    Vec unk2C;
    Vec unk38;
    u16 unk44;
    u16 unk46;
    Vec unk48;
};  // size = 0x54

struct SomeBigEndingStruct
{
    float unk0;
    u8 filler4[0xA804-0x4];
    // maybe substruct SomeBigEndingStruct_sub3 starts here?
    struct SomeBigEndingStruct_sub3 unkA804;
    // substruct starts here
    struct SomeBigEndingStruct_sub unkAA54;
    u8 fillerAAA8[8];
    s32 unkAAB0;
    char *unkAAB4[2];
    u8 fillerAABC[0xAACC-0xAABC];
    u32 unkAACC;
    s32 unkAAD0;
};

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

u32 lbl_801E2A98[] =
{
	0x3F333333,
	0xBECCCCCD,
	0x3F333333,
	0x00008000,
	0,
    0xBF2CCCCD,
    0,
    0x3F400000,
    0x00008000,
    0,
    0x3CA3D70B,
    0xBECCCCCD,
    0x3F8CCCCD,
    0x00008000,
    0,
};

u32 lbl_801E2AD4[] =
{
	0,
	0x41200000,
	0x40B00000,
	0xBF99999A,
	0x41200000,
	0x40A00000,
	0x3F99999A,
	0x41200000,
	0x40A00000,
	0,
	0x41200000,
	0x40800000,
	0xBDCCCCCD,
	0x41200000,
	0x40B00000,
	0x3DCCCCCD,
	0x41200000,
	0x40D00000,
	0,
	0x41200000,
	0x41180000,
};

s16 lbl_801E2B28[][2] =
{
	0x00E8, 0x00E8,
	0x0101, 0x0101,
	0x016D, 0x016D,
	0x0186, 0x0187,
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
/*
    r3->unkAA58.x += r3->unkAA64.x;
    r3->unkAA58.y += r3->unkAA64.y;
    r3->unkAA58.z += r3->unkAA64.z;

    r3->unkAA80.x += r3->unkAA8C.x;
    r3->unkAA80.y += r3->unkAA8C.y;
    r3->unkAA80.z += r3->unkAA8C.z;
*/
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
    struct Camera *camera;
    struct Camera *cameraBackup;
    int i;
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
    lbl_802C6BD8.unk8->unkA804.unkA874 = 1;

    cameraBackup = currentCameraStructPtr;
    camera = &cameraInfo[0];
    for (i = 0; i < 4; i++, camera++)
    {
        currentCameraStructPtr = camera;
        camera->unk1F = 0;
    }
    currentCameraStructPtr = cameraBackup;

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
    if (lbl_802C6BD8.unk8->unkA804.unkA804 != NULL)
        lbl_802C6BD8.unk8->unkA804.unkA804->unkB4 = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0, 0x1E);
    u_play_music(0x44, 0);
}

extern s16 lbl_802F18C0[4];

void func_800B6848(void)
{
    f32 temp_f2;
    struct SomeBigEndingStruct_sub3 *temp_r4;
    struct SomeBigEndingStruct_sub *temp_r6;

    temp_r6 = &lbl_802C6BD8.unk8->unkAA54;
    temp_r4 = &lbl_802C6BD8.unk8->unkA804;
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
        SoundReq((u32) lbl_802F18C0[temp_r4->unkA804->unk10]);
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
    struct Camera *camera;
    struct Camera *cameraBackup;
    int i;
    struct SomeBigEndingStruct *temp_r30;
    struct SomeBigEndingStruct_sub *temp_r29;
    Vec sp30;
    struct TextBox tbox;
    struct SomeBigEndingStruct_sub3 *temp_r4;
    int i2;

    lbl_802C6BD8.unk2 = 4;
    modeCtrl.submodeTimer = 60;
    modeCtrl.unk18 = 0x1E;

    cameraBackup = currentCameraStructPtr;
    camera = &cameraInfo[0];
    for (i = 0; i < 4; i++, camera++)
    {
        currentCameraStructPtr = camera;
        camera->unk1F = 0;
    }
    currentCameraStructPtr = cameraBackup;

    temp_r29 = (void *)&lbl_802C6BD8.unk8->unkAA54;
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
        for (i2 = 0; i2 < tbox.numRows; i2++)
        {
            textbox_add_text(0, temp_r30->unkAAB4[temp_r30->unkAACC]);
            temp_r30->unkAAB0--;
            temp_r30->unkAACC++;
        }
    }
    temp_r4 = &lbl_802C6BD8.unk8->unkA804;
    temp_r4->unkA80E = 1;
    temp_r4->unkA83C = 0;
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
    temp_r30 = &lbl_802C6BD8.unk8->unkA804;
    mathutil_mtxA_from_rotate_z(unpausedFrameCounter << 6);
    mathutil_mtxA_tf_vec_xyz(&sp30, 0.0f, 0.5f, 0.0f);
    temp_r29->unk4.x = sp30.x;
    temp_r29->unk4.y = 20.75f + (0.5f * sp30.y);
    temp_r29->unk4.z = -61.0f + sp30.z;
    temp_r30->unkA83C--;
    if (temp_r30->unkA83C < 0)
    {
        SoundReq(lbl_801E2B28[temp_r30->unkA804->unk10][temp_r30->unk3A & 1]);
        temp_r30->unkA83C = 240.0f * (0.8f + (0.4f * (rand() / 32767.0f)));
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
    struct Camera *camera;
    struct Camera *cameraBackup;
    int i;
    struct SomeBigEndingStruct_sub *temp_r29;

    lbl_802C6BD8.unk2 = 6;
    modeCtrl.submodeTimer = 0x168;

    cameraBackup = currentCameraStructPtr;
    camera = &cameraInfo[0];
    for (i = 0; i < 4; i++, camera++)
    {
        currentCameraStructPtr = camera;
        camera->unk1F = 0;
    }
    currentCameraStructPtr = cameraBackup;

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
    lbl_802C6BD8.unk8->unkA804.unkA80E = 2;
}

extern s16 lbl_802F18C8[4];

void func_800B6F9C(void)
{
    switch (modeCtrl.submodeTimer)
    {
    case 0xF0:
        lbl_802C6BD8.unk8->unkA804.unk4 = 4;
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
    f32 temp_f31;
    struct SomeBigEndingStruct_sub *temp_r31;
    struct SomeBigEndingStruct_sub2 *temp_r3;
    struct Camera *camera;
    struct Camera *cameraBackup;
    int i;

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
    lbl_802C6BD8.unk8->unkA804.unk4 = 6;
    lbl_802C6BD8.unk8->unkA804.unk6 = modeCtrl.submodeTimer - 0xB4;

    cameraBackup = currentCameraStructPtr;
    camera = &cameraInfo[0];
    for (i = 0; i < 4; i++, camera++)
    {
        currentCameraStructPtr = camera;
        camera->unk1F = 0;
    }
    currentCameraStructPtr = cameraBackup;

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
    temp_r3 = lbl_802C6BD8.unk8->unkA804.unkA804;
    if (temp_r3 != NULL)
        temp_r3->unkB4 = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0U, 0x1E);
    u_play_music(0x44U, 0);
    u_play_sound_0(0x232);
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
