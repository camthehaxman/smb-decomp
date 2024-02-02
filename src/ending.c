#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "ending.h"
#include "event.h"
#include "gxcache.h"
#include "gxutil.h"
#include "input.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "mot_ape.h"
#include "nl2ngc.h"
#include "ord_tbl.h"
#include "rend_efc.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "textbox.h"

#include "../data/common.nlobj.h"

struct EndingInfo endingInfo;

static void ending_state_init(void);
static void ending_state_beginner_init(void);
static void ending_state_2(void);
static void ending_state_3(void);
static void ending_state_4(void);
static void ending_state_5(void);
static void ending_state_6(void);
static void ending_state_advanced_init(void);
static void ending_state_8(void);
static void ending_state_9(void);
static void ending_state_10(void);
static void ending_state_11(void);
static void ending_state_12(void);
static void ending_state_expert_init(void);
static void ending_state_14(void);
static void ending_state_15(void);
static void ending_state_16(void);
static void ending_state_17(void);
static void ending_state_18(void);
static void ending_state_19(void);
static void ending_state_20(void);
static void ending_state_21(void);
static void ending_state_22(void);
static void ending_state_23(void);
static void ending_state_24(void);
static void ending_state_25(void);
static void ending_state_26(void);
static void ending_state_27(void);
static void ending_state_28(void);
static void ending_state_29(void);
static void ending_state_30(void);
static void ending_state_31(void);
static void ending_state_32(void);
static void func_800BB068(void);
static void func_800BB084(void);
static void func_800BBA54(void);
static void func_800BBA58(void);
static void func_800BBB10(int);
static void func_800BBB78(void);
static void func_800BBBA8(void);
static void func_800BC820(void);
static void func_800BC8B8(void);
static int func_800BCE54(int, int);
static void lbl_800BCF6C(struct Ape *, int);
static void func_800BD470(void);
static void func_800BD4B8(void);
static void func_800BEEE8(void);
static void func_800BF2D0(void);
static void func_800BF2D4(void);
static void func_800BF4F4(void);
static void func_800BF658(void);
static void func_800BF774(void);
static void func_800BF8C4(int);
static void func_800BF948(int, int);
static void func_800BF9CC(int);
static void func_800BFA2C(void);
static void func_800BFC2C(void);

struct Struct801E2A98
{
    Vec unk0;
    S16Vec unkC;
};

static struct Struct801E2A98 lbl_801E2A98[] =
{
    {{0.7f, -0.4f, 0.7f}, {0, 0x8000, 0}},
    {{-0.675f, 0.0f, 0.75f}, {0, 0x8000, 0}},
    {{0.0200000014156f, -0.4f, 1.1f}, {0, 0x8000, 0}},
};

static Vec lbl_801E2AD4[] =
{
    {  0.0f, 10.0f, 5.5f },
    { -1.2f, 10.0f, 5.0f },
    {  1.2f, 10.0f, 5.0f },
};

static Vec lbl_801E2AD4_2[] =
{
    {  0.0f, 10.0f, 4.0f },
    { -0.1f, 10.0f, 5.5f },
    {  0.1f, 10.0f, 6.5f },
    {  0.0f, 10.0f, 9.5f },
};

static s16 lbl_801E2B28[][2] =
{
    { 0x00E8, 0x00E8 },
    { 0x0101, 0x0101 },
    { 0x016D, 0x016D },
    { 0x0186, 0x0187 },
};

enum
{
    ENDING_STATE_INIT = 0,
    ENDING_STATE_BEGINNER_INIT = 1,
    ENDING_STATE_ADVANCED_INIT = 7,
    ENDING_STATE_EXPERT_INIT = 13,
};

static void (*endingFuncs[])(void) =
{
    ending_state_init,
    ending_state_beginner_init,
    ending_state_2,
    ending_state_3,
    ending_state_4,
    ending_state_5,
    ending_state_6,
    ending_state_advanced_init,
    ending_state_8,
    ending_state_9,
    ending_state_10,
    ending_state_11,
    ending_state_12,
    ending_state_expert_init,
    ending_state_14,
    ending_state_15,
    ending_state_16,
    ending_state_17,
    ending_state_18,
    ending_state_19,
    ending_state_20,
    ending_state_21,
    ending_state_22,
    ending_state_23,
    ending_state_24,
    ending_state_25,
    ending_state_26,
    ending_state_27,
    ending_state_28,
    ending_state_29,
    ending_state_30,
    ending_state_31,
    ending_state_32,
};

void ending_init(void)
{
    memset(&endingInfo, 0, sizeof(endingInfo));
    endingInfo.state = 0;
    endingInfo.nextState = -1;
    event_finish_all();
    endingInfo.unk8 = OSAllocFromHeap(stageHeap, sizeof(*endingInfo.unk8));
    if (endingInfo.unk8 == NULL)
    {
        printf("fail to alloc memory for ending-management!\n");
        endingInfo.state = 31;
        return;
    }
    memset(endingInfo.unk8, 0, sizeof(*endingInfo.unk8));
    SoundGroupLoad(SOUND_GRPending);
    func_800BBB78();
    func_800BD470();
    func_800BB068();
    func_800C008C();
    func_800C09B0();
    lbl_802F1B7C = ending_finish;
}

int ending_main(void)
{
    struct SomeBigEndingStruct *r3;

    if (endingInfo.unk8 == NULL)
        return 0;
    if (gamePauseStatus & 0xA)
        return 1;
    if (endingInfo.nextState != -1)
        endingInfo.state = endingInfo.nextState;
    endingFuncs[endingInfo.state]();
    func_800BBBA8();
    func_800BD4B8();
    func_800BB084();
    if (endingInfo.unk4 & 2)
        endingInfo.unk8->unk0 += 0.01666666753590107f;

    r3 = endingInfo.unk8;

    r3->unkAA54.unk4.x += r3->unkAA54.unk10.x;
    r3->unkAA54.unk4.y += r3->unkAA54.unk10.y;
    r3->unkAA54.unk4.z += r3->unkAA54.unk10.z;

    r3->unkAA54.unk2C.x += r3->unkAA54.unk38.x;
    r3->unkAA54.unk2C.y += r3->unkAA54.unk38.y;
    r3->unkAA54.unk2C.z += r3->unkAA54.unk38.z;

    return endingInfo.state != 32;
}

void ending_finish(void)
{
    func_800C09B4();
    func_800C00F0();
    func_800BBA54();
    func_800BF2D0();
    func_800BC820();
    OSFreeToHeap(stageHeap, endingInfo.unk8);
    endingInfo.unk8 = NULL;
    event_finish_all();
    SoundGroupFree();
    if (lbl_802F1B7C == ending_finish)
        lbl_802F1B7C = NULL;
}

void u_ending_draw(void)
{
    if (endingInfo.unk8 == NULL)
        return;
    u_call_camera_apply_viewport(modeCtrl.currPlayer);
    u_draw_ball_shadow();
    func_80054FF0();
    func_800BF2D4();
    if (endingInfo.unk4 & 1)
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
        ord_tbl_set_depth_offset(400.0f);
        background_draw();
        ord_tbl_set_depth_offset(0.0f);
    }
    if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
        effect_draw();
    draw_monkey();
    ord_tbl_draw_nodes();
    if (endingInfo.unk4 & 0x20)
        func_800BFC2C();
}

static void ending_state_init(void)
{
    switch (modeCtrl.difficulty)
    {
    default:
    case DIFFICULTY_BEGINNER: endingInfo.nextState = ENDING_STATE_BEGINNER_INIT;  break;
    case DIFFICULTY_ADVANCED: endingInfo.nextState = ENDING_STATE_ADVANCED_INIT;  break;
    case DIFFICULTY_EXPERT:   endingInfo.nextState = ENDING_STATE_EXPERT_INIT;    break;
    }
}

static void ending_state_beginner_init(void)
{
    int stageId;
    struct SomeBigEndingStruct_sub *r28;
    Vec sp8;

    endingInfo.nextState = 2;
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
    endingInfo.unk8->unkA804[0].unk70 = 1;
    CAMERA_FOREACH(camera->unk1F = 0;)
    r28 = &endingInfo.unk8->unkAA54;
    memset(r28, 0, sizeof(*r28));
    r28->unk0 = 1;
    r28->unk4.x = 0.0f;
    r28->unk4.y = 20.0f;
    r28->unk4.z = -64.0f;
    r28->unk2 = 1;
    mathutil_mtxA_from_rotate_z(rand() & 0x7FFF);
    mathutil_mtxA_tf_vec_xyz(&sp8, 250.0f, 0.0f, 0.0f);
    r28->unk2C.x = sp8.x;
    r28->unk2C.y = sp8.y + 20.0f;
    r28->unk2C.z = sp8.z + 100.0f;
    if (endingInfo.unk8->unkA804[0].unk0 != NULL)
        endingInfo.unk8->unkA804[0].unk0->colorId = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0, 0x1E);
    u_play_music(0x44, 0);
}

static s16 lbl_802F18C0[4] = { 0x00EA, 0x0102, 0x0142, 0x0173 };

static void ending_state_2(void)
{
    float temp_f2;
    struct SomeBigEndingStruct_sub3 *temp_r4;
    struct SomeBigEndingStruct_sub *temp_r6;

    temp_r6 = &endingInfo.unk8->unkAA54;
    temp_r4 = &endingInfo.unk8->unkA804[0];
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
        SoundReq(lbl_802F18C0[temp_r4->unk0->charaId]);
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
        endingInfo.nextState = 3;
}

static void ending_state_3(void)
{
    struct SomeBigEndingStruct *temp_r30;
    struct SomeBigEndingStruct_sub *temp_r29;
    Vec sp30;
    struct TextBox tbox;
    struct SomeBigEndingStruct_sub3 *temp_r4;
    int i;

    endingInfo.nextState = 4;
    modeCtrl.submodeTimer = 60;
    modeCtrl.unk18 = 0x1E;
    CAMERA_FOREACH(camera->unk1F = 0;)
    temp_r29 = &endingInfo.unk8->unkAA54;
    memset(temp_r29, 0, sizeof(*temp_r29));
    temp_r29->unk0 = 1;
    mathutil_mtxA_from_rotate_z((s16) (unpausedFrameCounter << 6));
    mathutil_mtxA_tf_vec_xyz(&sp30, 0.0f, 0.5f, 0.0f);
    temp_r29->unk4.x = sp30.x;
    temp_r29->unk4.y = sp30.y * 0.5f + 20.75f;
    temp_r29->unk4.z = sp30.z + -61.0f;
    temp_r29->unk2 = 2;
    temp_r29->unk44 = 0;
    temp_r29->unk46 = 0;
    temp_r29->unk48.x = 0.0f;
    temp_r29->unk48.y = 0.0f;
    temp_r29->unk48.z = 0.0f;

    temp_r30 = endingInfo.unk8;
    temp_r30->unkAACC = 0;

    func_800C0DC0(&temp_r30->unkAAB0, (char *)&temp_r30->unkAAD0);
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
            textbox_add_text(0, temp_r30->textboxStrings[temp_r30->unkAACC]);
            temp_r30->unkAAB0--;
            temp_r30->unkAACC++;
        }
    }
    temp_r4 = &endingInfo.unk8->unkA804[0];
    temp_r4->unkA = 1;
    temp_r4->unk38 = 0;
}

static void ending_state_4(void)
{
    Vec sp30;
    struct TextBox tbox;
    struct SomeBigEndingStruct *temp_r30_2;
    struct SomeBigEndingStruct_sub *temp_r29;
    struct SomeBigEndingStruct_sub3 *temp_r30;
    int i2;

    temp_r29 = &endingInfo.unk8->unkAA54;
    temp_r30 = &endingInfo.unk8->unkA804[0];
    mathutil_mtxA_from_rotate_z(unpausedFrameCounter << 6);
    mathutil_mtxA_tf_vec_xyz(&sp30, 0.0f, 0.5f, 0.0f);
    temp_r29->unk4.x = sp30.x;
    temp_r29->unk4.y = 20.75f + (0.5f * sp30.y);
    temp_r29->unk4.z = -61.0f + sp30.z;
    temp_r30->unk38--;
    if (temp_r30->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[temp_r30->unk0->charaId][temp_r30->unk3A & 1]);
        temp_r30->unk38 = 240.0f * (0.8f + (0.4f * RAND_FLOAT()));
        temp_r30->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & 0x100))
    {
        modeCtrl.unk18 = 0x1E;
        temp_r30_2 = endingInfo.unk8;
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
                textbox_add_text(0, temp_r30_2->textboxStrings[temp_r30_2->unkAACC]);
                temp_r30_2->unkAAB0--;
                temp_r30_2->unkAACC++;
            }
        }
        else
        {
            endingInfo.nextState = 5;
            textbox_set_properties(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

static void ending_state_5(void)
{
    struct SomeBigEndingStruct_sub *temp_r29;

    endingInfo.nextState = 6;
    modeCtrl.submodeTimer = 0x168;
    CAMERA_FOREACH(camera->unk1F = 0;)
    temp_r29 = &endingInfo.unk8->unkAA54;
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
    endingInfo.unk8->unkA804[0].unkA = 2;
}

static s16 lbl_802F18C8[4] = { 0x00B7, 0x0102, 0x0141, 0x018B };

static void ending_state_6(void)
{
    switch (modeCtrl.submodeTimer)
    {
    case 0xF0:
        endingInfo.unk8->unkA804[0].unk4 = 4;
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
        endingInfo.nextState = 0x1F;
}

static void ending_state_advanced_init(void)
{
    int stageId;
    float temp_f31;
    struct SomeBigEndingStruct_sub *temp_r31;
    struct Ape *ape;

    endingInfo.nextState = 8;
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
    endingInfo.unk8->unkA804[0].unk4 = 6;
    endingInfo.unk8->unkA804[0].unk6 = modeCtrl.submodeTimer - 0xB4;
    CAMERA_FOREACH(camera->unk1F = 0;)
    temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x12C);
    temp_r31 = &endingInfo.unk8->unkAA54;
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
    ape = endingInfo.unk8->unkA804[0].unk0;
    if (ape != NULL)
        ape->colorId = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0U, 0x1E);
    u_play_music(0x44U, 0);
    u_play_sound_0(0x232);
}

static s16 lbl_802F18D0[4] = { 0x00EC, 0x0115, 0x016D, 0x0173 };
static s16 lbl_802F18D8[4] = { 0x00B5, 0x00FF, 0x0138, 0x0192 };

static void ending_state_8(void)
{
    Vec spB4;
    struct Effect sp8;
    float temp_f30;
    float temp_f31;
    struct SomeBigEndingStruct_sub3 *temp_r30;
    struct SomeBigEndingStruct_sub *temp_r29;

    temp_r29 = &endingInfo.unk8->unkAA54;
    temp_r30 = &endingInfo.unk8->unkA804[0];
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
        temp_r30->unk0->flags &= 0xFFFFCFFF;
        if (modeCtrl.playerCount > 1)
            temp_r30->unk56 = modeCtrl.currPlayer;
        else
            temp_r30->unk56 = 3;
        func_8004CFF0(0x2E);
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x38);
        temp_f30 = 1.25f * (0.9f + (0.2f * RAND_FLOAT()));
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
        temp_r30->unk0->flags &= 0xFFFFCFFF;
        if (modeCtrl.playerCount > 1)
            temp_r30->unk56 = modeCtrl.currPlayer;
        else
            temp_r30->unk56 = 3;
        func_8004CFF0(0x2E);
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp_f31 = 1.0f / modeCtrl.submodeTimer;
        temp_f30 = 10.0f * (0.9f + (0.2f * RAND_FLOAT()));
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
            temp_r30->unk0->flags |= 0x1000;
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
            temp_f31 = 0.2f * (1.0f + (0.2f * RAND_FLOAT()));
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
        endingInfo.nextState = 9;
        endingInfo.unk4 &= 0xFFFFFFF7;
    }
}

static void ending_state_9(void)
{
    struct TextBox tbox;
    int i;
    u8 unused[4];
    struct SomeBigEndingStruct *temp_r30;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct SomeBigEndingStruct_sub3 *temp_r3_2;

    endingInfo.nextState = 0xA;
    modeCtrl.submodeTimer = 0x3C;
    modeCtrl.unk18 = 0x1E;
    temp_r30 = endingInfo.unk8;
    temp_r30->unkAACC = 0;
    func_800C0DC0(&temp_r30->unkAAB0, (char *)&temp_r30->unkAAD0);
    if (temp_r30->unkAAB0 > 0)
    {
        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_DOWN;
        tbox.x = 0x140;
        tbox.y = 0xA0;
        tbox.numRows = MIN(temp_r30->unkAAB0, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, 1, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, temp_r30->textboxStrings[temp_r30->unkAACC]);
            temp_r30->unkAAB0--;
            temp_r30->unkAACC++;
        }
    }
    temp_r28 = &endingInfo.unk8->unkAA54;
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

    temp_r3_2 = &endingInfo.unk8->unkA804[0];
    temp_r3_2->unkA = 6;
    temp_r3_2->unk0->flags &= 0xFFFFEFFF;
    temp_r3_2->unk38 = 0;
}

static void ending_state_10(void)
{
    struct TextBox tbox;
    int i;
    struct SomeBigEndingStruct *temp_r30_2;
    struct SomeBigEndingStruct_sub3 *temp_r30;

    temp_r30 = &endingInfo.unk8->unkA804[0];
    temp_r30->unk38--;
    if (temp_r30->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[temp_r30->unk0->charaId][temp_r30->unk3A & 1]);
        temp_r30->unk38 = 240.0f * (0.8f + (0.4f * RAND_FLOAT()));
        temp_r30->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & 0x100))
    {
        modeCtrl.unk18 = 0x1E;
        temp_r30_2 = endingInfo.unk8;
        if (temp_r30_2->unkAAB0 != 0)
        {
            textbox_set_properties(0, 0x14, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = TEXTBOX_STYLE_CENTER_DOWN;
            tbox.x = 0x140;
            tbox.y = 0xA0;
            tbox.numRows = temp_r30_2->unkAAB0;
            tbox.callback = NULL;
            textbox_set_properties(0, 1, &tbox);
            for (i = 0; i < tbox.numRows; i++)
            {
                textbox_add_text(0, temp_r30_2->textboxStrings[temp_r30_2->unkAACC]);
                temp_r30_2->unkAAB0--;
                temp_r30_2->unkAACC++;
            }
        }
        else
        {
            endingInfo.nextState = 0xB;
            textbox_set_properties(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

static void ending_state_11(void)
{
    struct Ape *ape;

    endingInfo.nextState = 0xC;
    modeCtrl.submodeTimer = 0x21C;
    endingInfo.unk8->unkA804[0].unkA = 7;
    ape = endingInfo.unk8->unkA804[0].unk0;
    ape->flags &= 0xFFFFBFFF;
}

static s16 lbl_802F18E0[4] = { 0x00EA, 0x0102, 0x016D, 0x0173 };

void ending_state_12(void)
{
    struct SomeBigEndingStruct_sub *temp_r28;
    struct SomeBigEndingStruct_sub3 *temp_r7;
    void *temp_r4;
    int tm;
    u8 unused[8];

    temp_r28 = &endingInfo.unk8->unkAA54;
    temp_r7 = &endingInfo.unk8->unkA804[0];
    switch (modeCtrl.submodeTimer)
    {
    case 0x21B:
        if (temp_r7->unk0->unk9C != 7)
            modeCtrl.submodeTimer += 1;
        else
        {
            temp_r7->unk0->flags |= 0x1000;
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
        endingInfo.unk4 |= 0x20;
        break;
    case 0xF0:
        endingInfo.unk4 &= 0xFFFFFFDF;
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
        endingInfo.unk8->unkAAA8 = 0.5f + 0.5f * (-0.016666668f * (float)(360 - tm));
    else if (tm < 300 && tm > 240)
        endingInfo.unk8->unkAAA8 = 0.5f + 0.5f * (1.0f - (-0.016666668f * (float)(240 - tm)));
    if (modeCtrl.submodeTimer < 420 && modeCtrl.submodeTimer > 240)
        endingInfo.unk8->unkAAAC = 0.6f + 0.19999999f * (-0.0055555557f * (float)(240 - modeCtrl.submodeTimer));
    if (modeCtrl.submodeTimer < 300 && modeCtrl.submodeTimer > 0)
    {
        float t = -0.0033333334f * -modeCtrl.submodeTimer;
        t = 1.0f - t;
        if (RAND_FLOAT() < t)
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
        endingInfo.nextState = 0x1F;
}

static void ending_state_expert_init(void)
{
    int temp_r0;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct Ape *temp_r3;
    int stageId;

    endingInfo.nextState = 14;
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
    endingInfo.unk8->unkA804[0].unk4 = 9;
    endingInfo.unk8->unkA804[1].unk4 = 9;
    endingInfo.unk8->unkA804[2].unk4 = 9;
    CAMERA_FOREACH(camera->unk1F = 0;)
    temp_r28 = &endingInfo.unk8->unkAA54;
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
        struct SomeBigEndingStruct_sub3 *temp_r28_2 = &endingInfo.unk8->unkA804[temp_r0];
        temp_r28_2->unk0->flags &= 0xFFFFFFDF;
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
    temp_r3 = endingInfo.unk8->unkA804[modeCtrl.currPlayer].unk0;
    if (temp_r3 != NULL)
        temp_r3->colorId = modeCtrl.currPlayer;
    start_screen_fade(0x100, 0U, 0x1E);
    u_play_music(0x44U, 0);
}

static void ending_state_14(void)
{
    struct SomeBigEndingStruct_sub *temp_r30 = &endingInfo.unk8->unkAA54;

    mathutil_mtxA_from_rotate_y(modeCtrl.submodeTimer * 4);
    mathutil_mtxA_tf_vec_xyz(&temp_r30->unk4, 0.0f, 20.0f, 12.0f);
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        endingInfo.nextState = 0xF;
}

static void ending_state_15(void)
{
    struct TextBox tbox;
    float temp_f31;
    struct SomeBigEndingStruct *temp_r28;
    struct SomeBigEndingStruct_sub *temp_r26;
    struct SomeBigEndingStruct_sub3 *temp_r27;
    struct SomeBigEndingStruct_sub3 *temp_r6;
    int i;

    endingInfo.nextState = 0x10;
    modeCtrl.submodeTimer = 0x12C;
    modeCtrl.unk18 = 0x1E;
    CAMERA_FOREACH(camera->unk1F = 0;)
    temp_r26 = &endingInfo.unk8->unkAA54;
    temp_r27 = &endingInfo.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]];
    temp_f31 = modeCtrl.submodeTimer / 300.0f;
    mathutil_mtxA_from_translate(&temp_r27->unkC);
    mathutil_mtxA_rotate_y(temp_r27->unk30.y);
    mathutil_mtxA_rotate_x(temp_r27->unk30.x);
    mathutil_mtxA_rotate_z(temp_r27->unk30.z);
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
    endingInfo.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]].unk70 = 1;
    temp_r28 = endingInfo.unk8;
    temp_r28->unkAACC = 0;
    func_800C0DC0(&temp_r28->unkAAB0, (char *)&temp_r28->unkAAD0);
    if (temp_r28->unkAAB0 > 0)
    {
        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_UP;
        tbox.x = 0x140;
        tbox.y = 0x142;
        tbox.numRows = MIN(temp_r28->unkAAB0, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, 1, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, temp_r28->textboxStrings[temp_r28->unkAACC]);
            temp_r28->unkAAB0--;
            temp_r28->unkAACC++;
        }
    }
    temp_r6 = &endingInfo.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]];
    temp_r6->unk38 = 0;
    if (playerCharacterSelection[modeCtrl.currPlayer] != 3)
        temp_r6->unkA = 0xB;
    else
        temp_r6->unkA = 0xC;
    endingInfo.unk8->unkA804[0].unk36 |= 1;
    endingInfo.unk8->unkA804[1].unk36 |= 1;
    endingInfo.unk8->unkA804[2].unk36 |= 1;
    endingInfo.unk8->unkA804[3].unk36 |= 1;
}

static void ending_state_16(void)
{
    struct TextBox tbox;
    float temp_f31;
    struct SomeBigEndingStruct *temp_r30_2;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct SomeBigEndingStruct_sub3 *temp_r30;
    int i;

    temp_r28 = &endingInfo.unk8->unkAA54;
    temp_r30 = &endingInfo.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]];
    temp_f31 = modeCtrl.submodeTimer / 300.0f;
    mathutil_mtxA_from_translate(&temp_r30->unkC);
    mathutil_mtxA_rotate_y(temp_r30->unk30.y);
    mathutil_mtxA_rotate_x(temp_r30->unk30.x);
    mathutil_mtxA_rotate_z(temp_r30->unk30.z);
    mathutil_mtxA_tf_point_xyz(&temp_r28->unk4, -4.0f * (temp_f31 - 0.5f), -0.2f, -2.0f);
    temp_r30->unk38--;
    if (temp_r30->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[temp_r30->unk0->charaId][temp_r30->unk3A & 1]);
        temp_r30->unk38 = 240.0f * (0.8f + (0.4f * RAND_FLOAT()));
        temp_r30->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & 0x100))
    {
        modeCtrl.unk18 = 0x1E;
        temp_r30_2 = endingInfo.unk8;
        if (temp_r30_2->unkAAB0 != 0)
        {
            textbox_set_properties(0, 0x14, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = TEXTBOX_STYLE_CENTER_UP;
            tbox.x = 0x140;
            tbox.y = 0x142;
            tbox.numRows = temp_r30_2->unkAAB0;
            tbox.callback = NULL;
            textbox_set_properties(0, 1, &tbox);
            for (i = 0; i < tbox.numRows; i++)
            {
                textbox_add_text(0, temp_r30_2->textboxStrings[temp_r30_2->unkAACC]);
                temp_r30_2->unkAAB0--;
                temp_r30_2->unkAACC++;
            }
        }
        else
        {
            endingInfo.nextState = 0x11;
            textbox_set_properties(0, 0x14, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

static void ending_state_17(void)
{
    endingInfo.nextState = 0x12;
    modeCtrl.submodeTimer = 0x1E0;
    func_800BBB10(modeCtrl.submodeTimer - 0x1A4);
    endingInfo.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]].unk74 = cameraInfo[0].eye;
}

static s16 lbl_802F18E8[3] = { 0x00E4, 0x0127, 0x014A };

static void ending_state_18(void)
{
    f32 temp_f2;
    f32 temp_f3;
    struct SomeBigEndingStruct_sub *temp_r26;
    struct SomeBigEndingStruct_sub3 *temp_r27;
    struct Ape *ape;
    struct SomeBigEndingStruct *temp;
    Vec sp8;

    switch (modeCtrl.submodeTimer)
    {
    case 0x1DF:
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp_f3 = 1.0f / (modeCtrl.submodeTimer - 0x168);
        temp = endingInfo.unk8;
        temp->unkAA54.unk0 = 1;
        temp->unkAA54.unk4.x = 0.0f;
        temp->unkAA54.unk4.y = 0.0f;
        temp->unkAA54.unk4.z = 6.5f;
        temp->unkAA54.unk10.x = -temp->unkAA54.unk4.x * temp_f3;
        temp->unkAA54.unk10.y = (1.5f - temp->unkAA54.unk4.y) * temp_f3;
        temp->unkAA54.unk10.z = (6.0f - temp->unkAA54.unk4.z) * temp_f3;
        temp->unkAA54.unk2 = 1;
        temp->unkAA54.unk2C = endingInfo.unk8->unkAA04.unk8;
        temp->unkAA54.unk38.x = 0.0f;
        temp->unkAA54.unk38.y = 0.0f;
        temp->unkAA54.unk38.z = 0.0f;
        break;
    case 0x1A4:
        ape = endingInfo.unk8->unkA804[playerCharacterSelection[modeCtrl.currPlayer]].unk0;
        ape->flags &= 0xFFFFFFBF;
        u_play_sound_0(0x234);
        break;
    case 0x168:
        endingInfo.unk8->unkA804[0].unkA = 8;
        endingInfo.unk8->unkA804[1].unkA = 8;
        endingInfo.unk8->unkA804[2].unkA = 8;
        SoundReq(lbl_802F18E8[playerCharacterSelection[modeCtrl.currPlayer] % 3]);
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp = endingInfo.unk8;
        temp_r26 = &temp->unkAA54;
        temp_r27 = &temp->unkA804[playerCharacterSelection[modeCtrl.currPlayer] % 3];
        memset(temp_r26, 0, sizeof(*temp_r26));
        mathutil_mtxA_from_translate(&temp_r27->unkC);
        mathutil_mtxA_rotate_y(temp_r27->unk30.y);
        mathutil_mtxA_rotate_x(temp_r27->unk30.x);
        mathutil_mtxA_rotate_z(temp_r27->unk30.z);
        temp_r26->unk0 = 1;
        mathutil_mtxA_tf_point_xyz(&temp_r26->unk4, 1.0f, 3.0f, -4.0f);
        temp_r26->unk10.y = (8.0f - temp_r26->unk4.y) * (1.0f / modeCtrl.submodeTimer);
        mathutil_mtxA_tf_point_xyz(&temp_r26->unk2C, 0.0f, 0.2f, 0.0f);
        break;
    case 0x12C:
        func_800BF948(1, modeCtrl.submodeTimer - 0x1E);
        temp_r26 = &endingInfo.unk8->unkAA54;
        temp_f2 = 1.0f / modeCtrl.submodeTimer;
        temp_r26->unk10.x = temp_r26->unk4.x * temp_f2;
        temp_r26->unk10.z = temp_r26->unk4.z * temp_f2;
        temp_r26->unk38.x = -temp_r26->unk2C.x * temp_f2;
        temp_r26->unk38.y = (40.0f - temp_r26->unk2C.y) * temp_f2;
        temp_r26->unk38.z = -temp_r26->unk2C.z * temp_f2;
        break;
    case 0x78:
        endingInfo.unk8->unkA804[0].unk4 = 0x10;
        endingInfo.unk8->unkA804[1].unk4 = 0x10;
        endingInfo.unk8->unkA804[2].unk4 = 0x10;
        endingInfo.unk8->unkA804[3].unk56 = -1;
        sp8.x = 0.0f;
        sp8.y = 4.0f;
        sp8.z = 0.0f;
        endingInfo.unk8->unkA804[0].unk70 = 2;
        endingInfo.unk8->unkA804[1].unk70 = 2;
        endingInfo.unk8->unkA804[2].unk70 = 2;
        endingInfo.unk8->unkA804[0].unk74 = sp8;
        endingInfo.unk8->unkA804[1].unk74 = sp8;
        endingInfo.unk8->unkA804[2].unk74 = sp8;
        endingInfo.unk8->unkA804[0].unkA = 0xC;
        endingInfo.unk8->unkA804[1].unkA = 0xC;
        endingInfo.unk8->unkA804[2].unkA = 0xC;
        break;
    case 0x1E:
        start_screen_fade(1, 0xFFFFFFU, modeCtrl.submodeTimer);
        u_play_sound_0(0x22F);
        break;
    }
    if (modeCtrl.submodeTimer < 0x1DF && modeCtrl.submodeTimer > 0x1A4)
        endingInfo.unk8->unkAA54.unk2C = endingInfo.unk8->unkAA04.unk8;
    if (modeCtrl.submodeTimer < 0x1A4 && modeCtrl.submodeTimer > 0.0f)
    {
        float t = -0.0023809525f * -(float)modeCtrl.submodeTimer;
        t = 1.0f - t;
        if (RAND_FLOAT() < t)
            func_800BF8C4(1);
        if (unpausedFrameCounter % 90 == 0)
            u_play_sound_0(0x22E);
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        endingInfo.nextState = 0x13;
}

static void ending_state_19(void)
{
    float temp_f4;
    struct SomeBigEndingStruct_sub *temp_r28;
    u8 unused[8];

    endingInfo.nextState = 0x14;
    modeCtrl.submodeTimer = 0x168;
    start_screen_fade(0, 0U, 0x1E);
    func_800BD470();
    func_800BF4F4();
    CAMERA_FOREACH(camera->unk1F = 0;)
    temp_r28 = &endingInfo.unk8->unkAA54;
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
    endingInfo.unk8->unkA804[0].unk38 = (s16) (1.0f + (120.0f * RAND_FLOAT()));
    endingInfo.unk8->unkA804[1].unk38 = (s16) (1.0f + (120.0f * RAND_FLOAT()));
    endingInfo.unk8->unkA804[2].unk38 = (s16) (1.0f + (120.0f * RAND_FLOAT()));
}

static s16 lbl_802F18F0[4] = { 0x00E8, 0x0101, 0x016B, 0x0000 };

static void ending_state_20(void)
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
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp_r4 = &endingInfo.unk8->unkAA54;
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
    var_r28 = endingInfo.unk8->unkA804;
    for (i = 3; i > 0; i--, var_r28++)  // BUG: maybe should be >= 0 instead?
    {
        if (var_r28->unk38 != 0)
        {
            var_r28->unk38--;
            if (var_r28->unk38 == 0)
                SoundReq(lbl_802F18F0[var_r28->unk0->charaId]);
        }
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        endingInfo.nextState = 0x15;
}

static void ending_state_21(void)
{
    struct SomeBigEndingStruct *temp_r3;
    float temp_f3;
    struct SomeBigEndingStruct_sub *temp_r4;

    endingInfo.nextState = 0x16;
    modeCtrl.submodeTimer = 0x2D0;
    endingInfo.unk8->unkA804[0].unk0->flags |= 0x20;
    endingInfo.unk8->unkA804[1].unk0->flags |= 0x20;
    endingInfo.unk8->unkA804[2].unk0->flags |= 0x20;
    endingInfo.unk8->unkA804[3].unk0->flags |= 0x20;
    CAMERA_FOREACH(camera->unk1F = 0;)
    temp_r4 = &endingInfo.unk8->unkAA54;
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

static void ending_state_22(void)
{
    float temp_f2;
    struct SomeBigEndingStruct_sub *temp;

    switch (modeCtrl.submodeTimer)
    {
    case 0x2B2:
        endingInfo.unk4 |= 4;
        u_play_sound_0(0x235);
        break;
    case 0x258:
        endingInfo.unk8->unkA804[0].unk4 = 0x11;
        endingInfo.unk8->unkA804[1].unk4 = 0x11;
        endingInfo.unk8->unkA804[2].unk4 = 0x11;
        break;
    case 0x257:
        endingInfo.unk8->unkA804[0].unk0->flags &= ~0x20;
        endingInfo.unk8->unkA804[1].unk0->flags &= ~0x20;
        endingInfo.unk8->unkA804[2].unk0->flags &= ~0x20;
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp = &endingInfo.unk8->unkAA54;
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
        endingInfo.unk8->unkA804[3].unk0->flags &= ~0x20;
        endingInfo.unk8->unkA804[3].unk4 = 0x13;
        break;
    case 0xD8:
        temp = &endingInfo.unk8->unkAA54;
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
        temp = &endingInfo.unk8->unkAA54;
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
        endingInfo.unk4 &= 0xFFFFFFFB;
        u_play_sound_0(0x236);
        temp = &endingInfo.unk8->unkAA54;
        temp->unk0 = 1;
        temp->unk10.x = 0.0f;
        temp->unk10.y = 0.0f;
        temp->unk10.z = 0.0f;
        break;
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        endingInfo.nextState = 0x17;
}

static void ending_state_23(void)
{
    struct SomeBigEndingStruct_sub *temp;

    endingInfo.nextState = 0x18;
    modeCtrl.submodeTimer = 0x12C;
    endingInfo.unk8->unkA804[0].unk4 = 0x15;
    endingInfo.unk8->unkA804[1].unk4 = 0x15;
    endingInfo.unk8->unkA804[2].unk4 = 0x15;
    endingInfo.unk8->unkA804[0].unk70 = 0;
    endingInfo.unk8->unkA804[1].unk70 = 0;
    endingInfo.unk8->unkA804[2].unk70 = 0;
    temp = &endingInfo.unk8->unkAA54;
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
    endingInfo.unk8->unkA804[0].unk36 &= ~0x1;
    endingInfo.unk8->unkA804[1].unk36 &= ~0x1;
    endingInfo.unk8->unkA804[2].unk36 &= ~0x1;
}

static void ending_state_24(void)
{
    struct SomeBigEndingStruct_sub3 *temp_r29 = &endingInfo.unk8->unkA804[3];
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
        endingInfo.nextState = 0x19;
        endingInfo.unk4 |= 2;
        endingInfo.unk8->unk0 = 0.0f;
        u_play_sound_0(0x238);
    }
}

static void ending_state_25(void)
{
    endingInfo.nextState = 0x1A;
    modeCtrl.submodeTimer = 0x1E0;
    modeCtrl.unk18 = 0;
    endingInfo.unk8->unkAA04.unk2 = 4;
}

static void ending_state_26(void)
{
    struct SomeBigEndingStruct_sub *temp_r4 = &endingInfo.unk8->unkAA54;
    float temp_f2;

    switch (modeCtrl.submodeTimer)
    {
    case 0x1D1:
        CAMERA_FOREACH(camera->unk1F = 0;)
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
            endingInfo.nextState = 0x1B;
        else
            endingInfo.nextState = 0x1D;
    }
}

static void ending_state_29(void)
{
    endingInfo.nextState = 0x1E;
    modeCtrl.submodeTimer = 0x14A;
    modeCtrl.unk18 = 0;
}

static void ending_state_30(void)
{
    float temp_f2;
    int charaId;
    struct SomeBigEndingStruct_sub *var_r31 = &endingInfo.unk8->unkAA54;
    struct SomeBigEndingStruct_sub3 *temp_r6;
    Vec sp8;

    switch (modeCtrl.submodeTimer)
    {
    case 0x14A:
        CAMERA_FOREACH(camera->unk1F = 0;)
        var_r31 = &endingInfo.unk8->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 0xD2);
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        temp_r6 = &endingInfo.unk8->unkA804[charaId];
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
        endingInfo.unk8->unkA804[3].unkA = 0x11;
        temp_r6 = &endingInfo.unk8->unkA804[charaId];
        CAMERA_FOREACH(camera->unk1F = 0;)
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
            endingInfo.nextState = 0x1F;
    }
}

static void ending_state_27(void)
{
    endingInfo.nextState = 0x1C;
    modeCtrl.submodeTimer = 0x276;
    modeCtrl.unk18 = 0;
}

static s16 lbl_802F18F8[4] = { 0x00E8, 0x0102, 0x016D, 0x0000 };

static void ending_state_28(void)
{
    struct Ape *ape;
    struct SomeBigEndingStruct_sub3 *temp_r27;
    int charaId;
    struct SomeBigEndingStruct_sub *temp_r30;
    Vec sp38;
    Mtx sp8;
    float temp_f31;

    temp_r30 = &endingInfo.unk8->unkAA54;
    switch (modeCtrl.submodeTimer)
    {
    case 0x276:
        endingInfo.unk4 |= 0x10;
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x1FE);
        charaId = (playerCharacterSelection[modeCtrl.currPlayer] + 2) % 3;
        temp_r30 = &endingInfo.unk8->unkAA54;
        temp_r27 = &endingInfo.unk8->unkA804[charaId];
        endingInfo.unk8->unkAA54.unk0 = 1;
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
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0x168);
        charaId = (playerCharacterSelection[modeCtrl.currPlayer] + 1) % 3;
        temp_r30 = &endingInfo.unk8->unkAA54;
        temp_r27 = &endingInfo.unk8->unkA804[charaId];
        endingInfo.unk8->unkAA54.unk0 = 1;
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
        CAMERA_FOREACH(camera->unk1F = 0;)
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 0xD2);
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        temp_r30 = &endingInfo.unk8->unkAA54;
        temp_r27 = &endingInfo.unk8->unkA804[charaId];
        endingInfo.unk8->unkAA54.unk0 = 1;
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
        endingInfo.unk8->unkA804[charaId].unkA = 0x10;
        temp_r27 = &endingInfo.unk8->unkA804[charaId];
        temp_r30->unk0 = 1;
        ape = temp_r27->unk0;
        mathutil_mtxA_from_quat(&ape->unk60);
        mathutil_mtxA_to_mtx(sp8);
        mathutil_mtxA_from_translate(&ape->unk30);
        mathutil_mtxA_scale_s(ape->modelScale);
        mathutil_mtxA_translate(&ape->unk3C);
        mathutil_mtxA_mult_right(sp8);
        mathutil_mtxA_mult_right(ape->unk0->joints[5].transformMtx);
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
            endingInfo.nextState = 0x1F;
    }
}

static void ending_state_31(void)
{
    endingInfo.nextState = 0x20;
}

static void ending_state_32(void) {}

static void func_800BB068(void)
{
    endingInfo.unk8->unkAA04.unk0 = 0;
}

static void func_800BB084(void)
{
    struct SomeStruct *temp_r30 = &endingInfo.unk8->unkAA04;
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
                temp_r30->unk44.x += 0.1f * (RAND_FLOAT() - 0.5f);
                temp_r30->unk44.y += 0.1f * (RAND_FLOAT() - 0.5f);
                temp_r30->unk44.z += 0.1f * (RAND_FLOAT() - 0.5f);
            }
            temp_r30->unk20.x = 0.4f + temp_r30->unk8.x;
            temp_r30->unk20.y = temp_r30->unk8.y;
            temp_r30->unk20.z = 2.95f + temp_r30->unk8.z;
            break;
        case 3:
            if (endingInfo.unk4 & 4)
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
                            temp_r30->unk44.x += 0.1f * (RAND_FLOAT() - 0.5f);
                            temp_r30->unk44.y += 0.1f * (RAND_FLOAT() - 0.5f);
                            temp_r30->unk44.z += 0.1f * (RAND_FLOAT() - 0.5f);
                        }
                        temp_r30->unk40 = temp_r30->unk40 * -0.4f;
                    }
                }
            }
            break;
        case 4:
            temp_r30->unk2 = 5;
            temp_r30->unk4 = 0x3C;
            temp_r30->unk3E += 768.0f * (RAND_FLOAT() - 0.5f);
            temp_r30->unk40 += 768.0f * (RAND_FLOAT() - 0.5f);
            temp_r30->unk42 += 2304.0f * (RAND_FLOAT() - 0.5f);
            temp_r30->unk44.x += 0.1f * (RAND_FLOAT() - 0.5f);
            temp_r30->unk44.y += 0.1f * (RAND_FLOAT() - 0.5f);
            temp_r30->unk44.z += 0.1f * (RAND_FLOAT() - 0.5f);
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
                temp_r30->unk3E += 2304.0f * (RAND_FLOAT() - 0.5f);
                temp_r30->unk40 += -2304.0f * -RAND_FLOAT();
                temp_r30->unk42 += 2304.0f * (RAND_FLOAT() - 0.5f);
                temp_r30->unk44.x += 0.1f * (RAND_FLOAT() - 0.5f);
                temp_r30->unk44.y += 0.1f * (RAND_FLOAT() - 0.5f);
                temp_r30->unk44.z += 0.1f * (RAND_FLOAT() - 0.5f);
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

static void func_800BBA54(void) {}

static void func_800BBA58(void)
{
    struct GMAModel *model;
    struct SomeStruct *temp_r31 = &endingInfo.unk8->unkAA04;

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

static void func_800BBB10(int arg0)
{
    struct SomeStruct *temp_r31 = &endingInfo.unk8->unkAA04;

    memset(temp_r31, 0, sizeof(*temp_r31));
    temp_r31->unk0 = 1;
    temp_r31->unk2 = 1;
    temp_r31->unk4 = arg0;
}

static void func_800BBB78(void)
{
    struct SomeBigEndingStruct *temp = endingInfo.unk8;

    if (temp != NULL)
    {
        temp->unkA804[0].unk56 = -1;
        temp->unkA804[1].unk56 = -1;
        temp->unkA804[2].unk56 = -1;
        temp->unkA804[3].unk56 = -1;
    }
}

static void func_800BBBA8(void)
{
    int i;
    struct Struct801E2A98 spD0;
    struct Effect effect;
    struct RaycastHit sp8;
    int playerId = modeCtrl.currPlayer;
    struct SomeBigEndingStruct_sub3 *var_r16 = endingInfo.unk8->unkA804;
    float temp_f0;

    for (i = 0; i < 4; i++, var_r16++)
    {
        if (var_r16->unk0 != NULL)
        {
            switch (var_r16->unk4)
            {
            case 1:
                var_r16->unk4 = 2;
                var_r16->unk0->flags &= 0xFFFFFFDF;
                var_r16->unkA = 0;
                var_r16->unk30.x = 0;
                var_r16->unk30.y = 0;
                var_r16->unk30.z = 0;
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
                var_r16->unk0->flags &= 0xFFFFFFDF;
                var_r16->unkA = 3;
                var_r16->unk30.x = 0;
                var_r16->unk30.y = RAND_FLOAT();
                var_r16->unk30.z = 0;
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
                var_r16->unk30.y -= 0x30;
                var_r16->unk5A -= 0x40;
                break;
            case 8:
                var_r16->unk18.y += 0.009799992;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unk6--;
                if (var_r16->unk6 < 0)
                {
                    var_r16->unk4 = 0;
                    var_r16->unk0->flags |= 0x20;
                }
                memset(&effect, 0, sizeof(effect));
                effect.type = 0xA;
                effect.playerId = playerId;
                effect.pos = var_r16->unkC;
                spawn_effect(&effect);
                break;
            case 9:
                var_r16->unk4 = 0xA;
                var_r16->unk0->flags &= 0xFFFFFFDF;
                var_r16->unkA = 0x12;
                var_r16->unk30.x = 0;
                var_r16->unk30.y = i * 0x10000 / 3;
                var_r16->unk30.z = 0;
                mathutil_mtxA_from_rotate_y(var_r16->unk30.y + 0x8000);
                mathutil_mtxA_tf_vec_xyz(&var_r16->unkC, 0.0f, 0.0f, -5.0f);
                var_r16->unkC.y += 30.0f + (5.0f * RAND_FLOAT());
                var_r16->unk18.x = 0.0f;
                var_r16->unk18.y = -0.20000000298023224f;
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
                 && (0.5f + (var_r16->unkC.y - sp8.pos.y) < -15.0f * var_r16->unk18.y))
                    var_r16->unk18.y += 0.012000000104308128f;
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
                var_r16->unk18.y += -0.0080000003799796104f;
                var_r16->unkC.x += var_r16->unk18.x;
                var_r16->unkC.y += var_r16->unk18.y;
                var_r16->unkC.z += var_r16->unk18.z;
                if (sp8.pos.y > var_r16->unkC.y - 0.5f - 0.1f)
                {
                    if (var_r16->unk18.y < -0.10000000149011612f)
                        u_play_sound_0(0x69);
                    var_r16->unkC.y = 0.5f + sp8.pos.y - 0.1f;
                    if (var_r16->unk18.y < 0.0f)
                        var_r16->unk18.y *= -0.60000002384185791f;
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
                        effect.colorFactor = 0.20000000298023224f;
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
                var_r16->unk30.x = 0;
                var_r16->unk30.y = mathutil_atan2(var_r16->unkC.x, var_r16->unkC.z);
                var_r16->unk30.z = 0;
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
                var_r16->unk30.x = 0;
                var_r16->unk30.y = 0;
                var_r16->unk30.z = 0;
                temp_f0 = 1.0f / modeCtrl.submodeTimer;
                var_r16->unk18.x = -var_r16->unkC.x * temp_f0;
                var_r16->unk18.y = 0.0f;
                var_r16->unk18.z = -4.5f * temp_f0 * (1.0f + (0.1f * RAND_FLOAT()));
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
                var_r16->unk30.x = 0;
                var_r16->unk30.y = 0;
                var_r16->unk30.z = 0;
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
                    var_r16->unk44.x = 0.05f;
                    var_r16->unk44.y = 0.12999999523162842f;
                    var_r16->unk44.z = 0.12000000476837158f;
                    var_r16->unk50 = -0x58E3;
                    var_r16->unk52 = -0x1555;
                    var_r16->unk54 = 0x438E;
                    break;
                case 1:
                    var_r16->unk3C = decodedBgGma->modelEntries[0x17].model;
                    var_r16->unk40 = 0xF;
                    var_r16->unk44.x = 0.05f;
                    var_r16->unk44.y = 0.0f;
                    var_r16->unk44.z = -0.094000004231929779f;
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

static void func_800BC820(void)
{
    int i;
    struct SomeBigEndingStruct_sub3 *var_r28;

    if (endingInfo.unk8 == NULL)
        return;
    var_r28 = &endingInfo.unk8->unkA804[3];
    for (i = 3; i >= 0; i--, var_r28--)
    {
        if (var_r28->unk0 != NULL)
        {
            func_8008D29C(lbl_80206B80[i]);
            var_r28->unk0 = NULL;
        }
    }
}

struct Blah
{
    s8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u32 filler4;
    Vec unk8;
    float unk14;
};

static struct Blah lbl_801E2C44[] =
{
    {  0, 35, 37, 38, 0, { 0.0f, 0.0f, 0.0f }, 0.32000002f },
    {  7, 35, 37, 38, 0, { 0.0f, 0.0f, 0.0f }, 0.16000001f },
    { 12, 35, 37, 38, 0, { 0.0f, 0.0f, 0.0f }, 0.16000001f },
    { 22, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f }, 0.12800001f },
    { 27, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f }, 0.12800001f },
    { 21, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f }, 0.16000001f },
    { 26, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f }, 0.16000001f },
    { 15, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f }, 0.16000001f },
    { 10, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f }, 0.16000001f },
    { 24, 35, 37, 38, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000056624f },
    { 19, 35, 37, 38, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000056624f },
    {  8, 35, 37, 38, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000056624f },
    { 13, 35, 37, 38, 0, { 0.0f, 0.0f, 0.0f }, 0.0800000056624f },
    {  5, 35, 37, 38, 0, { 0.0f, 0.0f, 0.1f }, 0.28800002f },
    { -1,  0,  0,  0, 0, { 0.0f, 0.0f, 0.0f }, 0.0f },
};

static struct Blah lbl_801E2DAC[] =
{
    {  0, 24, 25, 25, 0, { 0.0f, 0.0f, 0.0f  }, 0.320000023f },
    {  7, 32, 34, 35, 0, { 0.0f, 0.0f, 0.0f  }, 0.160000011f },
    { 12, 32, 34, 35, 0, { 0.0f, 0.0f, 0.0f  }, 0.160000011f },
    { 22, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f  }, 0.128000006f },
    { 27, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f  }, 0.128000006f },
    { 21, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f  }, 0.160000011f },
    { 26, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f  }, 0.160000011f },
    { 15, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f  }, 0.160000011f },
    { 10, 23, 24, 25, 0, { 0.0f, 0.0f, 0.0f  }, 0.160000011f },
    { 24, 32, 34, 35, 0, { 0.0f, 0.0f, 0.0f  }, 0.080000006f },
    { 19, 32, 34, 35, 0, { 0.0f, 0.0f, 0.0f  }, 0.080000006f },
    {  8, 32, 34, 35, 0, { 0.0f, 0.0f, 0.0f  }, 0.080000006f },
    { 13, 32, 34, 35, 0, { 0.0f, 0.0f, 0.0f  }, 0.080000006f },
    {  5, 32, 34, 35, 0, { 0.0f, 0.0f, 0.04f }, 1.439999938f },
    { -1,  0,  0,  0, 0, { 0.0f, 0.0f, 0.0f  }, 0.000000000f },
};

static struct Blah lbl_801E2F14[] =
{
    {  0, 32, 27, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.320000023f },
    {  7, 22, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.160000011f },
    { 12, 22, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.160000011f },
    { 22, 23, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.128000006f },
    { 27, 23, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.128000006f },
    { 21, 23, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.160000011f },
    { 26, 23, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.160000011f },
    { 15, 23, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.128000006f },
    { 10, 23, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.128000006f },
    { 24, 22, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.096000001f },
    { 19, 22, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.096000001f },
    {  8, 22, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.096000001f },
    { 13, 22, 24, 25, 0, { 0.0000f, 0.0000f, 0.0f }, 0.096000001f },
    {  5, 22, 24, 25, 0, { 0.0000f, 0.0000f, 0.2f }, 0.240000010f },
    { -1,  0,  0,  0, 0, { 0.0000f, 0.0000f, 0.0f }, 0.000000000f },
};

static struct Blah lbl_801E307C[] =
{
    {  0, 33, 37, 38, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.480000019f },
    {  7, 37, 40, 41, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.240000010f },
    { 12, 37, 40, 41, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.240000010f },
    { 22, 25, 26, 28, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.192000002f },
    { 27, 25, 26, 28, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.192000002f },
    { 21, 25, 26, 28, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.240000010f },
    { 26, 25, 26, 28, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.240000010f },
    { 15, 25, 26, 28, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.256000012f },
    { 10, 25, 26, 28, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.256000012f },
    { 24, 37, 40, 41, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.128000006f },
    { 19, 37, 40, 41, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.128000006f },
    {  8, 37, 40, 41, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.128000006f },
    { 13, 37, 40, 41, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.128000006f },
    {  5, 37, 40, 41, 0, { 0.0000f, 0.0000f, 0.2000f }, 0.496000022f },
    { -1,  0,  0,  0, 0, { 0.0000f, 0.0000f, 0.0000f }, 0.000000000f },
};

static struct Blah *lbl_801E31E4[4] =
{
    lbl_801E2C44,
    lbl_801E2DAC,
    lbl_801E2F14,
    lbl_801E307C,
};

struct MyDrawNode
{
    struct OrdTblNode node;
    struct SomeBigEndingStruct_sub3 *unk8;
};

static void lbl_800BCD30(struct MyDrawNode *);

static void func_800BC8B8(void)
{
    Vec *new_var;
    f32 temp_f21 ;
    f32 temp_f0;
    f32 temp_f4;
    s32 var_r28;
    struct SomeBigEndingStruct_sub3 *var_r27;
    struct Ape *temp_r26;
    struct Blah *var_r25;
    struct GMAModel *temp_r21;
    struct OrdTblNode *temp_r21_2;
    struct MyDrawNode *temp_r3;
    Mtx sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;

    mathutil_mtxA_from_rotate_y(s_bgLightInfo.infLightRotY);
    mathutil_mtxA_rotate_x(s_bgLightInfo.infLightRotX);
    mathutil_mtxA_tf_vec_xyz(&sp2C, 0.0f, 0.0f, -1.0f);

    var_r27 = endingInfo.unk8->unkA804;
    for (var_r28 = 4; var_r28 > 0; var_r28--, var_r27++)
    {
        temp_f0 = temp_f21 + -1.0f;  // needed to match

        temp_r26 = var_r27->unk0;
        if (temp_r26 == NULL)
            continue;

        temp_r21 = var_r27->unk3C;
        if (temp_r21 != NULL)
        {
            mathutil_mtxA_from_quat(&temp_r26->unk60);
            mathutil_mtxA_to_mtx(sp38);
            mathutil_mtxA_from_mtxB_translate(&temp_r26->unk30);
            mathutil_mtxA_scale_s(temp_r26->modelScale);
            mathutil_mtxA_translate(&temp_r26->unk3C);
            mathutil_mtxA_mult_right(sp38);
            mathutil_mtxA_mult_right(temp_r26->unk0->joints[var_r27->unk40].transformMtx);
            mathutil_mtxA_translate(&var_r27->unk44);
            mathutil_mtxA_rotate_z(var_r27->unk54);
            mathutil_mtxA_rotate_y(var_r27->unk52);
            mathutil_mtxA_rotate_x(var_r27->unk50);
            u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
            avdisp_set_bound_sphere_scale(temp_r26->modelScale);
            avdisp_draw_model_culled_sort_translucent(temp_r21);
        }
        if (var_r27->unk56 >= 0)
        {
            mathutil_mtxA_from_mtxB();
            temp_r21_2 = ord_tbl_get_entry_for_pos(&var_r27->unkC);
            temp_r3 = ord_tbl_alloc_node(0xCU);
            temp_r3->node.drawFunc = (OrdTblDrawFunc)lbl_800BCD30;
            temp_r3->unk8 = var_r27;
            ord_tbl_insert_node(temp_r21_2, &temp_r3->node);
        }
        if (var_r27->unk36 & 1)
        {
            sp20 = var_r27->unkC;
            mathutil_mtxA_from_quat(&temp_r26->unk60);
            mathutil_mtxA_to_mtx(sp38);
            mathutil_mtxA_from_translate(&temp_r26->unk30);
            mathutil_mtxA_scale_s(temp_r26->modelScale);
            mathutil_mtxA_translate(&temp_r26->unk3C);
            new_var = &sp14;
            mathutil_mtxA_mult_right(sp38);
            avdisp_set_z_mode(1, GX_LEQUAL, 0);
            var_r25 = lbl_801E31E4[temp_r26->charaId];
            while (var_r25->unk0 != -1)
            {
                mathutil_mtxA_push();
                mathutil_mtxA_mult_right(temp_r26->unk0->joints[var_r25->unk0].transformMtx);
                mathutil_mtxA_translate(&var_r25->unk8);
                mathutil_mtxA_sq_from_identity();
                mathutil_mtxA_get_translate_alt(new_var);
                temp_f21 = sp14.y - (sp20.y - 0.4f);
                temp_f4 = sp20.y - 0.4f;
                if (temp_f21 > 0.6f)
                {
                    mathutil_mtxA_pop();
                }
                else
                {
                    temp_f0 = -(temp_f21 / sp2C.y);
                    sp14.x += (sp2C.x * temp_f0);
                    sp14.y = temp_f4;
                    sp14.z += (sp2C.z * temp_f0);
                    mathutil_mtxA_set_translate(&sp14);
                    mathutil_mtxA_mult_left(mathutilData->mtxB);
                    mathutil_mtxA_get_translate_alt(&sp14);
                    if (sp14.z < -0.01f)
                    {
                        temp_f21 = 1.6666666f * temp_f21;
                        temp_f0 = 0.003921569f * (1.0f - temp_f21);
                        sp8.x = var_r25->unk1 * temp_f0;
                        sp8.y = var_r25->unk2 * temp_f0;
                        sp8.z = var_r25->unk3 * temp_f0;
                        avdisp_set_post_mult_color(sp8.x, sp8.y, sp8.z, 1.0f);
                        temp_f21 = (var_r25->unk14 + (0.2f * temp_f21));
                        temp_f0 = (0.005f + sp14.z) / sp14.z;
                        sp14.x *= temp_f0;
                        sp14.y *= temp_f0;
                        sp14.z *= temp_f0;
                        mathutil_mtxA_set_translate(&sp14);
                        temp_f21 *= temp_f0;
                        mathutil_mtxA_scale_s(temp_f21);
                        mathutil_mtxA_rotate_x(0x4000);
                        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
                        avdisp_set_bound_sphere_scale(temp_f21);
                        avdisp_draw_model_culled_sort_all(commonGma->modelEntries[0x4E].model);
                    }
                    mathutil_mtxA_pop();
                }
                var_r25++;
            }
            u_reset_post_mult_color();
            avdisp_set_z_mode(1, GX_LEQUAL, 1);
        }
    }
}

static void lbl_800BCD30(struct MyDrawNode *node)
{
    struct SomeBigEndingStruct_sub3 *r30 = node->unk8;
    s16 *r29 = coloredBallPartModelIDs[r30->unk56];
    struct GMAModelEntry *r31 = commonGma->modelEntries;

    mathutil_mtxA_from_mtxB();
    load_light_group_uncached(0);
    mathutil_mtxA_translate_xyz(r30->unkC.x, r30->unkC.y + 0.1f, r30->unkC.z);
    mathutil_mtxA_rotate_y(r30->unk5A);
    mathutil_mtxA_rotate_x(r30->unk58);
    mathutil_mtxA_rotate_z(r30->unk5C);
    u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_none(r31[clearHemisphereInsideParts[0]].model);
    avdisp_draw_model_unculled_sort_none(r31[r29[0]].model);
    avdisp_draw_model_unculled_sort_none(r31[r29[6]].model);
    avdisp_set_z_mode(1, 3, 1);
    avdisp_draw_model_unculled_sort_none(r31[clearHemisphereOutsideParts[0]].model);
    avdisp_draw_model_unculled_sort_none(r31[r29[3]].model);
    avdisp_set_z_mode(1, 3, 1);
}

static int func_800BCE54(int arg0, int charaId)
{
    struct SomeBigEndingStruct_sub3 *temp_r31;
    struct Ape *ape;

    temp_r31 = &endingInfo.unk8->unkA804[arg0];
    if (temp_r31->unk0 != NULL)
    {
        func_8008D29C(lbl_80206B80[arg0]);
        temp_r31->unk0 = NULL;
    }
    if (charaId >= 0)
    {
        temp_r31->unk0 = (void *)func_800380A8(arg0, charaId, &lbl_800BCF6C);
        ape = temp_r31->unk0;
        if (ape == NULL)
            return 0;
        ape->unk0->unk3C = 1.0f;
        ape->flags |= 0x20;
        temp_r31->unk8 = 0xC;
        temp_r31->unkA = -1;
        mathutil_mtxA_from_identity();
        mathutil_mtxA_get_translate_alt2(&ape->unk30);
        mathutil_mtxA_to_quat(&ape->unk60);
    }
    return 1;
}

static void lbl_800BCF6C(struct Ape *arg0, int arg1)
{
    int var_r4;
    struct SomeBigEndingStruct_sub3 *var_r30;
    int i;
    int b = (arg1 == 3);
    Vec sp10;

    if (b)
    {
        u_ape_free(arg0);
        if (arg1 != 3)
            func_8008D240();
        return;
    }
    if (gamePauseStatus & 0xA)
        return;

    var_r30 = &endingInfo.unk8->unkA804[0];
    for (var_r4 = 4; var_r4 > 0; var_r4--, var_r30++)
    {
        if (var_r30->unk0 == arg0)
            break;
    }

    if (var_r4 == 0)
        return;
    switch (arg0->charaId)
    {
    case 3:
        switch (var_r30->unk8)
        {
        case 9:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 8;
            break;
        case 8:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 0xA;
            break;
        case 10:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 0xE;
            break;
        case 14:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 0xB;
            break;
        case 11:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 0xF;
            break;
        case 15:
            if (var_r30->unkA != -1 && arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
            {
                var_r30->unk8 = var_r30->unkA;
                var_r30->unkA = -1;
            }
            break;
        case 17:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 0x10;
            break;
        default:
            if (var_r30->unkA != -1)
            {
                var_r30->unk8 = var_r30->unkA;
                var_r30->unkA = -1;
            }
            break;
        }
        break;
    default:
        switch (var_r30->unk8)
        {
        case 14:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 0xF;
            break;
        case 15:
            if (arg0->unk0->unk38 == arg0->unk0->unk3A - 1)
                var_r30->unk8 = 0xE;
            if (var_r30->unkA != -1)
            {
                var_r30->unk8 = var_r30->unkA;
                var_r30->unkA = -1;
            }
            break;
        default:
            if (var_r30->unkA != -1)
            {
                var_r30->unk8 = var_r30->unkA;
                var_r30->unkA = -1;
            }
            break;
        }
    }

    if (var_r30->unk8 < 0x12)
        u_set_ape_anim(arg0, 7, var_r30->unk8, 0, 0.0f);
    else
    {
        int temp_r5_3 = var_r30->unk8 - 0x12;
        u_set_ape_anim(arg0, 1, temp_r5_3, temp_r5_3 + 1, 0.0f);
    }
    u_do_ape_anim(arg0);
    switch (var_r30->unk70)
    {
    case 2:
        func_8008C090(arg0, &var_r30->unk74);
        break;
    case 1:
        mathutil_mtxA_from_mtx(cameraInfo[modeCtrl.currPlayer].unk144);
        mathutil_mtxA_rigid_inv_tf_tl(&sp10);
        func_8008C090(arg0, &sp10);
        break;
    }

    mathutil_mtxA_from_translate(&var_r30->unkC);
    mathutil_mtxA_rotate_z(var_r30->unk30.z);
    mathutil_mtxA_rotate_y(var_r30->unk30.y - 0x4000);
    mathutil_mtxA_rotate_x(var_r30->unk30.x);
    mathutil_mtxA_get_translate_alt2(&arg0->unk30);
    mathutil_mtxA_to_quat(&arg0->unk60);

    switch (arg0->charaId)
    {
    case 0:
        switch (arg0->unk9C)
        {
        case 0xE:
            if (endingInfo.unk4 & 0x10)
            {
                if (arg0->unk0->unk38 == 0x16)
                    SoundReq(0xB3U);
                else if (arg0->unk0->unk38 == 0x44)
                    SoundReq(0xE8U);
            }
            break;
        }
        break;
    case 1:
        switch (arg0->unk9C)
        {
        case 15:
            if (endingInfo.unk4 & 0x10)
            {
                if (arg0->unk0->unk38 == 0xC)
                    SoundReq(0x105U);
                else if (arg0->unk0->unk38 == 0x50 || arg0->unk0->unk38 == 0x66)
                    SoundReq(0x12FU);
                else if (arg0->unk0->unk38 == 0x76)
                    u_play_sound_0(0x1FF);
            }
            break;
        case 14:
            if ((endingInfo.unk4 & 0x10) && arg0->unk0->unk38 == 0x12)
                SoundReq(0xFCU);
            break;
        }
        break;
    case 2:
        switch (arg0->unk9C)
        {
        case 0xE:
            if ((endingInfo.unk4 & 0x10) && arg0->unk0->unk38 == 0x24)
                SoundReq(0x16BU);
            break;
        }
        break;
    }
}

static void func_800BD470(void)
{
    int i;
    struct SomeBigEndingStruct_sub4 *ptr;

    ptr = endingInfo.unk8->unk4;
    for (i = 0; i < 512; i++, ptr++)
        ptr->unk0 = 0;
}

static void func_800BD4B8(void)
{
    Vec spFC;
    Vec spF0;
    Vec spE4;
    struct Effect sp38;
    float temp_f0_5;
    float temp_f1;
    float temp_f27;
    float temp_f26;
    float alt_f26;
    float alt_f27;
    int var_r30;
    struct SomeBigEndingStruct_sub4 *var_r29;
    struct Camera *temp_r28;
    struct SomeBigEndingStruct_sub4_sub2 *temp_r27;
    struct
    {
        s16 unk0;
        u8 filler2[6];
        Vec sp10;
        u8 filler1C[4];
        Vec sp20;
        Vec sp2C;
    } sp8;

    func_800BEEE8();
    temp_r28 = &cameraInfo[modeCtrl.currPlayer];
    mathutil_mtxA_from_mtx(temp_r28->unk144);
    mathutil_mtxA_rigid_inv_tf_vec_xyz(&spF0, 0.0f, 0.0f, -1.0f);
    mathutil_mtxA_rigid_inv_tf_tl(&spE4);

    var_r29 = endingInfo.unk8->unk4;
    for (var_r30 = 512; var_r30 > 0; var_r30--, var_r29++)
    {
        if (var_r29->unk0 != 0)
        {
            temp_r27 = var_r29->unk50;
            switch (var_r29->unk2)
            {
            case 2:
                alt_f26 = var_r29->unkC.y;
                mathutil_mtxA_from_rotate_y((unpausedFrameCounter << 0xA) + var_r30);
                alt_f27 = 0.25f * alt_f26 * endingInfo.unk8->unk0;
                mathutil_mtxA_tf_vec_xyz(&spFC, alt_f27, 0.0f, 0.0f);
                mathutil_mtxA_from_translate(&spFC);
                alt_f27 = 1024.f * alt_f26 * endingInfo.unk8->unk0;
                mathutil_mtxA_rotate_x(alt_f27 * mathutil_sin((unpausedFrameCounter << 0xB) + var_r30));
                mathutil_mtxA_rotate_z(alt_f27 * mathutil_sin((unpausedFrameCounter << 0xB) + var_r30 + 0x4000));
                mathutil_mtxA_tf_point(&var_r29->unk4C->unk4, &spFC);
                if (spFC.y < var_r29->unk4C->unk4.y)
                    spFC.y = var_r29->unk4C->unk4.y;
                var_r29->unk18.x = 0.1f * (spFC.x - var_r29->unkC.x);
                var_r29->unk18.y = 0.1f * (spFC.y - var_r29->unkC.y);
                var_r29->unk18.z = 0.05f * (spFC.z - var_r29->unkC.z);
                var_r29->unk36 = 0;
                var_r29->unk38 = 0;
                var_r29->unk3A = 0;
                if (var_r29->unk4 != 0)
                {
                    var_r29->unk4--;
                    if (var_r29->unk4 == 0)
                    {
                        var_r29->unk2 = 3;
                        var_r29->unk8 &= 0xFFFFFFFC;
                        var_r29->unk8 |= 0x34;
                        if (var_r30 & 1)
                            var_r29->unk8 |= 8;
                        mathutil_vec_set_len(&var_r29->unkC, &spFC, 0.001f + (0.05f * RAND_FLOAT()));
                        var_r29->unk18.x += spFC.x;
                        var_r29->unk18.y += 0.05f + (4.0f * spFC.y);
                        var_r29->unk18.z += spFC.z;
                        var_r29->unk36 += 2048.0f * (RAND_FLOAT() - 0.5f);
                        var_r29->unk38 += 2048.0f * (RAND_FLOAT() - 0.5f);
                        var_r29->unk3A += 2048.0f * (RAND_FLOAT() - 0.5f);
                    }
                }
                break;
            case 4:
                var_r29->unk2 = 5;
                mathutil_mtxA_from_rotate_y(rand() & 0x7FFF);
                mathutil_mtxA_rotate_x(rand() & 0x7FFF);
                mathutil_mtxA_tf_vec_xyz(&spFC, 0.0f, 0.0f, 30.0f * RAND_FLOAT());
                spFC.y += 60.f;
                temp_f1 = mathutil_vec_dot_prod(&spFC, &spF0);
                if (temp_f1 < 0.0f)
                {
                    spFC.x -= temp_f1 * spF0.x;
                    spFC.y -= temp_f1 * spF0.y;
                    spFC.z -= temp_f1 * spF0.z;
                    if (mathutil_vec_len(&spFC) > 80.0f)
                        mathutil_vec_set_len(&spFC, &spFC, 80.0f * RAND_FLOAT());
                }
                var_r29->unkC.x = spE4.x + spFC.x;
                var_r29->unkC.y = spE4.y + spFC.y;
                var_r29->unkC.z = spE4.z + spFC.z;
                var_r29->unk18.x = 0.2f * (RAND_FLOAT() - 0.5f);
                var_r29->unk18.y = -0.2f * RAND_FLOAT();
                var_r29->unk18.z = 0.2f * (RAND_FLOAT() - 0.5f);
                var_r29->unk30.x = rand() & 0x7FFF;
                var_r29->unk30.y = rand() & 0x7FFF;
                var_r29->unk30.z = rand() & 0x7FFF;
                var_r29->unk36 = 2048.0f * (RAND_FLOAT() - 0.5f);
                var_r29->unk38 = 2048.0f * (RAND_FLOAT() - 0.5f);
                var_r29->unk3A = 2048.0f * (RAND_FLOAT() - 0.5f);
                var_r29->unk24.x = 0.3f * (1.0f + RAND_FLOAT());
                var_r29->unk24.y = var_r29->unk24.x;
                var_r29->unk24.z = var_r29->unk24.x;
                var_r29->unk48 = 0.0f;
                /* fall through */
            case 5:
                mathutil_mtxA_from_mtx(temp_r28->unk144);
                mathutil_mtxA_tf_point(&var_r29->unkC, &spFC);
                if (spFC.z > 5.f)
                {
                    var_r29->unk2 = 4;
                    var_r29->unk48 = 0.0f;
                }
                else if (mathutil_vec_len(&spFC) < 80.0f)
                {
                    var_r29->unk48 += 0.1f * (1.1f - var_r29->unk48);
                }
                else
                {
                    var_r29->unk48 += 0.15f * (-0.1f - var_r29->unk48);
                    if (var_r29->unk48 < 0.0f)
                    {
                        var_r29->unk2 = 4;
                        var_r29->unk48 = 0.0f;
                    }
                }
                break;
            case 6:
                var_r29->unk2 = 7;
                var_r29->unk8 |= 3;
                var_r29->unk48 = 0.0f;
                var_r29->unk24.x = 2.0f * (1.0f + (0.5f * RAND_FLOAT()));
                var_r29->unk24.y = var_r29->unk24.x;
                var_r29->unk24.z = var_r29->unk24.x;
                var_r29->unk30.x = rand() & 0x7FFF;
                var_r29->unk30.y = rand() & 0x7FFF;
                var_r29->unk30.z = rand() & 0x7FFF;
                var_r29->unk36 += 4096.0f * (RAND_FLOAT() - 0.5f);
                var_r29->unk38 += 4096.0f * (RAND_FLOAT() - 0.5f);
                var_r29->unk3A += 4096.0f * (RAND_FLOAT() - 0.5f);
                alt_f27 = var_r29->unk4;
                var_r29->unk18.x = 0.0f;
                var_r29->unk18.y = -0.2f * RAND_FLOAT();
                var_r29->unk18.z = 0.0f;
                var_r29->unk3C = RAND_FLOAT() - 0.5f;
                var_r29->unk40 = -0.5f * (alt_f27 * ((2.0f * var_r29->unk18.y) + (-0.008f * alt_f27)));
                var_r29->unk44 = RAND_FLOAT() - 0.5f;
                /* fall through */
            case 7:
                var_r29->unk36 = 0.99f * var_r29->unk36;
                var_r29->unk38 = 0.99f * var_r29->unk38;
                var_r29->unk3A = 0.99f * var_r29->unk3A;
                var_r29->unk30.x += var_r29->unk36;
                var_r29->unk30.y += var_r29->unk38;
                var_r29->unk30.z += var_r29->unk3A;
                var_r29->unk18.y += -0.008f;
                var_r29->unk40 += var_r29->unk18.y;
                if (var_r29->unk40 <= 0.0f)
                {
                    spFC.x = var_r29->unk18.x;
                    spFC.y = var_r29->unk18.y;
                    spFC.z = var_r29->unk18.z;
                    func_8001898C(modeCtrl.currPlayer, 0xF, &spFC);
                    var_r29->unk40 = 0.0f;
                    if (var_r29->unk18.y < 0.0f)
                        var_r29->unk18.y *= -0.125f;
                    var_r29->unk36 += 4096.0f * (RAND_FLOAT() - 0.5f);
                    var_r29->unk38 += 4096.0f * (RAND_FLOAT() - 0.5f);
                    var_r29->unk3A += 4096.0f * (RAND_FLOAT() - 0.5f);
                }
                var_r29->unkC.x = temp_r28->eye.x + var_r29->unk3C;
                var_r29->unkC.y = var_r29->unk40 + (temp_r28->eye.y + (3.2000000476837158f * var_r29->unk24.x));
                var_r29->unkC.z = temp_r28->eye.z + var_r29->unk44;
                var_r29->unk48 += 0.1f * (1.1f - var_r29->unk48);
                break;
            case 8:
                var_r29->unk2 = 9;
                var_r29->unk8 = 0x43;
                var_r29->unk48 = 0.0f;
                var_r29->unk24.x = 0.4f;
                var_r29->unk24.y = var_r29->unk24.x;
                var_r29->unk24.z = var_r29->unk24.x;
                var_r29->unk30.x = rand() & 0x7FFF;
                var_r29->unk30.y = rand() & 0x7FFF;
                var_r29->unk30.z = rand() & 0x7FFF;
                var_r29->unk36 += 256.0f * (RAND_FLOAT() - 0.5f);
                var_r29->unk38 += 16.0f * (RAND_FLOAT() - 0.5f);
                var_r29->unk3A += 4096.0f * (0.5f + RAND_FLOAT());
                var_r29->unkC = endingInfo.unk8->unkAA54.unk2C;
                /* fall through */
            case 9:
                if (var_r29->unk4 != 0)
                    var_r29->unk4--;
                endingInfo.unk8->unkAA54.unk2C = var_r29->unkC;
                if (var_r29->unk4 < 0x3C)
                {
                    temp_f0_5 = 1.0f / (var_r29->unk4 + 1);
                    var_r29->unk18.x = (0.15016f - var_r29->unkC.x) * temp_f0_5;
                    var_r29->unk18.y = (20.07f - var_r29->unkC.y) * temp_f0_5;
                    var_r29->unk18.z = (-60.0f - var_r29->unkC.z) * temp_f0_5;
                    var_r29->unkC.x += var_r29->unk18.x;
                    var_r29->unkC.y += var_r29->unk18.y;
                    var_r29->unkC.z += var_r29->unk18.z;
                    if (var_r29->unk4 == 0)
                    {
                        var_r29->unk36 = 0.92f * var_r29->unk36;
                        var_r29->unk38 = 0.92f * var_r29->unk38;
                        var_r29->unk3A = 0.92f * var_r29->unk3A;
                        var_r29->unk36 += -0.1f * var_r29->unk30.x;
                        var_r29->unk38 += -0.1f * var_r29->unk30.y;
                        var_r29->unk3A += -0.1f * var_r29->unk30.z;
                    }
                    var_r29->unk30.x += var_r29->unk36;
                    var_r29->unk30.y += var_r29->unk38;
                    var_r29->unk30.z += var_r29->unk3A;
                }
                else
                {
                    var_r29->unk18.x = 0.025f * (0.15016f - var_r29->unkC.x);
                    var_r29->unk18.y = 0.025f * (20.07f - var_r29->unkC.y);
                    var_r29->unk18.z = 0.0f;
                    var_r29->unkC.x += var_r29->unk18.x;
                    var_r29->unkC.y += var_r29->unk18.y;
                    var_r29->unkC.z += var_r29->unk18.z;
                    var_r29->unk30.x += var_r29->unk36;
                    var_r29->unk30.y += var_r29->unk38;
                    var_r29->unk30.z += var_r29->unk3A;
                    memset(&sp38, 0, sizeof(sp38));
                    sp38.type = 0xA;
                    sp38.playerId = modeCtrl.currPlayer;
                    sp38.pos = var_r29->unkC;
                    spawn_effect(&sp38);
                }
                var_r29->unk48 += (0.1f * (1.1f - var_r29->unk48));
                break;
            case 10:
                {
                    struct SomeBigEndingStruct *temp = endingInfo.unk8;
                    var_r29->unkC.x = 0.15016f + temp->unkA804[0].unkC.x;
                    var_r29->unkC.y = 0.07f + temp->unkA804[0].unkC.y;
                    var_r29->unkC.z = temp->unkA804[0].unkC.z;
                    var_r29->unk30 = temp->unkA804[0].unk30;
                }
                break;
            }
            if (!(var_r29->unk8 & 2))
            {
                var_r29->unk18.x *= 0.99f;
                var_r29->unk18.y *= 0.99f;
                var_r29->unk18.z *= 0.99f;
                var_r29->unk36 = 0.984375f * var_r29->unk36;
                var_r29->unk38 = 0.984375f * var_r29->unk38;
                var_r29->unk3A = 0.984375f * var_r29->unk3A;
            }
            if (!(var_r29->unk8 & 1))
            {
                var_r29->unk18.y += -0.008f;
                var_r29->unkC.x += var_r29->unk18.x;
                var_r29->unkC.y += var_r29->unk18.y;
                var_r29->unkC.z += var_r29->unk18.z;
                var_r29->unk30.x += var_r29->unk36;
                var_r29->unk30.y += var_r29->unk38;
                var_r29->unk30.z += var_r29->unk3A;
            }
            if (var_r29->unk8 & 0x20)
            {
                temp_f26 = temp_r27->unk14 * var_r29->unk24.x;
                temp_f27 = 2.0f + temp_f26;
                if (mathutil_vec_sq_len(&var_r29->unkC) < temp_f27 * temp_f27)
                {
                    mathutil_vec_set_len(&var_r29->unkC, &spFC, temp_f27);
                    spFC.x -= var_r29->unkC.x;
                    spFC.y -= var_r29->unkC.y;
                    spFC.z -= var_r29->unkC.z;
                    var_r29->unkC.x += 0.025f * spFC.x;
                    var_r29->unkC.y += 0.025f * spFC.y;
                    var_r29->unkC.z += 0.025f * spFC.z;
                    var_r29->unk18.x += 0.01f * spFC.x;
                    var_r29->unk18.y += 0.01f * spFC.y;
                    var_r29->unk18.z += 0.01f * spFC.z;
                }

                spFC.x = var_r29->unkC.x;
                spFC.y = var_r29->unkC.y - 1.0f;
                spFC.z = var_r29->unkC.z - 3.0f;
                temp_f27 = 1.5f + temp_f26;
                if (mathutil_vec_sq_len(&spFC) < temp_f27 * temp_f27)
                {
                    mathutil_vec_set_len(&spFC, &spFC, temp_f27);
                    var_r29->unkC.x += 0.05f * spFC.x;
                    var_r29->unkC.y += 0.05f * spFC.y;
                    var_r29->unkC.z += 0.05f * spFC.z;
                    var_r29->unk18.x += 0.01f * spFC.x;
                    var_r29->unk18.y += 0.01f * spFC.y;
                    var_r29->unk18.z += 0.01f * spFC.z;
                }
                temp_f27 = 1.0f + temp_f26;
                {
                    struct SomeBigEndingStruct *temp = endingInfo.unk8;
                    spFC.x = var_r29->unkC.x - temp->unkA804[3].unkC.x;
                    spFC.y = var_r29->unkC.y - temp->unkA804[3].unkC.y;
                    spFC.z = var_r29->unkC.z - temp->unkA804[3].unkC.z;
                }
                if (mathutil_vec_sq_len(&spFC) < (temp_f27 * temp_f27))
                {
                    mathutil_vec_set_len(&spFC, &spFC, temp_f27);
                    var_r29->unkC.x += 0.05f * spFC.x;
                    var_r29->unkC.y += 0.05f * spFC.y;
                    var_r29->unkC.z += 0.05f * spFC.z;
                    var_r29->unk18.x += 0.01f * spFC.x;
                    var_r29->unk18.y += 0.01f * spFC.y;
                    var_r29->unk18.z += 0.01f * spFC.z;
                }
            }
            if (var_r29->unk8 & 0x10)
            {
                float f;
                float g = temp_r27->unk14;
                sp8.sp20.x = 0.0f;
                sp8.sp20.y = -0.4f;
                sp8.sp20.z = 0.0f;
                sp8.sp2C.x = 0.0f;
                sp8.sp10.x = 0.0f;
                sp8.sp2C.y = 1.0f;
                sp8.sp10.y = 0.0f;
                sp8.sp2C.z = 0.0f;
                sp8.sp10.z = 0.0f;
                mathutil_mtxA_from_rotate_y(var_r29->unk30.y);
                mathutil_mtxA_rotate_x(var_r29->unk30.x);
                mathutil_mtxA_rotate_z(var_r29->unk30.z);
                mathutil_mtxA_tf_vec_xyz(&spFC, 0.70710677f, 0.70710677f, 0.0f);
                f = g * var_r29->unk24.x * (0.30000001192092896 + 0.69999998807907104 * fabs(spFC.y));
                if (var_r29->unkC.y < sp8.sp20.y + f)
                {
                    var_r29->unkC.y = sp8.sp20.y + f;
                    temp_f26 = mathutil_vec_dot_prod(&sp8.sp2C, &var_r29->unk18);
                    if (temp_f26 < 0.0f)
                    {
                        var_r29->unk36 = var_r29->unk36 * 0.96875f;
                        var_r29->unk38 = var_r29->unk38 * 0.96875f;
                        var_r29->unk3A = var_r29->unk3A * 0.96875f;
                        mathutil_mtxA_rigid_inv_tf_vec(&sp8.sp2C, &spFC);
                        sp8.unk0 = (256.0 + (256.0 * fabs(temp_f26))) * mathutil_cos(((s16)(((var_r30 * 0x800) & 0x1FFF) - 0xFFF) + mathutil_atan2(-spFC.z, spFC.y)));
                        var_r29->unk36 += sp8.unk0;
                        if (temp_f26 < -0.1f)
                        {
                            temp_f27 = 16384.0f * (0.1f + temp_f26);
                            var_r29->unk36 += temp_f27 * (RAND_FLOAT() - 0.5f);
                            var_r29->unk38 += temp_f27 * (RAND_FLOAT() - 0.5f);
                            var_r29->unk3A += temp_f27 * (RAND_FLOAT() - 0.5f);
                        }
                        spFC = var_r29->unk18;
                        temp_f26 *= -1.0f;
                        spFC.x += temp_f26 * sp8.sp2C.x;
                        spFC.y += temp_f26 * sp8.sp2C.y;
                        spFC.z += temp_f26 * sp8.sp2C.z;
                        spFC.x *= 0.9f;
                        spFC.y *= 0.9f;
                        spFC.z *= 0.9f;
                        temp_f26 *= 0.6f;
                        spFC.x += temp_f26 * sp8.sp2C.x;
                        spFC.y += temp_f26 * sp8.sp2C.y;
                        spFC.z += temp_f26 * sp8.sp2C.z;
                        var_r29->unk18 = spFC;
                    }
                }
            }
        }
    }
}

static void func_800BEEE8(void)
{
    float unused_f31;
    float unused_f30;

    int i;
    int j;
    struct SomeBigEndingStruct_sub4 *var_r29;
    struct SomeBigEndingStruct_sub4 *var_r28;

    // useless, but needed to match
    unused_f30 = 0.5f;
    unused_f31 = 0.25f;

    var_r29 = endingInfo.unk8->unk4;
    for (i = 512; i > 0; i--, var_r29++)
    {
        if (var_r29->unk0 != 0 && (var_r29->unk8 & 8))
        {
            float temp_f29 = 0.5f * (var_r29->unk50->unk14 * var_r29->unk24.x);
            Vec sp38 = var_r29->unkC;

            var_r28 = var_r29 + 1;
            for (j = i - 1; j > 0; j--, var_r28++)
            {
                if (var_r28->unk0 != 0 && (var_r28->unk8 & 8))
                {
                    float temp_f28 = 0.5f * (var_r28->unk50->unk14 * var_r28->unk24.x);
                    float temp_f27 = temp_f29 + temp_f28;
                    Vec sp2C = var_r28->unkC;
                    Vec sp20;

                    sp20.x = sp38.x - sp2C.x;
                    if (sp20.x < -temp_f27 || sp20.x > temp_f27)
                        continue;
                    sp20.z = sp38.z - sp2C.z;
                    if (sp20.z < -temp_f27 || sp20.z > temp_f27)
                        continue;
                    sp20.y = sp38.y - sp2C.y;
                    if (sp20.y < -temp_f27 || sp20.y > temp_f27)
                        continue;
                    if (mathutil_vec_distance(&sp38, &sp2C) < temp_f27)
                    {
                        float temp_f0_4 = temp_f28 / temp_f27;
                        Vec sp14;
                        Vec sp8;

                        sp14.x = sp2C.x + (sp20.x * temp_f0_4);
                        sp14.y = sp2C.y + (sp20.y * temp_f0_4);
                        sp14.z = sp2C.z + (sp20.z * temp_f0_4);
                        mathutil_vec_normalize_len(&sp20);
                        temp_f0_4 = 0.25f / temp_f29;
                        sp8.x = (sp14.x + (temp_f29 * sp20.x)) - sp38.x;
                        sp8.y = (sp14.y + (temp_f29 * sp20.y)) - sp38.y;
                        sp8.z = (sp14.z + (temp_f29 * sp20.z)) - sp38.z;
                        sp38.x += 0.5f * sp8.x;
                        sp38.y += 0.5f * sp8.y;
                        sp38.z += 0.5f * sp8.z;
                        var_r29->unk18.x += (sp8.x * temp_f0_4);
                        var_r29->unk18.y += (sp8.y * temp_f0_4);
                        var_r29->unk18.z += (sp8.z * temp_f0_4);
                        temp_f0_4 = 0.25f / temp_f28;
                        sp8.x = (sp14.x - (temp_f28 * sp20.x)) - sp2C.x;
                        sp8.y = (sp14.y - (temp_f28 * sp20.y)) - sp2C.y;
                        sp8.z = (sp14.z - (temp_f28 * sp20.z)) - sp2C.z;
                        sp2C.x += 0.5f * sp8.x;
                        sp2C.y += 0.5f * sp8.y;
                        sp2C.z += 0.5f * sp8.z;
                        var_r28->unk18.x += (sp8.x * temp_f0_4);
                        var_r28->unk18.y += (sp8.y * temp_f0_4);
                        var_r28->unk18.z += (sp8.z * temp_f0_4);
                        var_r28->unkC = sp2C;
                    }
                }
            }
            var_r29->unkC = sp38;
        }
    }
}

static void func_800BF2D0(void) {}

static void func_800BF2D4(void)
{
    Vec sp14;
    Point3d sp8;
    float opacity;
    float scale;
    int isNaomiModel;
    int i;
    float radius;
    struct SomeBigEndingStruct_sub4 *var_r30;
    struct SomeBigEndingStruct_sub4_sub2 *model;

    var_r30 = endingInfo.unk8->unk4;
    for (i = 512; i > 0; i--, var_r30++)
    {
        if (var_r30->unk0 != 0)
        {
            model = var_r30->unk50;
            opacity = var_r30->unk48;
            if (opacity <= 0.0f)
                continue;

            sp14 = var_r30->unk24;
            mathutil_mtxA_from_mtxB_translate(&var_r30->unkC);
            mathutil_mtxA_rotate_y(var_r30->unk30.y);
            mathutil_mtxA_rotate_x(var_r30->unk30.x);
            mathutil_mtxA_rotate_z(var_r30->unk30.z);
            mathutil_mtxA_scale(&sp14);
            scale = MAX(MAX(sp14.x, sp14.y), sp14.z);
            isNaomiModel = var_r30->unk8 & 0x40;
            if (isNaomiModel)
            {
                radius = model->unk14;
                sp8 = model->unk8;
            }
            else
            {
                radius = model->unk14;
                sp8 = model->unk8;
            }
            if (test_scaled_sphere_in_frustum(&sp8, radius, scale))
            {
                if (var_r30->unk8 & 4)
                {
                    float temp_f0_2 = -(model->unk14 * scale) - mathutilData->mtxA[2][3];
                    if ((temp_f0_2 < 0.0f))
                        continue;
                    if (temp_f0_2 < 1.0f)
                        opacity *= temp_f0_2;
                }
                if (!isNaomiModel)
                {
                    u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
                    if (opacity < 1.0f)
                    {
                        avdisp_set_alpha(opacity);
                        avdisp_draw_model_unculled_sort_all((struct GMAModel *)model);
                    }
                    else
                        avdisp_draw_model_unculled_sort_translucent((struct GMAModel *)model);
                }
                else
                {
                    nl2ngc_set_scale(scale);
                    if (opacity < 1.0f)
                        nl2ngc_draw_model_alpha_sort_all((struct NlModel *)model, opacity);
                    else
                        nl2ngc_draw_model_sort_translucent((struct NlModel *)model);
                }
            }
        }
    }
}

static void func_800BF4F4(void)
{
    int var_r0;
    int i;
    int j;
    struct Stage_Child90 *var_r29;
    struct SomeBigEndingStruct_sub4 *var_r28;

    endingInfo.unk4 |= 1;
    var_r29 = decodedStageLzPtr->unk90;
    j = decodedStageLzPtr->unk8C;
    var_r28 = endingInfo.unk8->unk4;

    for (i = 512; i > 0 && j > 0; i--, var_r28++)
    {
        if (var_r28->unk0 == 0)
        {
            var_r28->unk0 = 1;
            var_r28->unk2 = 1;
            var_r28->unkC = var_r29->unk4;
            var_r28->unk18.x = 0.0f;
            var_r28->unk18.y = 0.0f;
            var_r28->unk18.z = 0.0f;
            var_r28->unk30.x = 0;
            var_r28->unk30.y = 0;
            var_r28->unk30.z = 0;
            mathutil_mtxA_from_rotate_z(var_r29->unk14);
            mathutil_mtxA_rotate_y(var_r29->unk12);
            mathutil_mtxA_rotate_x(var_r29->unk10);
            mathutil_mtxA_to_euler((S16Vec *) &var_r28->unk30);
            var_r28->unk24 = var_r29->unk18;
            var_r28->unk48 = 1.0f;
            var_r28->unk4C = (struct SomeBigEndingStruct_sub4_sub *) var_r29;
            var_r28->unk8 = 3;
            if (!(var_r29->unk0 & 1))
                var_r0 = 0;
            else
                var_r0 = 5;
            var_r28->unk50 = (struct SomeBigEndingStruct_sub4_sub2 *) decodedBgGma->modelEntries[var_r0].model;
            var_r29++;
            j--;
        }
    }
}

static void func_800BF658(void)
{
    int i;
    struct SomeBigEndingStruct_sub4 *ptr;

    ptr = endingInfo.unk8->unk4;
    for (i = 512; i > 0; i--, ptr++)
    {
        if (ptr->unk0 != 0 && ptr->unk2 == 1)
        {
            ptr->unk2 = 2;
            ptr->unk4 = 0;
            ptr->unk8 &= 0xFFFFFFFE;
            ptr->unk8 |= 2;
        }
    }
}

static void func_800BF774(void)
{
    Vec sp8;
    float temp_f25;
    int i;
    struct SomeBigEndingStruct_sub4 *var_r29;

    mathutil_mtxA_from_rotate_y((rand() & 0x3FF) - 0x200);
    mathutil_mtxA_rotate_z((rand() & 0x7FFF) - 0x4000);
    mathutil_mtxA_tf_vec_xyz(&sp8, 0.0f, -1.0f, 0.0f);
    var_r29 = endingInfo.unk8->unk4;
    for (i = 512; i > 0; i--, var_r29++)
    {
        if (var_r29->unk0 != 0 && var_r29->unk2 == 2 && var_r29->unk4 <= 0)
        {
            temp_f25 = 1.0f + mathutil_vec_dot_normalized_safe(&var_r29->unkC, &sp8);
            var_r29->unk4 = 1.0f + (60.0f * temp_f25 * (0.9f + (0.2f * RAND_FLOAT())));
        }
    }
}

static void func_800BF8C4(int arg0)
{
    int var_r0;
    int i;
    struct SomeBigEndingStruct_sub4 *var_r6;

    var_r6 = endingInfo.unk8->unk4;
    for (i = 512; i > 0 && arg0 > 0; i--, var_r6++)
    {
        if (var_r6->unk0 == 0)
        {
            var_r6->unk0 = 1;
            var_r6->unk2 = 4;
            var_r6->unk8 = 0x14;
            if (i & 1)
                var_r0 = 0;
            else
                var_r0 = 5;
            arg0 -= 1;
            var_r6->unk50 = (struct SomeBigEndingStruct_sub4_sub2 *) decodedBgGma->modelEntries[var_r0].model;
        }
    }
}

static void func_800BF948(int arg0, int arg1)
{
    int var_r0;
    int i;
    struct SomeBigEndingStruct_sub4 *var_r6;

    var_r6 = endingInfo.unk8->unk4;
    for (i = 512; i > 0 && arg0 > 0; i--, var_r6++)
    {
        if (var_r6->unk0 == 0)
        {
            var_r6->unk0 = 1;
            var_r6->unk2 = 6;
            var_r6->unk4 = arg1;
            if (i & 1)
                var_r0 = 0;
            else
                var_r0 = 5;
            arg0 -= 1;
            var_r6->unk50 = (struct SomeBigEndingStruct_sub4_sub2 *) decodedBgGma->modelEntries[var_r0].model;
        }
    }
}

static void func_800BF9CC(int arg0)
{
    int i;
    struct SomeBigEndingStruct_sub4 *var_r6;

    var_r6 = endingInfo.unk8->unk4;
    for (i = 512; i > 0; i--, var_r6++)
    {
        if (var_r6->unk0 == 0)
        {
            var_r6->unk0 = 1;
            var_r6->unk2 = 8;
            var_r6->unk4 = arg0;
            var_r6->unk48 = 0.0f;
            var_r6->unk50 = (void *)NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_OBJ_BANANA_ENDING);
            return;
        }
    }
}

static void func_800BFA2C(void)
{
    int i;
    struct SomeBigEndingStruct_sub4 *var_r6;

    var_r6 = endingInfo.unk8->unk4;
    for (i = 512; i > 0; i--, var_r6++)
    {
        if ((var_r6->unk2 == 8 || var_r6->unk2 == 9) && var_r6->unk0 != 0)
        {
            var_r6->unk2 = 10;
            var_r6->unk48 = 1.0f;
            return;
        }
    }
}

static void func_800BFC2C(void)
{
    GXTexObj texObj;
    GXColor kcolor;
    Point3d cameraPos;
    Vec upVec;
    Vec cameraTarget;
    Mtx sp14;
    struct Ape *ape;
    struct SomeBigEndingStruct_sub3 *temp_r29;
    struct SomeBigEndingStruct *temp;
    float x1, y1, x2, y2;

    temp = endingInfo.unk8;
    if (temp == NULL)
        return;
    if (temp->unkAAA8 <= 0.0f)
        return;
    temp_r29 = &temp->unkA804[0];
    ape = temp_r29->unk0;
    if (ape == NULL)
        return;

    GXSetTexCopySrc(0, 0, 640, 448);
    GXSetTexCopyDst(640, 448, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(lbl_802F1B40, 0U);
    GXInitTexObj(&texObj, lbl_802F1B40, 640, 448, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    mathutil_mtxA_from_quat(&ape->unk60);
    mathutil_mtxA_to_mtx(sp14);
    mathutil_mtxA_from_translate(&ape->unk30);
    mathutil_mtxA_scale_s(ape->modelScale);
    mathutil_mtxA_translate(&ape->unk3C);
    mathutil_mtxA_mult_right(sp14);
    mathutil_mtxA_mult_right(ape->unk0->joints[5].transformMtx);
    mathutil_mtxA_get_translate_alt(&cameraTarget);
    mathutil_mtxA_tf_point_xyz(&cameraTarget, 0.0f, 0.0f, 0.1f);
    mathutil_mtxA_tf_point_xyz(&cameraPos, temp->unkAAAC, 0.0f, 0.2f);
    upVec.x = 0.0f;
    upVec.y = 1.0f;
    upVec.z = 0.0f;
    MTXLookAt(mathutilData->mtxB, &cameraPos, &upVec, &cameraTarget);
    temp_r29->unk0->flags &= 0xFFFFFFDF;
    draw_monkey();
    temp_r29->unk0->flags |= 0x20;
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetNumTevStages_cached(1);
    GXLoadTexObj_cached(&texObj, GX_TEXMAP0);
    kcolor.r = 255;
    kcolor.g = 255;
    kcolor.b = 255;
    kcolor.a = temp->unkAAA8 * 255.0f;
    GXSetTevKColor_cached(GX_KCOLOR0, kcolor);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevKAlphaSel_cached(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3CU);
    GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevSwapMode_cached(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
    GXSetZMode_cached(GX_ENABLE, GX_GEQUAL, GX_DISABLE);
    GXSetBlendMode_cached(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    {
        GXColor fogColor = {0};
        GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, fogColor);
    }
    GXSetCullMode_cached(GX_CULL_NONE);
    mathutil_mtxA_from_identity();
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    gxutil_set_vtx_attrs((1 << GX_VA_POS) | (1 << GX_VA_TEX0));
    y1 = -10.0f * currentCameraStructPtr->sub28.unk38;
    y2 = -y1;
    x1 = -10.0f * (currentCameraStructPtr->sub28.unk38 * currentCameraStructPtr->sub28.aspect);
    x2 = -x1;
    GXBegin(GX_QUADS, GX_VTXFMT0, 4U);
    GXPosition3f32(x1, y1, -10.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(x2, y1, -10.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(x2, y2, -10.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(x1, y2, -10.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
    camera_apply_viewport(u_cameraId1);
}
