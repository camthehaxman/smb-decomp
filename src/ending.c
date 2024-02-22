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
#include "polydisp.h"
#include "rend_efc.h"
#include "shadow.h"
#include "sound.h"
#include "sprite.h"
#include "stage.h"
#include "stcoli.h"
#include "textbox.h"
#include "thread.h"
#include "window.h"

#include "../data/common.nlobj.h"
#include "../data/common.gma.h"
#include "../data/bg_ending.gma.h"

struct EndingInfo endingInfo;

static void ending_state_init(void);
static void ending_state_beginner_intro_init(void);
static void ending_state_beginner_intro_main(void);
static void ending_state_beginner_dialogue_init(void);
static void ending_state_beginner_dialogue_main(void);
static void ending_state_beginner_end_init(void);
static void ending_state_beginner_end_main(void);
static void ending_state_advanced_intro_init(void);
static void ending_state_advanced_intro_main(void);
static void ending_state_advanced_dialogue_init(void);
static void ending_state_advanced_dialogue_main(void);
static void ending_state_advanced_end_init(void);
static void ending_state_advanced_end_main(void);
static void ending_state_expert_intro_init(void);
static void ending_state_expert_intro_main(void);
static void ending_state_expert_dialogue_init(void);
static void ending_state_expert_dialogue_main(void);
static void ending_state_expert_fallbanana_init(void);
static void ending_state_expert_fallbanana_main(void);
static void ending_state_expert_house_init(void);
static void ending_state_expert_house_main(void);
static void ending_state_expert_walkin_init(void);
static void ending_state_expert_walkin_main(void);
static void ending_state_expert_breakin_init(void);
static void ending_state_expert_breakin_main(void);
static void ending_state_expert_end_init(void);
static void ending_state_expert_end_main(void);
static void ending_state_expert_apecam_init(void);
static void ending_state_expert_apecam_main(void);
static void ending_state_expert_gorcam_init(void);
static void ending_state_expert_gorcam_main(void);
static void ending_state_end_init(void);
static void ending_state_end_main(void);
static void func_800BB068(void);
static void ending_hgate_main(void);
static void func_800BBA54(void);
static void ending_hgate_draw(void);
static void func_800BBB10(int);
static void ending_chara_init(void);
static void ending_chara_main(void);
static void func_800BC820(void);
static void ending_chara_draw(void);
static int set_ending_chara(int, enum Character);
static void ending_ape_thread(struct Ape *, int);
static void ending_banana_init(void);
static void ending_banana_main(void);
static void ending_banana_each_coli(void);
static void func_800BF2D0(void);
static void ending_banana_draw(void);
static void set_house_ending_bananas(void);
static void func_800BF658(void);
static void func_800BF774(void);
static void set_freefall_ending_bananas(int);
static void set_cam_ride_ending_bananas(int, int);
static void spawn_beginner_banana(int);
static void func_800BFA2C(void);
static void rend_efc_ape_face(void);

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
    ending_state_beginner_intro_init,
    ending_state_beginner_intro_main,
    ending_state_beginner_dialogue_init,
    ending_state_beginner_dialogue_main,
    ending_state_beginner_end_init,
    ending_state_beginner_end_main,
    ending_state_advanced_intro_init,
    ending_state_advanced_intro_main,
    ending_state_advanced_dialogue_init,
    ending_state_advanced_dialogue_main,
    ending_state_advanced_end_init,
    ending_state_advanced_end_main,
    ending_state_expert_intro_init,
    ending_state_expert_intro_main,
    ending_state_expert_dialogue_init,
    ending_state_expert_dialogue_main,
    ending_state_expert_fallbanana_init,
    ending_state_expert_fallbanana_main,
    ending_state_expert_house_init,
    ending_state_expert_house_main,
    ending_state_expert_walkin_init,
    ending_state_expert_walkin_main,
    ending_state_expert_breakin_init,
    ending_state_expert_breakin_main,
    ending_state_expert_end_init,
    ending_state_expert_end_main,
    ending_state_expert_apecam_init,
    ending_state_expert_apecam_main,
    ending_state_expert_gorcam_init,
    ending_state_expert_gorcam_main,
    ending_state_end_init,
    ending_state_end_main,
};

void ending_init(void)
{
    memset(&endingInfo, 0, sizeof(endingInfo));
    endingInfo.state = 0;
    endingInfo.nextState = -1;
    event_finish_all();
    endingInfo.work = OSAllocFromHeap(stageHeap, sizeof(*endingInfo.work));
    if (endingInfo.work == NULL)
    {
        printf("fail to alloc memory for ending-management!\n");
        endingInfo.state = 31;
        return;
    }
    memset(endingInfo.work, 0, sizeof(*endingInfo.work));
    SoundGroupLoad(SOUND_GRPending);
    ending_chara_init();
    ending_banana_init();
    func_800BB068();
    ending_camera_init();
    ending_camera_emptyfunc2();
    submodeFinishFunc = ending_finish;
}

// runs the ending. returns FALSE when done
int ending_main(void)
{
    struct EndingWork *work;

    if (endingInfo.work == NULL)
        return FALSE;
    if (debugFlags & 0xA)
        return TRUE;
    if (endingInfo.nextState != -1)
        endingInfo.state = endingInfo.nextState;
    endingFuncs[endingInfo.state]();
    ending_chara_main();
    ending_banana_main();
    ending_hgate_main();
    if (endingInfo.flags & 2)
        endingInfo.work->unk0 += 0.01666666753590107f;

    work = endingInfo.work;

    work->unkAA54.unk4.x += work->unkAA54.unk10.x;
    work->unkAA54.unk4.y += work->unkAA54.unk10.y;
    work->unkAA54.unk4.z += work->unkAA54.unk10.z;

    work->unkAA54.unk2C.x += work->unkAA54.unk38.x;
    work->unkAA54.unk2C.y += work->unkAA54.unk38.y;
    work->unkAA54.unk2C.z += work->unkAA54.unk38.z;

    return (endingInfo.state != 32);
}

void ending_finish(void)
{
    ending_effect_finish();
    ending_camera_finish();
    func_800BBA54();
    func_800BF2D0();
    func_800BC820();
    OSFreeToHeap(stageHeap, endingInfo.work);
    endingInfo.work = NULL;
    event_finish_all();
    SoundGroupFree();
    if (submodeFinishFunc == ending_finish)
        submodeFinishFunc = NULL;
}

void u_ending_draw(void)
{
    if (endingInfo.work == NULL)
        return;
    change_current_camera(modeCtrl.currPlayer);
    u_draw_ball_shadow();
    background_light_assign();
    ending_banana_draw();
    if (endingInfo.flags & 1)
    {
        mathutil_mtxA_from_mtxB();
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_culled_sort_translucent(decodedBgGma->modelEntries[INSIDE_BANANA].model);
    }
    ending_chara_draw();
    ending_hgate_draw();
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
    if (endingInfo.flags & 0x20)
        rend_efc_ape_face();
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

static void ending_state_beginner_intro_init(void)
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
    set_ending_chara(0, playerCharacterSelection[modeCtrl.currPlayer]);
    endingInfo.work->characters[0].unk70 = 1;
    CAMERA_FOREACH(camera->subState = 0;)
    r28 = &endingInfo.work->unkAA54;
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
    if (endingInfo.work->characters[0].ape != NULL)
        endingInfo.work->characters[0].ape->colorId = modeCtrl.currPlayer;
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 30);
    u_play_music(0x44, 0);
}

/* Beginner course ending scene */

static s16 lbl_802F18C0[4] = { 0x00EA, 0x0102, 0x0142, 0x0173 };

static void ending_state_beginner_intro_main(void)
{
    float temp_f2;
    struct EndingSceneCharacter *chara;
    struct SomeBigEndingStruct_sub *temp_r6;

    temp_r6 = &endingInfo.work->unkAA54;
    chara = &endingInfo.work->characters[0];
    switch (modeCtrl.submodeTimer)
    {
    case 414:
        spawn_beginner_banana(modeCtrl.submodeTimer - 180);
        break;
    case 308:
        u_play_sound_0(0x230);
        break;
    case 240:
        chara->state = 1;
        chara->timer = modeCtrl.submodeTimer - 120;
        break;
    case 180:
        temp_f2 = 1.0f / modeCtrl.submodeTimer;
        temp_r6->unk10.x = -temp_r6->unk4.x * temp_f2;
        temp_r6->unk10.y = (20.0f - temp_r6->unk4.y) * temp_f2;
        temp_r6->unk10.z = (-61.0f - temp_r6->unk4.z) * temp_f2;
        break;
    case 135:
        SoundReq(lbl_802F18C0[chara->ape->charaId]);
        break;
    case 120:
        func_800BFA2C();
        u_play_sound_0(0x231);
        break;
    case 105:
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

static void ending_state_beginner_dialogue_init(void)
{
    struct EndingWork *work;
    struct SomeBigEndingStruct_sub *temp_r29;
    Vec sp30;
    struct TextBox tbox;
    struct EndingSceneCharacter *chara;
    int i;

    endingInfo.nextState = 4;
    modeCtrl.submodeTimer = 60;
    modeCtrl.unk18 = 30;
    CAMERA_FOREACH(camera->subState = 0;)
    temp_r29 = &endingInfo.work->unkAA54;
    memset(temp_r29, 0, sizeof(*temp_r29));
    temp_r29->unk0 = 1;
    mathutil_mtxA_from_rotate_z((s16) (globalAnimTimer << 6));
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

    work = endingInfo.work;
    work->unkAACC = 0;

    ending_prepare_chara_dialogue(&work->charaDialogue, work->dialogueBuf);
    if (work->charaDialogue.numLines > 0)
    {
        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_UP;
        tbox.x = 320;
        tbox.y = 322;
        tbox.numRows = MIN(work->charaDialogue.numLines, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, TEXTBOX_STATE_INIT, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, work->charaDialogue.lines[work->unkAACC]);
            work->charaDialogue.numLines--;
            work->unkAACC++;
        }
    }
    chara = &endingInfo.work->characters[0];
    chara->unkA = 1;
    chara->unk38 = 0;
}

static void ending_state_beginner_dialogue_main(void)
{
    Vec sp30;
    struct TextBox tbox;
    struct EndingWork *work;
    struct SomeBigEndingStruct_sub *temp_r29;
    struct EndingSceneCharacter *chara;
    int i2;

    temp_r29 = &endingInfo.work->unkAA54;
    chara = &endingInfo.work->characters[0];
    mathutil_mtxA_from_rotate_z(globalAnimTimer << 6);
    mathutil_mtxA_tf_vec_xyz(&sp30, 0.0f, 0.5f, 0.0f);
    temp_r29->unk4.x = sp30.x;
    temp_r29->unk4.y = 20.75f + (0.5f * sp30.y);
    temp_r29->unk4.z = -61.0f + sp30.z;
    chara->unk38--;
    if (chara->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[chara->ape->charaId][chara->unk3A & 1]);
        chara->unk38 = 240.0f * (0.8f + (0.4f * RAND_FLOAT()));
        chara->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & PAD_BUTTON_A))
    {
        modeCtrl.unk18 = 30;
        work = endingInfo.work;
        if (work->charaDialogue.numLines != 0)
        {
            textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = TEXTBOX_STYLE_CENTER_UP;
            tbox.x = 320;
            tbox.y = 322;
            tbox.numRows = work->charaDialogue.numLines;
            tbox.callback = NULL;
            textbox_set_properties(0, TEXTBOX_STATE_INIT, &tbox);
            for (i2 = 0; i2 < tbox.numRows; i2++)
            {
                textbox_add_text(0, work->charaDialogue.lines[work->unkAACC]);
                work->charaDialogue.numLines--;
                work->unkAACC++;
            }
        }
        else
        {
            endingInfo.nextState = 5;
            textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

static void ending_state_beginner_end_init(void)
{
    struct SomeBigEndingStruct_sub *temp_r29;

    endingInfo.nextState = 6;
    modeCtrl.submodeTimer = 360;
    CAMERA_FOREACH(camera->subState = 0;)
    temp_r29 = &endingInfo.work->unkAA54;
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
    endingInfo.work->characters[0].unkA = 2;
}

static s16 lbl_802F18C8[4] = { 0x00B7, 0x0102, 0x0141, 0x018B };

static void ending_state_beginner_end_main(void)
{
    switch (modeCtrl.submodeTimer)
    {
    case 240:
        endingInfo.work->characters[0].state = 4;
        break;
    case 279:
        SoundReq(lbl_802F18C8[playerCharacterSelection[modeCtrl.currPlayer]]);
        break;
    case 30:
        start_screen_fade(FADE_OUT, RGBA(255, 255, 255, 0), modeCtrl.submodeTimer);
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        endingInfo.nextState = 31;
}

/* Advanced course ending scene */

static void ending_state_advanced_intro_init(void)
{
    int stageId;
    float temp_f31;
    struct SomeBigEndingStruct_sub *temp_r31;
    struct Ape *ape;

    endingInfo.nextState = 8;
    modeCtrl.submodeTimer = 390;
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
    set_ending_chara(0, playerCharacterSelection[modeCtrl.currPlayer]);
    endingInfo.work->characters[0].state = 6;
    endingInfo.work->characters[0].timer = modeCtrl.submodeTimer - 180;
    CAMERA_FOREACH(camera->subState = 0;)
    temp_f31 = 1.0f / (modeCtrl.submodeTimer - 300);
    temp_r31 = &endingInfo.work->unkAA54;
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
    ape = endingInfo.work->characters[0].ape;
    if (ape != NULL)
        ape->colorId = modeCtrl.currPlayer;
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 30);
    u_play_music(0x44U, 0);
    u_play_sound_0(0x232);
}

static s16 lbl_802F18D0[4] = { 0x00EC, 0x0115, 0x016D, 0x0173 };
static s16 lbl_802F18D8[4] = { 0x00B5, 0x00FF, 0x0138, 0x0192 };

static void ending_state_advanced_intro_main(void)
{
    Vec spB4;
    struct Effect effect;
    float temp_f30;
    float temp_f31;
    struct EndingSceneCharacter *chara;
    struct SomeBigEndingStruct_sub *temp_r29;

    temp_r29 = &endingInfo.work->unkAA54;
    chara = &endingInfo.work->characters[0];
    switch (modeCtrl.submodeTimer)
    {
    case 375:
        temp_r29->unk2 = 2;
        temp_r29->unk44 = 0;
        temp_r29->unk46 = 0;
        temp_r29->unk48.x = 0.0f;
        temp_r29->unk48.y = 0.0f;
        temp_r29->unk48.z = 0.0f;
        SoundReq(lbl_802F18D0[playerCharacterSelection[modeCtrl.currPlayer]]);
        break;
    case 300:
        temp_r29->unk0 = 2;
        temp_r29->unk1C = 0;
        temp_r29->unk1E = 0;
        temp_r29->unk20.x = 0.0f;
        temp_r29->unk20.y = 0.0f;
        temp_r29->unk20.z = -2.0f;
        break;
    case 180:
        break;
    case 67:
        chara->ape->flags &= 0xFFFFCFFF;
        if (modeCtrl.playerCount > 1)
            chara->unk56 = modeCtrl.currPlayer;
        else
            chara->unk56 = 3;
        erase_effect(ET_ENDING_BALLFRAG);
        CAMERA_FOREACH(camera->subState = 0;)
        temp_f31 = 1.0f / (modeCtrl.submodeTimer - 56);
        temp_f30 = 1.25f * (0.9f + (0.2f * RAND_FLOAT()));
        temp_r29->unk0 = 1;
        mathutil_mtxA_from_rotate_y(rand() & 0x7FFF);
        mathutil_mtxA_rotate_x(rand() & 0x7FFF);
        mathutil_mtxA_tf_vec_xyz(&temp_r29->unk4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk4.x += chara->pos.x;
        temp_r29->unk4.y += chara->pos.y;
        temp_r29->unk4.z += chara->pos.z;
        mathutil_mtxA_rotate_y((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_rotate_x((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_tf_vec_xyz(&spB4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk10.x = temp_f31 * ((spB4.x + chara->pos.x) - temp_r29->unk4.x);
        temp_r29->unk10.y = temp_f31 * ((spB4.y + chara->pos.y) - temp_r29->unk4.y);
        temp_r29->unk10.z = temp_f31 * ((spB4.z + chara->pos.z) - temp_r29->unk4.z);
        break;
    case 60:
        chara->ape->flags &= 0xFFFFCFFF;
        if (modeCtrl.playerCount > 1)
            chara->unk56 = modeCtrl.currPlayer;
        else
            chara->unk56 = 3;
        erase_effect(ET_ENDING_BALLFRAG);
        CAMERA_FOREACH(camera->subState = 0;)
        temp_f31 = 1.0f / modeCtrl.submodeTimer;
        temp_f30 = 10.0f * (0.9f + (0.2f * RAND_FLOAT()));
        temp_r29->unk0 = 1;
        mathutil_mtxA_from_rotate_y(rand() & 0x7FFF);
        mathutil_mtxA_rotate_x(rand() & 0x7FFF);
        mathutil_mtxA_tf_vec_xyz(&temp_r29->unk4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk4.x += chara->pos.x;
        temp_r29->unk4.y += chara->pos.y;
        temp_r29->unk4.z += chara->pos.z;
        mathutil_mtxA_rotate_y((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_rotate_x((rand() & 0x7FF) - 0x400);
        mathutil_mtxA_tf_vec_xyz(&spB4, 0.0f, 0.0f, temp_f30);
        temp_r29->unk10.x = temp_f31 * ((spB4.x + chara->pos.x) - temp_r29->unk4.x);
        temp_r29->unk10.y = temp_f31 * ((spB4.y + chara->pos.y) - temp_r29->unk4.y);
        temp_r29->unk10.z = temp_f31 * ((spB4.z + chara->pos.z) - temp_r29->unk4.z);
        break;
    case 105:
    case 66:
    case 59:
        chara->unkA = 4;
        break;
    case 104:
    case 65:
    case 58:
        if (chara->ape->unk9C != 4)
            modeCtrl.submodeTimer++;
        else
            chara->ape->flags |= 0x1000;
        break;
    case 75:
    case 64:
    case 57:
        if (chara->ape->unk0->u_currKeyframe == (chara->ape->unk0->u_keyframeCount >> 1))
            SoundReq(lbl_802F18D8[playerCharacterSelection[modeCtrl.currPlayer]]);
        if (chara->ape->unk0->u_currKeyframe > chara->ape->unk0->u_keyframeCount - 0x10)
        {
            memset(&effect, 0, sizeof(effect));
            effect.type = ET_ENDING_BALLFRAG;
            effect.pos.x = chara->pos.x;
            effect.pos.y = 0.1f + chara->pos.y;
            effect.pos.z = chara->pos.z;
            effect.rotX = chara->unk58;
            effect.rotY = chara->unk5A;
            effect.rotZ = chara->unk5C;
            mathutil_mtxA_from_rotate_y(effect.rotY);
            mathutil_mtxA_rotate_x(effect.rotX);
            mathutil_mtxA_rotate_z(effect.rotZ);
            effect.unkA0 = -1;
            effect.unk88.x = 0.0f;
            effect.unk88.y = 0.0f;
            effect.unk88.z = -1.0f;
            mathutil_mtxA_tf_vec(&effect.unk88, &effect.vel);
            temp_f31 = 0.2f * (1.0f + (0.2f * RAND_FLOAT()));
            effect.vel.x *= temp_f31;
            effect.vel.y *= temp_f31;
            effect.vel.z *= temp_f31;
            effect.vel.x += chara->vel.x;
            effect.vel.y += chara->vel.y;
            effect.vel.z += chara->vel.z;
            spawn_effect(&effect);
            effect.unkA0 = chara->unk56;
            effect.unk88.x = 0.0f;
            effect.unk88.y = 0.0f;
            effect.unk88.z = 1.0f;
            mathutil_mtxA_tf_vec(&effect.unk88, &effect.vel);
            effect.vel.x *= temp_f31;
            effect.vel.y *= temp_f31;
            effect.vel.z *= temp_f31;
            effect.vel.x += chara->vel.x;
            effect.vel.y += chara->vel.y;
            effect.vel.z += chara->vel.z;
            spawn_effect(&effect);
            chara->unk56 = -1;
            u_play_sound_0(0x233);
        }
        else
            modeCtrl.submodeTimer++;
        break;
    case 74:
    case 63:
    case 56:
        if (chara->ape->unk0->u_currKeyframe != chara->ape->unk0->u_keyframeCount - 1)
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
        endingInfo.flags &= 0xFFFFFFF7;
    }
}

static void ending_state_advanced_dialogue_init(void)
{
    struct TextBox tbox;
    int i;
    u8 unused[4];
    struct EndingWork *work;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct EndingSceneCharacter *chara;

    endingInfo.nextState = 10;
    modeCtrl.submodeTimer = 60;
    modeCtrl.unk18 = 30;
    work = endingInfo.work;
    work->unkAACC = 0;
    ending_prepare_chara_dialogue(&work->charaDialogue, work->dialogueBuf);
    if (work->charaDialogue.numLines > 0)
    {
        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_DOWN;
        tbox.x = 320;
        tbox.y = 160;
        tbox.numRows = MIN(work->charaDialogue.numLines, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, TEXTBOX_STATE_INIT, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, work->charaDialogue.lines[work->unkAACC]);
            work->charaDialogue.numLines--;
            work->unkAACC++;
        }
    }
    temp_r28 = &endingInfo.work->unkAA54;
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

    chara = &endingInfo.work->characters[0];
    chara->unkA = 6;
    chara->ape->flags &= 0xFFFFEFFF;
    chara->unk38 = 0;
}

static void ending_state_advanced_dialogue_main(void)
{
    struct TextBox tbox;
    int i;
    struct EndingWork *work;
    struct EndingSceneCharacter *chara;

    chara = &endingInfo.work->characters[0];
    chara->unk38--;
    if (chara->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[chara->ape->charaId][chara->unk3A & 1]);
        chara->unk38 = 240.0f * (0.8f + (0.4f * RAND_FLOAT()));
        chara->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & PAD_BUTTON_A))
    {
        modeCtrl.unk18 = 30;
        work = endingInfo.work;
        if (work->charaDialogue.numLines != 0)
        {
            textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = TEXTBOX_STYLE_CENTER_DOWN;
            tbox.x = 320;
            tbox.y = 160;
            tbox.numRows = work->charaDialogue.numLines;
            tbox.callback = NULL;
            textbox_set_properties(0, TEXTBOX_STATE_INIT, &tbox);
            for (i = 0; i < tbox.numRows; i++)
            {
                textbox_add_text(0, work->charaDialogue.lines[work->unkAACC]);
                work->charaDialogue.numLines--;
                work->unkAACC++;
            }
        }
        else
        {
            endingInfo.nextState = 11;
            textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

static void ending_state_advanced_end_init(void)
{
    struct Ape *ape;

    endingInfo.nextState = 12;
    modeCtrl.submodeTimer = 540;
    endingInfo.work->characters[0].unkA = 7;
    ape = endingInfo.work->characters[0].ape;
    ape->flags &= 0xFFFFBFFF;
}

static s16 lbl_802F18E0[4] = { 0x00EA, 0x0102, 0x016D, 0x0173 };

void ending_state_advanced_end_main(void)
{
    struct SomeBigEndingStruct_sub *temp_r28;
    struct EndingSceneCharacter *chara;
    void *temp_r4;
    int tm;
    u8 unused[8];

    temp_r28 = &endingInfo.work->unkAA54;
    chara = &endingInfo.work->characters[0];
    switch (modeCtrl.submodeTimer)
    {
    case 539:
        if (chara->ape->unk9C != 7)
            modeCtrl.submodeTimer++;
        else
        {
            chara->ape->flags |= 0x1000;
            temp_r28->unk2 = 2;
            temp_r28->unk44 = 0;
            temp_r28->unk46 = 5;
            temp_r28->unk48.x = 0.0f;
            temp_r28->unk48.y = 0.0f;
            temp_r28->unk48.z = 0.0f;
        }
        break;
    case 538:
        if (chara->ape->unk0->u_currKeyframe > chara->ape->unk0->u_keyframeCount - 8)
        {
            chara->state = 8;
            chara->timer = modeCtrl.submodeTimer - 420;
            temp_r28->unk0 = 1;
            temp_r28->unk4 = cameraInfo[modeCtrl.currPlayer].eye;
            temp_r28->unk10 = cameraInfo[modeCtrl.currPlayer].eyeVel;
            u_play_sound_0(0x232);
            SoundReq(lbl_802F18E0[playerCharacterSelection[modeCtrl.currPlayer]]);
        }
        else
            modeCtrl.submodeTimer++;
        break;
    case 420:
        endingInfo.flags |= 0x20;
        break;
    case 240:
        endingInfo.flags &= 0xFFFFFFDF;
        break;
    case 270:
        set_cam_ride_ending_bananas(1, modeCtrl.submodeTimer - 30);
        break;
    case 30:
        start_screen_fade(FADE_OUT, RGBA(255, 255, 255, 0), modeCtrl.submodeTimer);
        u_play_sound_0(0x22F);
        break;
    }
    tm = modeCtrl.submodeTimer;
    if (tm < 420 && tm > 360)
        endingInfo.work->unkAAA8 = 0.5f + 0.5f * (-0.016666668f * (float)(360 - tm));
    else if (tm < 300 && tm > 240)
        endingInfo.work->unkAAA8 = 0.5f + 0.5f * (1.0f - (-0.016666668f * (float)(240 - tm)));
    if (modeCtrl.submodeTimer < 420 && modeCtrl.submodeTimer > 240)
        endingInfo.work->unkAAAC = 0.6f + 0.19999999f * (-0.0055555557f * (float)(240 - modeCtrl.submodeTimer));
    if (modeCtrl.submodeTimer < 300 && modeCtrl.submodeTimer > 0)
    {
        float t = -0.0033333334f * -modeCtrl.submodeTimer;
        t = 1.0f - t;
        if (RAND_FLOAT() < t)
            set_freefall_ending_bananas(1);
        if (globalAnimTimer % 90 == 0)
            u_play_sound_0(0x22E);
    }
    temp_r28->unk10.x *= 0.99f;
    temp_r28->unk10.y *= 0.99f;
    temp_r28->unk10.z *= 0.99f;
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        endingInfo.nextState = 31;
}

static void ending_state_expert_intro_init(void)
{
    int temp_r0;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct Ape *temp_r3;
    int stageId;

    endingInfo.nextState = 14;
    modeCtrl.submodeTimer = 180;
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
    set_ending_chara(0, CHARACTER_AIAI);
    set_ending_chara(1, CHARACTER_MEEMEE);
    set_ending_chara(2, CHARACTER_BABY);
    set_ending_chara(3, CHARACTER_GONGON);
    endingInfo.work->characters[0].state = 9;
    endingInfo.work->characters[1].state = 9;
    endingInfo.work->characters[2].state = 9;
    CAMERA_FOREACH(camera->subState = 0;)
    temp_r28 = &endingInfo.work->unkAA54;
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
        struct EndingSceneCharacter *chara = &endingInfo.work->characters[temp_r0];
        chara->ape->flags &= 0xFFFFFFDF;
        chara->unkA = 0x12;
        chara->pos.x = -20.38f;
        chara->pos.y = -2.34f;
        chara->pos.z = -20.39f;
        if (modeCtrl.playerCount == 1)
            chara->unk56 = 3;
        else
            chara->unk56 = modeCtrl.currPlayer;
        chara->unk58 = (rand() & 0x7FFF);
        chara->unk5A = (rand() & 0x7FFF);
        chara->unk5C = (rand() & 0x7FFF);
    }
    temp_r3 = endingInfo.work->characters[modeCtrl.currPlayer].ape;
    if (temp_r3 != NULL)
        temp_r3->colorId = modeCtrl.currPlayer;
    start_screen_fade(FADE_IN|FADE_ABOVE_SPRITES, RGBA(0, 0, 0, 0), 30);
    u_play_music(0x44U, 0);
}

/* Expert course ending scene */

static void ending_state_expert_intro_main(void)
{
    struct SomeBigEndingStruct_sub *temp_r30 = &endingInfo.work->unkAA54;

    mathutil_mtxA_from_rotate_y(modeCtrl.submodeTimer * 4);
    mathutil_mtxA_tf_vec_xyz(&temp_r30->unk4, 0.0f, 20.0f, 12.0f);
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0)
        endingInfo.nextState = 15;
}

static void ending_state_expert_dialogue_init(void)
{
    struct TextBox tbox;
    float temp_f31;
    struct EndingWork *work;
    struct SomeBigEndingStruct_sub *temp_r26;
    struct EndingSceneCharacter *chara;
    int i;

    endingInfo.nextState = 16;
    modeCtrl.submodeTimer = 300;
    modeCtrl.unk18 = 30;
    CAMERA_FOREACH(camera->subState = 0;)
    temp_r26 = &endingInfo.work->unkAA54;
    chara = &endingInfo.work->characters[playerCharacterSelection[modeCtrl.currPlayer]];
    temp_f31 = modeCtrl.submodeTimer / 300.0f;
    mathutil_mtxA_from_translate(&chara->pos);
    mathutil_mtxA_rotate_y(chara->unk30.y);
    mathutil_mtxA_rotate_x(chara->unk30.x);
    mathutil_mtxA_rotate_z(chara->unk30.z);
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
    endingInfo.work->characters[playerCharacterSelection[modeCtrl.currPlayer]].unk70 = 1;
    work = endingInfo.work;
    work->unkAACC = 0;
    ending_prepare_chara_dialogue(&work->charaDialogue, work->dialogueBuf);
    if (work->charaDialogue.numLines > 0)
    {
        memset(&tbox, 0, sizeof(tbox));
        tbox.style = TEXTBOX_STYLE_CENTER_UP;
        tbox.x = 320;
        tbox.y = 322;
        tbox.numRows = MIN(work->charaDialogue.numLines, 2);
        tbox.callback = NULL;
        textbox_set_properties(0, TEXTBOX_STATE_INIT, &tbox);
        for (i = 0; i < tbox.numRows; i++)
        {
            textbox_add_text(0, work->charaDialogue.lines[work->unkAACC]);
            work->charaDialogue.numLines--;
            work->unkAACC++;
        }
    }
    chara = &endingInfo.work->characters[playerCharacterSelection[modeCtrl.currPlayer]];
    chara->unk38 = 0;
    if (playerCharacterSelection[modeCtrl.currPlayer] != 3)
        chara->unkA = 0xB;
    else
        chara->unkA = 0xC;
    endingInfo.work->characters[0].unk36 |= 1;
    endingInfo.work->characters[1].unk36 |= 1;
    endingInfo.work->characters[2].unk36 |= 1;
    endingInfo.work->characters[3].unk36 |= 1;
}

static void ending_state_expert_dialogue_main(void)
{
    struct TextBox tbox;
    float t;
    struct EndingWork *work;
    struct SomeBigEndingStruct_sub *temp_r28;
    struct EndingSceneCharacter *chara;
    int i;

    temp_r28 = &endingInfo.work->unkAA54;
    chara = &endingInfo.work->characters[playerCharacterSelection[modeCtrl.currPlayer]];
    t = modeCtrl.submodeTimer / 300.0f;
    mathutil_mtxA_from_translate(&chara->pos);
    mathutil_mtxA_rotate_y(chara->unk30.y);
    mathutil_mtxA_rotate_x(chara->unk30.x);
    mathutil_mtxA_rotate_z(chara->unk30.z);
    mathutil_mtxA_tf_point_xyz(&temp_r28->unk4, -4.0f * (t - 0.5f), -0.2f, -2.0f);
    chara->unk38--;
    if (chara->unk38 < 0)
    {
        SoundReq(lbl_801E2B28[chara->ape->charaId][chara->unk3A & 1]);
        chara->unk38 = 240.0f * (0.8f + (0.4f * RAND_FLOAT()));
        chara->unk3A++;
    }
    if (modeCtrl.unk18 != 0)
        modeCtrl.unk18--;
    if (modeCtrl.unk18 == 0 && (g_currPlayerButtons[2] & PAD_BUTTON_A))
    {
        modeCtrl.unk18 = 30;
        work = endingInfo.work;
        if (work->charaDialogue.numLines != 0)
        {
            textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
            memset(&tbox, 0, sizeof(tbox));
            tbox.style = TEXTBOX_STYLE_CENTER_UP;
            tbox.x = 320;
            tbox.y = 322;
            tbox.numRows = work->charaDialogue.numLines;
            tbox.callback = NULL;
            textbox_set_properties(0, 1, &tbox);
            for (i = 0; i < tbox.numRows; i++)
            {
                textbox_add_text(0, work->charaDialogue.lines[work->unkAACC]);
                work->charaDialogue.numLines--;
                work->unkAACC++;
            }
        }
        else
        {
            endingInfo.nextState = 17;
            textbox_set_properties(0, TEXTBOX_STATE_FADEOUT, NULL);
        }
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
}

static void ending_state_expert_fallbanana_init(void)
{
    endingInfo.nextState = 18;
    modeCtrl.submodeTimer = 480;
    func_800BBB10(modeCtrl.submodeTimer - 420);
    endingInfo.work->characters[playerCharacterSelection[modeCtrl.currPlayer]].unk74 = cameraInfo[0].eye;
}

static s16 lbl_802F18E8[3] = { 0x00E4, 0x0127, 0x014A };

static void ending_state_expert_fallbanana_main(void)
{
    f32 temp_f2;
    f32 temp_f3;
    struct SomeBigEndingStruct_sub *temp_r26;
    struct EndingSceneCharacter *chara;
    struct Ape *ape;
    struct EndingWork *work;
    Vec sp8;

    switch (modeCtrl.submodeTimer)
    {
    case 479:
        CAMERA_FOREACH(camera->subState = 0;)
        temp_f3 = 1.0f / (modeCtrl.submodeTimer - 360);
        work = endingInfo.work;
        work->unkAA54.unk0 = 1;
        work->unkAA54.unk4.x = 0.0f;
        work->unkAA54.unk4.y = 0.0f;
        work->unkAA54.unk4.z = 6.5f;
        work->unkAA54.unk10.x = -work->unkAA54.unk4.x * temp_f3;
        work->unkAA54.unk10.y = (1.5f - work->unkAA54.unk4.y) * temp_f3;
        work->unkAA54.unk10.z = (6.0f - work->unkAA54.unk4.z) * temp_f3;
        work->unkAA54.unk2 = 1;
        work->unkAA54.unk2C = endingInfo.work->unkAA04.unk8;
        work->unkAA54.unk38.x = 0.0f;
        work->unkAA54.unk38.y = 0.0f;
        work->unkAA54.unk38.z = 0.0f;
        break;
    case 420:
        ape = endingInfo.work->characters[playerCharacterSelection[modeCtrl.currPlayer]].ape;
        ape->flags &= 0xFFFFFFBF;
        u_play_sound_0(0x234);
        break;
    case 360:
        endingInfo.work->characters[0].unkA = 8;
        endingInfo.work->characters[1].unkA = 8;
        endingInfo.work->characters[2].unkA = 8;
        SoundReq(lbl_802F18E8[playerCharacterSelection[modeCtrl.currPlayer] % 3]);
        CAMERA_FOREACH(camera->subState = 0;)
        work = endingInfo.work;
        temp_r26 = &work->unkAA54;
        chara = &work->characters[playerCharacterSelection[modeCtrl.currPlayer] % 3];
        memset(temp_r26, 0, sizeof(*temp_r26));
        mathutil_mtxA_from_translate(&chara->pos);
        mathutil_mtxA_rotate_y(chara->unk30.y);
        mathutil_mtxA_rotate_x(chara->unk30.x);
        mathutil_mtxA_rotate_z(chara->unk30.z);
        temp_r26->unk0 = 1;
        mathutil_mtxA_tf_point_xyz(&temp_r26->unk4, 1.0f, 3.0f, -4.0f);
        temp_r26->unk10.y = (8.0f - temp_r26->unk4.y) * (1.0f / modeCtrl.submodeTimer);
        mathutil_mtxA_tf_point_xyz(&temp_r26->unk2C, 0.0f, 0.2f, 0.0f);
        break;
    case 300:
        set_cam_ride_ending_bananas(1, modeCtrl.submodeTimer - 30);
        temp_r26 = &endingInfo.work->unkAA54;
        temp_f2 = 1.0f / modeCtrl.submodeTimer;
        temp_r26->unk10.x = temp_r26->unk4.x * temp_f2;
        temp_r26->unk10.z = temp_r26->unk4.z * temp_f2;
        temp_r26->unk38.x = -temp_r26->unk2C.x * temp_f2;
        temp_r26->unk38.y = (40.0f - temp_r26->unk2C.y) * temp_f2;
        temp_r26->unk38.z = -temp_r26->unk2C.z * temp_f2;
        break;
    case 120:
        endingInfo.work->characters[0].state = 16;
        endingInfo.work->characters[1].state = 16;
        endingInfo.work->characters[2].state = 16;
        endingInfo.work->characters[3].unk56 = -1;
        sp8.x = 0.0f;
        sp8.y = 4.0f;
        sp8.z = 0.0f;
        endingInfo.work->characters[0].unk70 = 2;
        endingInfo.work->characters[1].unk70 = 2;
        endingInfo.work->characters[2].unk70 = 2;
        endingInfo.work->characters[0].unk74 = sp8;
        endingInfo.work->characters[1].unk74 = sp8;
        endingInfo.work->characters[2].unk74 = sp8;
        endingInfo.work->characters[0].unkA = 0xC;
        endingInfo.work->characters[1].unkA = 0xC;
        endingInfo.work->characters[2].unkA = 0xC;
        break;
    case 30:
        start_screen_fade(FADE_OUT, RGBA(255, 255, 255, 0), modeCtrl.submodeTimer);
        u_play_sound_0(0x22F);
        break;
    }
    if (modeCtrl.submodeTimer < 479 && modeCtrl.submodeTimer > 420)
        endingInfo.work->unkAA54.unk2C = endingInfo.work->unkAA04.unk8;
    if (modeCtrl.submodeTimer < 420 && modeCtrl.submodeTimer > 0.0f)
    {
        float t = -0.0023809525f * -(float)modeCtrl.submodeTimer;
        t = 1.0f - t;
        if (RAND_FLOAT() < t)
            set_freefall_ending_bananas(1);
        if (globalAnimTimer % 90 == 0)
            u_play_sound_0(0x22E);
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        endingInfo.nextState = 19;
}

static void ending_state_expert_house_init(void)
{
    float temp_f4;
    struct SomeBigEndingStruct_sub *temp_r28;
    u8 unused[8];

    endingInfo.nextState = 20;
    modeCtrl.submodeTimer = 360;
    start_screen_fade(FADE_IN, RGBA(0, 0, 0, 0), 30);
    ending_banana_init();
    set_house_ending_bananas();
    CAMERA_FOREACH(camera->subState = 0;)
    temp_r28 = &endingInfo.work->unkAA54;
    memset(temp_r28, 0, sizeof(*temp_r28));
    temp_f4 = 1.0f / (modeCtrl.submodeTimer - 240);
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
    endingInfo.work->characters[0].unk38 = (s16) (1.0f + (120.0f * RAND_FLOAT()));
    endingInfo.work->characters[1].unk38 = (s16) (1.0f + (120.0f * RAND_FLOAT()));
    endingInfo.work->characters[2].unk38 = (s16) (1.0f + (120.0f * RAND_FLOAT()));
}

static s16 lbl_802F18F0[4] = { 0x00E8, 0x0101, 0x016B, 0x0000 };

static void ending_state_expert_house_main(void)
{
    float temp_f3;
    struct SomeBigEndingStruct_sub *temp_r4;
    int i;
    struct EndingSceneCharacter *chara;

    switch (modeCtrl.submodeTimer)
    {
    case 0:
        break;
    case 240:
        CAMERA_FOREACH(camera->subState = 0;)
        temp_r4 = &endingInfo.work->unkAA54;
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
    chara = &endingInfo.work->characters[0];
    for (i = 3; i > 0; i--, chara++)  // BUG: maybe should be >= 0 instead?
    {
        if (chara->unk38 != 0)
        {
            chara->unk38--;
            if (chara->unk38 == 0)
                SoundReq(lbl_802F18F0[chara->ape->charaId]);
        }
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        endingInfo.nextState = 21;
}

static void ending_state_expert_walkin_init(void)
{
    float temp_f3;
    struct SomeBigEndingStruct_sub *temp_r4;

    endingInfo.nextState = 22;
    modeCtrl.submodeTimer = 720;
    endingInfo.work->characters[0].ape->flags |= 0x20;
    endingInfo.work->characters[1].ape->flags |= 0x20;
    endingInfo.work->characters[2].ape->flags |= 0x20;
    endingInfo.work->characters[3].ape->flags |= 0x20;
    CAMERA_FOREACH(camera->subState = 0;)
    temp_r4 = &endingInfo.work->unkAA54;
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

static void ending_state_expert_walkin_main(void)
{
    float temp_f2;
    struct SomeBigEndingStruct_sub *temp;

    switch (modeCtrl.submodeTimer)
    {
    case 690:
        endingInfo.flags |= 4;
        u_play_sound_0(0x235);
        break;
    case 600:
        endingInfo.work->characters[0].state = 17;
        endingInfo.work->characters[1].state = 17;
        endingInfo.work->characters[2].state = 17;
        break;
    case 599:
        endingInfo.work->characters[0].ape->flags &= ~0x20;
        endingInfo.work->characters[1].ape->flags &= ~0x20;
        endingInfo.work->characters[2].ape->flags &= ~0x20;
        CAMERA_FOREACH(camera->subState = 0;)
        temp = &endingInfo.work->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 216);
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
    case 480:
        endingInfo.work->characters[3].ape->flags &= ~0x20;
        endingInfo.work->characters[3].state = 19;
        break;
    case 216:
        temp = &endingInfo.work->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 60);
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
    case 138:
        temp = &endingInfo.work->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 60);
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
    case 60:
        endingInfo.flags &= 0xFFFFFFFB;
        u_play_sound_0(0x236);
        temp = &endingInfo.work->unkAA54;
        temp->unk0 = 1;
        temp->unk10.x = 0.0f;
        temp->unk10.y = 0.0f;
        temp->unk10.z = 0.0f;
        break;
    }
    modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer < 0)
        endingInfo.nextState = 23;
}

static void ending_state_expert_breakin_init(void)
{
    struct SomeBigEndingStruct_sub *temp;

    endingInfo.nextState = 24;
    modeCtrl.submodeTimer = 300;
    endingInfo.work->characters[0].state = 21;
    endingInfo.work->characters[1].state = 21;
    endingInfo.work->characters[2].state = 21;
    endingInfo.work->characters[0].unk70 = 0;
    endingInfo.work->characters[1].unk70 = 0;
    endingInfo.work->characters[2].unk70 = 0;
    temp = &endingInfo.work->unkAA54;
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
    endingInfo.work->characters[0].unk36 &= ~0x1;
    endingInfo.work->characters[1].unk36 &= ~0x1;
    endingInfo.work->characters[2].unk36 &= ~0x1;
}

static void ending_state_expert_breakin_main(void)
{
    struct EndingSceneCharacter *chara = &endingInfo.work->characters[3];
    int temp_r28 = chara->ape->unk0->u_currKeyframe;

    if (chara->ape->unk9C == 8 && temp_r28 == 0x2A)
        SoundReq(0x181U);
    if (chara->ape->unk9C == 0xA)
    {
        if (temp_r28 == 1)
            SoundReq(0x185U);
        if (temp_r28 == 0x14 || temp_r28 == 0x24 || temp_r28 == 0x34 || temp_r28 == 0x44)
            u_play_sound_0(0x238);
    }
    if (chara->ape->unk9C == 0xBU && temp_r28 == 0xB2)
        SoundReq(0x199U);
    if (chara->ape->unk9C == 0xEU && (temp_r28 == 0x10 || temp_r28 == 0x2E))
        SoundReq(0x186U);
    if (chara->unk8 == 0xB && temp_r28 > 0xC3)
    {
        func_800BF658();
        endingInfo.nextState = 25;
        endingInfo.flags |= 2;
        endingInfo.work->unk0 = 0.0f;
        u_play_sound_0(0x238);
    }
}

static void ending_state_expert_end_init(void)
{
    endingInfo.nextState = 26;
    modeCtrl.submodeTimer = 480;
    modeCtrl.unk18 = 0;
    endingInfo.work->unkAA04.unk2 = 4;
}

static void ending_state_expert_end_main(void)
{
    struct SomeBigEndingStruct_sub *temp_r4 = &endingInfo.work->unkAA54;
    float temp_f2;

    switch (modeCtrl.submodeTimer)
    {
    case 465:
        CAMERA_FOREACH(camera->subState = 0;)
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 300);
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
    case 405:
        temp_r4->unk0 = 1;
        break;
    case 390:
        func_800BF774();
        break;
    case 300:
        temp_r4->unk10.x = 0.0f;
        temp_r4->unk10.y = 0.0f;
        temp_r4->unk10.z = 0.0f;
        break;
    case 210:
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 150);
        temp_r4->unk0 = 1;
        temp_r4->unk10.x = -temp_r4->unk4.x * temp_f2;
        temp_r4->unk10.y = (0.7f - temp_r4->unk4.y) * temp_f2;
        temp_r4->unk10.z = (4.2f - temp_r4->unk4.z) * temp_f2;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 90);
        temp_r4->unk2 = 1;
        temp_r4->unk2C = cameraInfo[modeCtrl.currPlayer].lookAt;
        temp_r4->unk38.x = -temp_r4->unk2C.x * temp_f2;
        temp_r4->unk38.y = (0.1f - temp_r4->unk2C.y) * temp_f2;
        temp_r4->unk38.z = -temp_r4->unk2C.z * temp_f2;
        break;
    case 150:
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 90);
        temp_r4->unk10.x = -temp_r4->unk4.x * temp_f2;
        temp_r4->unk10.y = (-0.4f - temp_r4->unk4.y) * temp_f2;
        temp_r4->unk10.z = (2.6f - temp_r4->unk4.z) * temp_f2;
        SoundReq(0xEEU);
        SoundReq(0x102U);
        SoundReq(0x16DU);
        break;
    case 90:
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
            endingInfo.nextState = 27;
        else
            endingInfo.nextState = 29;
    }
}

static void ending_state_expert_gorcam_init(void)
{
    endingInfo.nextState = 30;
    modeCtrl.submodeTimer = 330;
    modeCtrl.unk18 = 0;
}

static void ending_state_expert_gorcam_main(void)
{
    float temp_f2;
    int charaId;
    struct SomeBigEndingStruct_sub *var_r31 = &endingInfo.work->unkAA54;
    struct EndingSceneCharacter *chara;
    Vec sp8;

    switch (modeCtrl.submodeTimer)
    {
    case 330:
        CAMERA_FOREACH(camera->subState = 0;)
        var_r31 = &endingInfo.work->unkAA54;
        temp_f2 = 1.0f / (modeCtrl.submodeTimer - 210);
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        chara = &endingInfo.work->characters[charaId];
        var_r31->unk0 = 1;
        var_r31->unk4.x = chara->pos.x;
        var_r31->unk4.y = chara->pos.y - 0.5f;
        var_r31->unk4.z = chara->pos.z - 2.0f;
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
    case 210:
        var_r31->unk10.x = 0.0f;
        var_r31->unk10.y = 0.0f;
        var_r31->unk10.z = 0.0f;
        break;
    case 180:
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        endingInfo.work->characters[3].unkA = 0x11;
        chara = &endingInfo.work->characters[charaId];
        CAMERA_FOREACH(camera->subState = 0;)
        var_r31->unk0 = 1;
        var_r31->unk4.x = chara->pos.x;
        var_r31->unk4.y = 0.5f + chara->pos.y;
        var_r31->unk4.z = 2.5f + chara->pos.z;
        var_r31->unk10.x = 0.0f;
        var_r31->unk10.y = 0.0f;
        var_r31->unk10.z = 0.0f;
        var_r31->unk2 = 1;
        var_r31->unk2C.x = chara->pos.x;
        var_r31->unk2C.y = 0.25f + chara->pos.y;
        var_r31->unk2C.z = chara->pos.z;
        var_r31->unk38.x = 0.0f;
        var_r31->unk38.y = 0.0f;
        var_r31->unk38.z = 0.0f;
        break;
    case 66:
        SoundReq(0x188U);
        break;
    }
    if (modeCtrl.submodeTimer != 0)
        modeCtrl.submodeTimer--;
    if (modeCtrl.submodeTimer == 0 && modeCtrl.unk18 == 0)
    {
        modeCtrl.unk18 = 120;
        start_screen_fade(FADE_OUT, RGBA(255, 255, 255, 0), modeCtrl.unk18);
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
            endingInfo.nextState = 31;
    }
}

static void ending_state_expert_apecam_init(void)
{
    endingInfo.nextState = 28;
    modeCtrl.submodeTimer = 630;
    modeCtrl.unk18 = 0;
}

static s16 lbl_802F18F8[4] = { 0x00E8, 0x0102, 0x016D, 0x0000 };

static void ending_state_expert_apecam_main(void)
{
    struct Ape *ape;
    struct EndingSceneCharacter *chara;
    int charaId;
    struct SomeBigEndingStruct_sub *temp_r30;
    Vec sp38;
    Mtx sp8;
    float t;

    temp_r30 = &endingInfo.work->unkAA54;
    switch (modeCtrl.submodeTimer)
    {
    case 630:
        endingInfo.flags |= 0x10;
        CAMERA_FOREACH(camera->subState = 0;)
        t = 1.0f / (modeCtrl.submodeTimer - 510);
        charaId = (playerCharacterSelection[modeCtrl.currPlayer] + 2) % 3;
        temp_r30 = &endingInfo.work->unkAA54;
        chara = &endingInfo.work->characters[charaId];
        endingInfo.work->unkAA54.unk0 = 1;
        sp38.x = chara->pos.x;
        sp38.y = 0.0f;
        sp38.z = chara->pos.z;
        mathutil_vec_set_len(&sp38, &sp38, 1.25f);
        temp_r30->unk4.x = chara->pos.x + sp38.x;
        temp_r30->unk4.y = chara->pos.y;
        temp_r30->unk4.z = chara->pos.z + sp38.z;
        temp_r30->unk10.x = -0.5f * sp38.x * t;
        temp_r30->unk10.y = 0.5f * t;
        temp_r30->unk10.z = t * (1.0f + chara->pos.z - temp_r30->unk4.z);
        temp_r30->unk2 = 2;
        temp_r30->unk44 = charaId;
        temp_r30->unk46 = 0;
        temp_r30->unk48.x = 0.0f;
        temp_r30->unk48.y = 0.0f;
        temp_r30->unk48.z = 0.0f;
        break;
    case 480:
        CAMERA_FOREACH(camera->subState = 0;)
        t = 1.0f / (modeCtrl.submodeTimer - 360);
        charaId = (playerCharacterSelection[modeCtrl.currPlayer] + 1) % 3;
        temp_r30 = &endingInfo.work->unkAA54;
        chara = &endingInfo.work->characters[charaId];
        endingInfo.work->unkAA54.unk0 = 1;
        sp38.x = chara->pos.x;
        sp38.y = 0.0f;
        sp38.z = chara->pos.z;
        mathutil_vec_set_len(&sp38, &sp38, 1.25f);
        temp_r30->unk4.x = chara->pos.x + sp38.x;
        temp_r30->unk4.y = chara->pos.y;
        temp_r30->unk4.z = chara->pos.z + sp38.z;
        temp_r30->unk10.x = -0.5f * sp38.x * t;
        temp_r30->unk10.y = 0.5f * t;
        temp_r30->unk10.z = t * (1.0f + chara->pos.z - temp_r30->unk4.z);
        temp_r30->unk2 = 2;
        temp_r30->unk44 = charaId;
        temp_r30->unk46 = 0;
        temp_r30->unk48.x = 0.0f;
        temp_r30->unk48.y = 0.0f;
        temp_r30->unk48.z = 0.0f;
        break;
    case 330:
        CAMERA_FOREACH(camera->subState = 0;)
        t = 1.0f / (modeCtrl.submodeTimer - 210);
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        temp_r30 = &endingInfo.work->unkAA54;
        chara = &endingInfo.work->characters[charaId];
        endingInfo.work->unkAA54.unk0 = 1;
        sp38.x = chara->pos.x;
        sp38.y = 0.0f;
        sp38.z = chara->pos.z;
        mathutil_vec_set_len(&sp38, &sp38, 1.25f);
        temp_r30->unk4.x = chara->pos.x + sp38.x;
        temp_r30->unk4.y = chara->pos.y;
        temp_r30->unk4.z = chara->pos.z + sp38.z;
        temp_r30->unk10.x = -0.5f * sp38.x * t;
        temp_r30->unk10.y = 0.5f * t;
        temp_r30->unk10.z = t * (1.0f + chara->pos.z - temp_r30->unk4.z);
        temp_r30->unk2 = 2;
        temp_r30->unk44 = charaId;
        temp_r30->unk46 = 0;
        temp_r30->unk48.x = 0.0f;
        temp_r30->unk48.y = 0.0f;
        temp_r30->unk48.z = 0.0f;
        break;
    case 360:
    case 510:
    case 210:
        temp_r30->unk10.x = 0.0f;
        temp_r30->unk10.y = 0.0f;
        temp_r30->unk10.z = 0.0f;
        break;
    case 180:
        charaId = playerCharacterSelection[modeCtrl.currPlayer];
        endingInfo.work->characters[charaId].unkA = 0x10;
        chara = &endingInfo.work->characters[charaId];
        temp_r30->unk0 = 1;
        ape = chara->ape;
        mathutil_mtxA_from_quat(&ape->unk60);
        mathutil_mtxA_to_mtx(sp8);
        mathutil_mtxA_from_translate(&ape->pos);
        mathutil_mtxA_scale_s(ape->modelScale);
        mathutil_mtxA_translate(&ape->unk3C);
        mathutil_mtxA_mult_right(sp8);
        mathutil_mtxA_mult_right(ape->unk0->joints[5].transformMtx);
        mathutil_mtxA_tf_point_xyz(&sp38, 0.0f, 0.0f, 0.3f);
        sp38.y += -0.3f;
        sp38.z += 1.0f;
        t = 1.0f / (modeCtrl.submodeTimer - 66);
        temp_r30->unk10.x = t * (sp38.x - temp_r30->unk4.x);
        temp_r30->unk10.y = t * (sp38.y - temp_r30->unk4.y);
        temp_r30->unk10.z = t * (sp38.z - temp_r30->unk4.z);
        break;
    case 66:
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
        modeCtrl.unk18 = 120;
        start_screen_fade(FADE_OUT, RGBA(255, 255, 255, 0), modeCtrl.unk18);
    }
    if (modeCtrl.unk18 != 0)
    {
        modeCtrl.unk18--;
        if (modeCtrl.unk18 == 0)
            endingInfo.nextState = 31;
    }
}

static void ending_state_end_init(void)
{
    endingInfo.nextState = 32;
}

static void ending_state_end_main(void) {}

static void func_800BB068(void)
{
    endingInfo.work->unkAA04.unk0 = 0;
}

static void ending_hgate_main(void)
{
    struct SomeStruct *temp_r30 = &endingInfo.work->unkAA04;
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
            if (endingInfo.flags & 4)
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
            temp_r30->unk4 = 60;
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

static void ending_hgate_draw(void)
{
    struct GMAModel *model;
    struct SomeStruct *temp_r31 = &endingInfo.work->unkAA04;

    if (temp_r31->unk0 != 0)
    {
        model = decodedBgGma->modelEntries[STONE_GATE].model;
        mathutil_mtxA_from_mtxB_translate(&temp_r31->unk8);
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_unculled_sort_translucent(model);
        model = decodedBgGma->modelEntries[WALL_PARTS].model;
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
    struct SomeStruct *temp_r31 = &endingInfo.work->unkAA04;

    memset(temp_r31, 0, sizeof(*temp_r31));
    temp_r31->unk0 = 1;
    temp_r31->unk2 = 1;
    temp_r31->unk4 = arg0;
}

static void ending_chara_init(void)
{
    struct EndingWork *work = endingInfo.work;

    if (work != NULL)
    {
        work->characters[0].unk56 = -1;
        work->characters[1].unk56 = -1;
        work->characters[2].unk56 = -1;
        work->characters[3].unk56 = -1;
    }
}

static void ending_chara_main(void)
{
    int i;
    struct Struct801E2A98 spD0;
    struct Effect effect;
    struct RaycastHit floor;
    int playerId = modeCtrl.currPlayer;
    struct EndingSceneCharacter *chara;
    float t;

    chara = &endingInfo.work->characters[0];
    for (i = 0; i < 4; i++, chara++)
    {
        if (chara->ape != NULL)
        {
            switch (chara->state)
            {
            case 1:
                chara->state = 2;
                chara->ape->flags &= 0xFFFFFFDF;
                chara->unkA = 0;
                chara->unk30.x = 0;
                chara->unk30.y = 0;
                chara->unk30.z = 0;
                t = chara->timer;
                chara->pos.x = 0.0f;
                chara->pos.y = 20.0f - (0.5f * (-0.004f * (t * t)));
                chara->pos.z = -60.0f;
                chara->vel.x = 0.0f;
                chara->vel.y = 0.0f;
                chara->vel.z = 0.0f;
                break;
            case 2:
                chara->vel.y += -0.004f;
                chara->pos.y += chara->vel.y;
                chara->timer--;
                if (chara->timer <= 0)
                {
                    chara->state = 3;
                    chara->pos.x = 0.0f;
                    chara->pos.y = 20.0f;
                    chara->pos.z = -60.0f;
                }
                break;
            case 3:
                chara->vel.x *= 0.92f;
                chara->vel.y *= 0.92f;
                chara->vel.z *= 0.92f;
                chara->vel.y += -0.004f;
                chara->vel.x += 0.05f * -chara->pos.x;
                chara->vel.y += 0.05f * (20.0f - chara->pos.y);
                chara->vel.z += 0.05f * (-60.0f - chara->pos.z);
                chara->pos.x += chara->vel.x;
                chara->pos.y += chara->vel.y;
                chara->pos.z += chara->vel.z;
                break;
            case 4:
                chara->state = 5;
                chara->timer = 0;
                chara->vel.z = 0.0f;
                /* fallthrough */
            case 5:
                chara->vel.z += 0.000075f * chara->timer;
                chara->pos.z += chara->vel.z;
                chara->timer++;
                break;
            case 6:
                chara->state = 7;
                chara->ape->flags &= 0xFFFFFFDF;
                chara->unkA = 3;
                chara->unk30.x = 0;
                chara->unk30.y = RAND_FLOAT();
                chara->unk30.z = 0;
                chara->pos.x = 0.0f;
                chara->pos.y = 0.0f;
                chara->pos.z = 0.0f;
                chara->vel.x = 0.0f;
                chara->vel.y = 0.0f;
                chara->vel.z = 0.0f;
                if (modeCtrl.playerCount == 1)
                    chara->unk56 = 3;
                else
                    chara->unk56 = modeCtrl.currPlayer;
                chara->unk58 = rand() & 0x7FFF;
                chara->unk5A = rand() & 0x7FFF;
                chara->unk5C = rand() & 0x7FFF;
                /* fallthrough */
            case 7:
                if (chara->timer > 0)
                {
                    chara->timer--;
                    chara->vel.y += 0.009799992;
                    memset(&effect, 0, sizeof(effect));
                    effect.type = ET_LEVITATE;
                    effect.playerId = playerId;
                    effect.pos = chara->pos;
                    spawn_effect(&effect);
                }
                else
                {
                    chara->vel.x *= 0.92f;
                    chara->vel.y *= 0.92f;
                    chara->vel.z *= 0.92f;
                }
                chara->pos.x += chara->vel.x;
                chara->pos.y += chara->vel.y;
                chara->pos.z += chara->vel.z;
                chara->unk30.y -= 0x30;
                chara->unk5A -= 0x40;
                break;
            case 8:
                chara->vel.y += 0.009799992;
                chara->pos.y += chara->vel.y;
                chara->timer--;
                if (chara->timer < 0)
                {
                    chara->state = 0;
                    chara->ape->flags |= 0x20;
                }
                memset(&effect, 0, sizeof(effect));
                effect.type = ET_LEVITATE;
                effect.playerId = playerId;
                effect.pos = chara->pos;
                spawn_effect(&effect);
                break;
            case 9:
                chara->state = 0xA;
                chara->ape->flags &= 0xFFFFFFDF;
                chara->unkA = 0x12;
                chara->unk30.x = 0;
                chara->unk30.y = i * 0x10000 / 3;
                chara->unk30.z = 0;
                mathutil_mtxA_from_rotate_y(chara->unk30.y + 0x8000);
                mathutil_mtxA_tf_vec_xyz(&chara->pos, 0.0f, 0.0f, -5.0f);
                chara->pos.y += 30.0f + (5.0f * RAND_FLOAT());
                chara->vel.x = 0.0f;
                chara->vel.y = -0.20000000298023224f;
                chara->vel.z = 0.0f;
                if (i == playerCharacterSelection[playerId])
                {
                    if (modeCtrl.playerCount == 1)
                        chara->unk56 = 3;
                    else
                        chara->unk56 = i;
                }
                else
                    chara->unk56 = rand() & 3;
                chara->unk58 = rand() & 0x7FFF;
                chara->unk5A = rand() & 0x7FFF;
                chara->unk5C = rand() & 0x7FFF;
                /* fallthrough */
            case 10:
                raycast_stage_down(&chara->pos, &floor, NULL);
                if (chara->vel.y < 0.0f
                 && (0.5f + (chara->pos.y - floor.pos.y) < -15.0f * chara->vel.y))
                    chara->vel.y += 0.012000000104308128f;
                memset(&effect, 0, sizeof(effect));
                effect.type = ET_LEVITATE;
                effect.playerId = playerId;
                effect.pos = chara->pos;
                spawn_effect(&effect);
                /* fallthrough */
            case 11:
                raycast_stage_down(&chara->pos, &floor, NULL);
                chara->vel.x *= 0.99f;
                chara->vel.y *= 0.99f;
                chara->vel.z *= 0.99f;
                chara->vel.y += -0.0080000003799796104f;
                chara->pos.x += chara->vel.x;
                chara->pos.y += chara->vel.y;
                chara->pos.z += chara->vel.z;
                if (floor.pos.y > chara->pos.y - 0.5f - 0.1f)
                {
                    if (chara->vel.y < -0.10000000149011612f)
                        u_play_sound_0(0x69);
                    chara->pos.y = 0.5f + floor.pos.y - 0.1f;
                    if (chara->vel.y < 0.0f)
                        chara->vel.y *= -0.60000002384185791f;
                    if (chara->state != 0xB)
                    {
                        chara->state = 0xB;
                        memset(&effect, 0, sizeof(effect));
                        effect.playerId = playerId;
                        effect.type = ET_COLIFLASH;
                        effect.pos = chara->pos;
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
                chara->state = 0;
                chara->unkA = 0xC;
                chara->pos = lbl_801E2AD4[i];
                if (raycast_stage_down(&chara->pos, &floor, NULL) != 0U)
                {
                    chara->pos = floor.pos;
                    chara->pos.y += 0.4f;
                }
                chara->unk30.x = 0;
                chara->unk30.y = mathutil_atan2(chara->pos.x, chara->pos.z);
                chara->unk30.z = 0;
                chara->unk56 = -1;
                break;
            case 17:
                chara->state = 0x12;
                chara->unkA = 0xD;
                if (playerCharacterSelection[playerId] == 3)
                    chara->pos = lbl_801E2AD4_2[i];
                else
                    chara->pos = lbl_801E2AD4_2[(i + 3 - playerCharacterSelection[playerId]) % 3];
                if (raycast_stage_down(&chara->pos, &floor, NULL) != 0U)
                {
                    chara->pos = floor.pos;
                    chara->pos.y += 0.4f;
                }
                chara->unk30.x = 0;
                chara->unk30.y = 0;
                chara->unk30.z = 0;
                t = 1.0f / modeCtrl.submodeTimer;
                chara->vel.x = -chara->pos.x * t;
                chara->vel.y = 0.0f;
                chara->vel.z = -4.5f * t * (1.0f + (0.1f * RAND_FLOAT()));
                break;
            case 18:
                chara->pos.x += chara->vel.x;
                chara->pos.y += chara->vel.y;
                chara->pos.z += chara->vel.z;
                break;
            case 19:
                chara->state = 0x14;
                chara->timer = modeCtrl.submodeTimer;
                chara->unkA = 0xD;
                chara->pos = lbl_801E2AD4_2[i];
                chara->pos.y = 0.0f;
                chara->unk30.x = 0;
                chara->unk30.y = 0;
                chara->unk30.z = 0;
                t = 1.0f / chara->timer;
                chara->vel.x = -chara->pos.x * t;
                chara->vel.y = 0.0f;
                chara->vel.z = (3.45f - chara->pos.z) * t;
                break;
            case 20:
                chara->pos.x += chara->vel.x;
                chara->pos.y += chara->vel.y;
                chara->pos.z += chara->vel.z;
                if (chara->pos.z < 3.45f)
                {
                    chara->state = 0;
                    chara->unkA = 9;
                    chara->pos.z = 3.45f;
                }
                break;
            case 21:
                chara->state = 0;
                chara->unkA = 0xE;
                spD0 = lbl_801E2A98[i];
                chara->pos = spD0.unk0;
                chara->unk30 = spD0.unkC;
                switch (i)
                {
                case 0:
                    chara->model = decodedBgGma->modelEntries[AIAI_BNN].model;
                    chara->unk40 = 0xA;
                    chara->unk44.x = 0.05f;
                    chara->unk44.y = 0.12999999523162842f;
                    chara->unk44.z = 0.12000000476837158f;
                    chara->unk50 = -0x58E3;
                    chara->unk52 = -0x1555;
                    chara->unk54 = 0x438E;
                    break;
                case 1:
                    chara->model = decodedBgGma->modelEntries[GAL_BNN].model;
                    chara->unk40 = 0xF;
                    chara->unk44.x = 0.05f;
                    chara->unk44.y = 0.0f;
                    chara->unk44.z = -0.094000004231929779f;
                    chara->unk50 = 0x20B7;
                    chara->unk52 = -0x6BE1;
                    chara->unk54 = -0x26E9;
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
    struct EndingSceneCharacter *chara;

    if (endingInfo.work == NULL)
        return;
    chara = &endingInfo.work->characters[3];
    for (i = 3; i >= 0; i--, chara--)
    {
        if (chara->ape != NULL)
        {
            thread_kill(apeThreadNo[i]);
            chara->ape = NULL;
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
    struct EndingSceneCharacter *unk8;
};

static void lbl_800BCD30(struct MyDrawNode *);

static void ending_chara_draw(void)
{
    Vec *new_var;
    f32 temp_f21 ;
    f32 temp_f0;
    f32 temp_f4;
    s32 var_r28;
    struct EndingSceneCharacter *chara;
    struct Ape *temp_r26;
    struct Blah *var_r25;
    struct GMAModel *model;
    struct OrdTblNode *entry;
    struct MyDrawNode *node;
    Mtx sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;

    mathutil_mtxA_from_rotate_y(g_bgLightInfo.infLightRotY);
    mathutil_mtxA_rotate_x(g_bgLightInfo.infLightRotX);
    mathutil_mtxA_tf_vec_xyz(&sp2C, 0.0f, 0.0f, -1.0f);

    chara = &endingInfo.work->characters[0];
    for (var_r28 = 4; var_r28 > 0; var_r28--, chara++)
    {
        temp_f0 = temp_f21 + -1.0f;  // needed to match

        temp_r26 = chara->ape;
        if (temp_r26 == NULL)
            continue;

        model = chara->model;
        if (model != NULL)
        {
            mathutil_mtxA_from_quat(&temp_r26->unk60);
            mathutil_mtxA_to_mtx(sp38);
            mathutil_mtxA_from_mtxB_translate(&temp_r26->pos);
            mathutil_mtxA_scale_s(temp_r26->modelScale);
            mathutil_mtxA_translate(&temp_r26->unk3C);
            mathutil_mtxA_mult_right(sp38);
            mathutil_mtxA_mult_right(temp_r26->unk0->joints[chara->unk40].transformMtx);
            mathutil_mtxA_translate(&chara->unk44);
            mathutil_mtxA_rotate_z(chara->unk54);
            mathutil_mtxA_rotate_y(chara->unk52);
            mathutil_mtxA_rotate_x(chara->unk50);
            u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
            avdisp_set_bound_sphere_scale(temp_r26->modelScale);
            avdisp_draw_model_culled_sort_translucent(model);
        }
        if (chara->unk56 >= 0)
        {
            mathutil_mtxA_from_mtxB();
            entry = ord_tbl_get_entry_for_pos(&chara->pos);
            node = ord_tbl_alloc_node(sizeof(*node));
            node->node.drawFunc = (OrdTblDrawFunc)lbl_800BCD30;
            node->unk8 = chara;
            ord_tbl_insert_node(entry, &node->node);
        }
        if (chara->unk36 & 1)
        {
            sp20 = chara->pos;
            mathutil_mtxA_from_quat(&temp_r26->unk60);
            mathutil_mtxA_to_mtx(sp38);
            mathutil_mtxA_from_translate(&temp_r26->pos);
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
                        avdisp_draw_model_culled_sort_all(commonGma->modelEntries[polyshadow01].model);
                    }
                    mathutil_mtxA_pop();
                }
                var_r25++;
            }
            fade_color_base_default();
            avdisp_set_z_mode(1, GX_LEQUAL, 1);
        }
    }
}

static void lbl_800BCD30(struct MyDrawNode *node)
{
    struct EndingSceneCharacter *chara = node->unk8;
    s16 *r29 = coloredBallPartModelIDs[chara->unk56];
    struct GMAModelEntry *modelEntries = commonGma->modelEntries;

    mathutil_mtxA_from_mtxB();
    load_light_group_uncached(0);
    mathutil_mtxA_translate_xyz(chara->pos.x, chara->pos.y + 0.1f, chara->pos.z);
    mathutil_mtxA_rotate_y(chara->unk5A);
    mathutil_mtxA_rotate_x(chara->unk58);
    mathutil_mtxA_rotate_z(chara->unk5C);
    u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
    avdisp_set_z_mode(1, 3, 0);
    avdisp_draw_model_unculled_sort_none(modelEntries[clearHemisphereInsideParts[0]].model);
    avdisp_draw_model_unculled_sort_none(modelEntries[r29[0]].model);
    avdisp_draw_model_unculled_sort_none(modelEntries[r29[6]].model);
    avdisp_set_z_mode(1, 3, 1);
    avdisp_draw_model_unculled_sort_none(modelEntries[clearHemisphereOutsideParts[0]].model);
    avdisp_draw_model_unculled_sort_none(modelEntries[r29[3]].model);
    avdisp_set_z_mode(1, 3, 1);
}

static int set_ending_chara(int apeId, enum Character charaId)
{
    struct EndingSceneCharacter *chara;
    struct Ape *ape;

    chara = &endingInfo.work->characters[apeId];
    if (chara->ape != NULL)
    {
        thread_kill(apeThreadNo[apeId]);
        chara->ape = NULL;
    }

    if (charaId >= 0)
    {
        chara->ape = ape_get_by_type(apeId, charaId, ending_ape_thread);
        ape = chara->ape;
        if (ape == NULL)
            return 0;
        ape->unk0->u_someDeltaTime = 1.0f;
        ape->flags |= 0x20;
        chara->unk8 = 0xC;
        chara->unkA = -1;
        mathutil_mtxA_from_identity();
        mathutil_mtxA_get_translate_alt2(&ape->pos);
        mathutil_mtxA_to_quat(&ape->unk60);
    }
    return 1;
}

static void ending_ape_thread(struct Ape *ape, int status)
{
    int var_r4;
    struct EndingSceneCharacter *chara;
    int i;
    int done = (status == THREAD_STATUS_KILLED);
    Vec sp10;

    if (done)
    {
        ape_destroy(ape);
        if (status != THREAD_STATUS_KILLED)
            thread_exit();
        return;
    }
    if (debugFlags & 0xA)
        return;

    chara = &endingInfo.work->characters[0];
    for (var_r4 = 4; var_r4 > 0; var_r4--, chara++)
    {
        if (chara->ape == ape)
            break;
    }

    if (var_r4 == 0)
        return;
    switch (ape->charaId)
    {
    case 3:
        switch (chara->unk8)
        {
        case 9:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 8;
            break;
        case 8:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 0xA;
            break;
        case 10:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 0xE;
            break;
        case 14:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 0xB;
            break;
        case 11:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 0xF;
            break;
        case 15:
            if (chara->unkA != -1 && ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
            {
                chara->unk8 = chara->unkA;
                chara->unkA = -1;
            }
            break;
        case 17:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 0x10;
            break;
        default:
            if (chara->unkA != -1)
            {
                chara->unk8 = chara->unkA;
                chara->unkA = -1;
            }
            break;
        }
        break;
    default:
        switch (chara->unk8)
        {
        case 14:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 0xF;
            break;
        case 15:
            if (ape->unk0->u_currKeyframe == ape->unk0->u_keyframeCount - 1)
                chara->unk8 = 0xE;
            if (chara->unkA != -1)
            {
                chara->unk8 = chara->unkA;
                chara->unkA = -1;
            }
            break;
        default:
            if (chara->unkA != -1)
            {
                chara->unk8 = chara->unkA;
                chara->unkA = -1;
            }
            break;
        }
    }

    if (chara->unk8 < 0x12)
        new_ape_stat_motion(ape, 7, chara->unk8, 0, 0.0f);
    else
    {
        int temp_r5_3 = chara->unk8 - 0x12;
        new_ape_stat_motion(ape, 1, temp_r5_3, temp_r5_3 + 1, 0.0f);
    }
    ape_skel_anim_main(ape);
    switch (chara->unk70)
    {
    case 2:
        ape_face_dir(ape, &chara->unk74);
        break;
    case 1:
        mathutil_mtxA_from_mtx(cameraInfo[modeCtrl.currPlayer].unk144);
        mathutil_mtxA_rigid_inv_tf_tl(&sp10);
        ape_face_dir(ape, &sp10);
        break;
    }

    mathutil_mtxA_from_translate(&chara->pos);
    mathutil_mtxA_rotate_z(chara->unk30.z);
    mathutil_mtxA_rotate_y(chara->unk30.y - 0x4000);
    mathutil_mtxA_rotate_x(chara->unk30.x);
    mathutil_mtxA_get_translate_alt2(&ape->pos);
    mathutil_mtxA_to_quat(&ape->unk60);

    switch (ape->charaId)
    {
    case 0:
        switch (ape->unk9C)
        {
        case 14:
            if (endingInfo.flags & 0x10)
            {
                if (ape->unk0->u_currKeyframe == 0x16)
                    SoundReq(0xB3U);
                else if (ape->unk0->u_currKeyframe == 0x44)
                    SoundReq(0xE8U);
            }
            break;
        }
        break;
    case 1:
        switch (ape->unk9C)
        {
        case 15:
            if (endingInfo.flags & 0x10)
            {
                if (ape->unk0->u_currKeyframe == 0xC)
                    SoundReq(0x105U);
                else if (ape->unk0->u_currKeyframe == 0x50 || ape->unk0->u_currKeyframe == 0x66)
                    SoundReq(0x12FU);
                else if (ape->unk0->u_currKeyframe == 0x76)
                    u_play_sound_0(0x1FF);
            }
            break;
        case 14:
            if ((endingInfo.flags & 0x10) && ape->unk0->u_currKeyframe == 0x12)
                SoundReq(0xFCU);
            break;
        }
        break;
    case 2:
        switch (ape->unk9C)
        {
        case 14:
            if ((endingInfo.flags & 0x10) && ape->unk0->u_currKeyframe == 0x24)
                SoundReq(0x16BU);
            break;
        }
        break;
    }
}

static void ending_banana_init(void)
{
    int i;
    struct EndingSceneObject *obj;

    obj = endingInfo.work->objects;
    for (i = 0; i < 512; i++, obj++)
        obj->isActive = FALSE;
}

static void ending_banana_main(void)
{
    Vec spFC;
    Vec spF0;
    Vec spE4;
    struct Effect effect;
    float temp_f0_5;
    float temp_f1;
    float temp_f27;
    float temp_f26;
    float alt_f26;
    float alt_f27;
    int var_r30;
    struct EndingSceneObject *obj;
    struct Camera *camera;
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

    ending_banana_each_coli();
    camera = &cameraInfo[modeCtrl.currPlayer];
    mathutil_mtxA_from_mtx(camera->unk144);
    mathutil_mtxA_rigid_inv_tf_vec_xyz(&spF0, 0.0f, 0.0f, -1.0f);
    mathutil_mtxA_rigid_inv_tf_tl(&spE4);

    obj = endingInfo.work->objects;
    for (var_r30 = 512; var_r30 > 0; var_r30--, obj++)
    {
        if (obj->isActive)
        {
            temp_r27 = obj->model;
            switch (obj->unk2)
            {
            case 2:
                alt_f26 = obj->pos.y;
                mathutil_mtxA_from_rotate_y((globalAnimTimer << 0xA) + var_r30);
                alt_f27 = 0.25f * alt_f26 * endingInfo.work->unk0;
                mathutil_mtxA_tf_vec_xyz(&spFC, alt_f27, 0.0f, 0.0f);
                mathutil_mtxA_from_translate(&spFC);
                alt_f27 = 1024.f * alt_f26 * endingInfo.work->unk0;
                mathutil_mtxA_rotate_x(alt_f27 * mathutil_sin((globalAnimTimer << 0xB) + var_r30));
                mathutil_mtxA_rotate_z(alt_f27 * mathutil_sin((globalAnimTimer << 0xB) + var_r30 + 0x4000));
                mathutil_mtxA_tf_point(&obj->unk4C->unk4, &spFC);
                if (spFC.y < obj->unk4C->unk4.y)
                    spFC.y = obj->unk4C->unk4.y;
                obj->vel.x = 0.1f * (spFC.x - obj->pos.x);
                obj->vel.y = 0.1f * (spFC.y - obj->pos.y);
                obj->vel.z = 0.05f * (spFC.z - obj->pos.z);
                obj->unk36 = 0;
                obj->unk38 = 0;
                obj->unk3A = 0;
                if (obj->unk4 != 0)
                {
                    obj->unk4--;
                    if (obj->unk4 == 0)
                    {
                        obj->unk2 = 3;
                        obj->unk8 &= 0xFFFFFFFC;
                        obj->unk8 |= 0x34;
                        if (var_r30 & 1)
                            obj->unk8 |= 8;
                        mathutil_vec_set_len(&obj->pos, &spFC, 0.001f + (0.05f * RAND_FLOAT()));
                        obj->vel.x += spFC.x;
                        obj->vel.y += 0.05f + (4.0f * spFC.y);
                        obj->vel.z += spFC.z;
                        obj->unk36 += 2048.0f * (RAND_FLOAT() - 0.5f);
                        obj->unk38 += 2048.0f * (RAND_FLOAT() - 0.5f);
                        obj->unk3A += 2048.0f * (RAND_FLOAT() - 0.5f);
                    }
                }
                break;
            case 4:
                obj->unk2 = 5;
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
                obj->pos.x = spE4.x + spFC.x;
                obj->pos.y = spE4.y + spFC.y;
                obj->pos.z = spE4.z + spFC.z;
                obj->vel.x = 0.2f * (RAND_FLOAT() - 0.5f);
                obj->vel.y = -0.2f * RAND_FLOAT();
                obj->vel.z = 0.2f * (RAND_FLOAT() - 0.5f);
                obj->unk30.x = rand() & 0x7FFF;
                obj->unk30.y = rand() & 0x7FFF;
                obj->unk30.z = rand() & 0x7FFF;
                obj->unk36 = 2048.0f * (RAND_FLOAT() - 0.5f);
                obj->unk38 = 2048.0f * (RAND_FLOAT() - 0.5f);
                obj->unk3A = 2048.0f * (RAND_FLOAT() - 0.5f);
                obj->unk24.x = 0.3f * (1.0f + RAND_FLOAT());
                obj->unk24.y = obj->unk24.x;
                obj->unk24.z = obj->unk24.x;
                obj->unk48 = 0.0f;
                /* fall through */
            case 5:
                mathutil_mtxA_from_mtx(camera->unk144);
                mathutil_mtxA_tf_point(&obj->pos, &spFC);
                if (spFC.z > 5.f)
                {
                    obj->unk2 = 4;
                    obj->unk48 = 0.0f;
                }
                else if (mathutil_vec_len(&spFC) < 80.0f)
                {
                    obj->unk48 += 0.1f * (1.1f - obj->unk48);
                }
                else
                {
                    obj->unk48 += 0.15f * (-0.1f - obj->unk48);
                    if (obj->unk48 < 0.0f)
                    {
                        obj->unk2 = 4;
                        obj->unk48 = 0.0f;
                    }
                }
                break;
            case 6:
                obj->unk2 = 7;
                obj->unk8 |= 3;
                obj->unk48 = 0.0f;
                obj->unk24.x = 2.0f * (1.0f + (0.5f * RAND_FLOAT()));
                obj->unk24.y = obj->unk24.x;
                obj->unk24.z = obj->unk24.x;
                obj->unk30.x = rand() & 0x7FFF;
                obj->unk30.y = rand() & 0x7FFF;
                obj->unk30.z = rand() & 0x7FFF;
                obj->unk36 += 4096.0f * (RAND_FLOAT() - 0.5f);
                obj->unk38 += 4096.0f * (RAND_FLOAT() - 0.5f);
                obj->unk3A += 4096.0f * (RAND_FLOAT() - 0.5f);
                alt_f27 = obj->unk4;
                obj->vel.x = 0.0f;
                obj->vel.y = -0.2f * RAND_FLOAT();
                obj->vel.z = 0.0f;
                obj->unk3C = RAND_FLOAT() - 0.5f;
                obj->unk40 = -0.5f * (alt_f27 * ((2.0f * obj->vel.y) + (-0.008f * alt_f27)));
                obj->unk44 = RAND_FLOAT() - 0.5f;
                /* fall through */
            case 7:
                obj->unk36 = 0.99f * obj->unk36;
                obj->unk38 = 0.99f * obj->unk38;
                obj->unk3A = 0.99f * obj->unk3A;
                obj->unk30.x += obj->unk36;
                obj->unk30.y += obj->unk38;
                obj->unk30.z += obj->unk3A;
                obj->vel.y += -0.008f;
                obj->unk40 += obj->vel.y;
                if (obj->unk40 <= 0.0f)
                {
                    spFC.x = obj->vel.x;
                    spFC.y = obj->vel.y;
                    spFC.z = obj->vel.z;
                    shake_camera(modeCtrl.currPlayer, 0xF, &spFC);
                    obj->unk40 = 0.0f;
                    if (obj->vel.y < 0.0f)
                        obj->vel.y *= -0.125f;
                    obj->unk36 += 4096.0f * (RAND_FLOAT() - 0.5f);
                    obj->unk38 += 4096.0f * (RAND_FLOAT() - 0.5f);
                    obj->unk3A += 4096.0f * (RAND_FLOAT() - 0.5f);
                }
                obj->pos.x = camera->eye.x + obj->unk3C;
                obj->pos.y = obj->unk40 + (camera->eye.y + (3.2000000476837158f * obj->unk24.x));
                obj->pos.z = camera->eye.z + obj->unk44;
                obj->unk48 += 0.1f * (1.1f - obj->unk48);
                break;
            case 8:
                obj->unk2 = 9;
                obj->unk8 = 67;
                obj->unk48 = 0.0f;
                obj->unk24.x = 0.4f;
                obj->unk24.y = obj->unk24.x;
                obj->unk24.z = obj->unk24.x;
                obj->unk30.x = rand() & 0x7FFF;
                obj->unk30.y = rand() & 0x7FFF;
                obj->unk30.z = rand() & 0x7FFF;
                obj->unk36 += 256.0f * (RAND_FLOAT() - 0.5f);
                obj->unk38 += 16.0f * (RAND_FLOAT() - 0.5f);
                obj->unk3A += 4096.0f * (0.5f + RAND_FLOAT());
                obj->pos = endingInfo.work->unkAA54.unk2C;
                /* fall through */
            case 9:
                if (obj->unk4 != 0)
                    obj->unk4--;
                endingInfo.work->unkAA54.unk2C = obj->pos;
                if (obj->unk4 < 60)
                {
                    temp_f0_5 = 1.0f / (obj->unk4 + 1);
                    obj->vel.x = (0.15016f - obj->pos.x) * temp_f0_5;
                    obj->vel.y = (20.07f - obj->pos.y) * temp_f0_5;
                    obj->vel.z = (-60.0f - obj->pos.z) * temp_f0_5;
                    obj->pos.x += obj->vel.x;
                    obj->pos.y += obj->vel.y;
                    obj->pos.z += obj->vel.z;
                    if (obj->unk4 == 0)
                    {
                        obj->unk36 = 0.92f * obj->unk36;
                        obj->unk38 = 0.92f * obj->unk38;
                        obj->unk3A = 0.92f * obj->unk3A;
                        obj->unk36 += -0.1f * obj->unk30.x;
                        obj->unk38 += -0.1f * obj->unk30.y;
                        obj->unk3A += -0.1f * obj->unk30.z;
                    }
                    obj->unk30.x += obj->unk36;
                    obj->unk30.y += obj->unk38;
                    obj->unk30.z += obj->unk3A;
                }
                else
                {
                    obj->vel.x = 0.025f * (0.15016f - obj->pos.x);
                    obj->vel.y = 0.025f * (20.07f - obj->pos.y);
                    obj->vel.z = 0.0f;
                    obj->pos.x += obj->vel.x;
                    obj->pos.y += obj->vel.y;
                    obj->pos.z += obj->vel.z;
                    obj->unk30.x += obj->unk36;
                    obj->unk30.y += obj->unk38;
                    obj->unk30.z += obj->unk3A;
                    memset(&effect, 0, sizeof(effect));
                    effect.type = ET_LEVITATE;
                    effect.playerId = modeCtrl.currPlayer;
                    effect.pos = obj->pos;
                    spawn_effect(&effect);
                }
                obj->unk48 += (0.1f * (1.1f - obj->unk48));
                break;
            case 10:
                {
                    struct EndingWork *work = endingInfo.work;
                    obj->pos.x = 0.15016f + work->characters[0].pos.x;
                    obj->pos.y = 0.07f + work->characters[0].pos.y;
                    obj->pos.z = work->characters[0].pos.z;
                    obj->unk30 = work->characters[0].unk30;
                }
                break;
            }
            if (!(obj->unk8 & 2))
            {
                obj->vel.x *= 0.99f;
                obj->vel.y *= 0.99f;
                obj->vel.z *= 0.99f;
                obj->unk36 = 0.984375f * obj->unk36;
                obj->unk38 = 0.984375f * obj->unk38;
                obj->unk3A = 0.984375f * obj->unk3A;
            }
            if (!(obj->unk8 & 1))
            {
                obj->vel.y += -0.008f;
                obj->pos.x += obj->vel.x;
                obj->pos.y += obj->vel.y;
                obj->pos.z += obj->vel.z;
                obj->unk30.x += obj->unk36;
                obj->unk30.y += obj->unk38;
                obj->unk30.z += obj->unk3A;
            }
            if (obj->unk8 & 0x20)
            {
                temp_f26 = temp_r27->unk14 * obj->unk24.x;
                temp_f27 = 2.0f + temp_f26;
                if (mathutil_vec_sq_len(&obj->pos) < temp_f27 * temp_f27)
                {
                    mathutil_vec_set_len(&obj->pos, &spFC, temp_f27);
                    spFC.x -= obj->pos.x;
                    spFC.y -= obj->pos.y;
                    spFC.z -= obj->pos.z;
                    obj->pos.x += 0.025f * spFC.x;
                    obj->pos.y += 0.025f * spFC.y;
                    obj->pos.z += 0.025f * spFC.z;
                    obj->vel.x += 0.01f * spFC.x;
                    obj->vel.y += 0.01f * spFC.y;
                    obj->vel.z += 0.01f * spFC.z;
                }

                spFC.x = obj->pos.x;
                spFC.y = obj->pos.y - 1.0f;
                spFC.z = obj->pos.z - 3.0f;
                temp_f27 = 1.5f + temp_f26;
                if (mathutil_vec_sq_len(&spFC) < temp_f27 * temp_f27)
                {
                    mathutil_vec_set_len(&spFC, &spFC, temp_f27);
                    obj->pos.x += 0.05f * spFC.x;
                    obj->pos.y += 0.05f * spFC.y;
                    obj->pos.z += 0.05f * spFC.z;
                    obj->vel.x += 0.01f * spFC.x;
                    obj->vel.y += 0.01f * spFC.y;
                    obj->vel.z += 0.01f * spFC.z;
                }
                temp_f27 = 1.0f + temp_f26;
                {
                    struct EndingWork *work = endingInfo.work;
                    spFC.x = obj->pos.x - work->characters[3].pos.x;
                    spFC.y = obj->pos.y - work->characters[3].pos.y;
                    spFC.z = obj->pos.z - work->characters[3].pos.z;
                }
                if (mathutil_vec_sq_len(&spFC) < (temp_f27 * temp_f27))
                {
                    mathutil_vec_set_len(&spFC, &spFC, temp_f27);
                    obj->pos.x += 0.05f * spFC.x;
                    obj->pos.y += 0.05f * spFC.y;
                    obj->pos.z += 0.05f * spFC.z;
                    obj->vel.x += 0.01f * spFC.x;
                    obj->vel.y += 0.01f * spFC.y;
                    obj->vel.z += 0.01f * spFC.z;
                }
            }
            if (obj->unk8 & 0x10)
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
                mathutil_mtxA_from_rotate_y(obj->unk30.y);
                mathutil_mtxA_rotate_x(obj->unk30.x);
                mathutil_mtxA_rotate_z(obj->unk30.z);
                mathutil_mtxA_tf_vec_xyz(&spFC, 0.70710677f, 0.70710677f, 0.0f);
                f = g * obj->unk24.x * (0.30000001192092896 + 0.69999998807907104 * fabs(spFC.y));
                if (obj->pos.y < sp8.sp20.y + f)
                {
                    obj->pos.y = sp8.sp20.y + f;
                    temp_f26 = mathutil_vec_dot_prod(&sp8.sp2C, &obj->vel);
                    if (temp_f26 < 0.0f)
                    {
                        obj->unk36 = obj->unk36 * 0.96875f;
                        obj->unk38 = obj->unk38 * 0.96875f;
                        obj->unk3A = obj->unk3A * 0.96875f;
                        mathutil_mtxA_rigid_inv_tf_vec(&sp8.sp2C, &spFC);
                        sp8.unk0 = (256.0 + (256.0 * fabs(temp_f26))) * mathutil_cos(((s16)(((var_r30 * 0x800) & 0x1FFF) - 0xFFF) + mathutil_atan2(-spFC.z, spFC.y)));
                        obj->unk36 += sp8.unk0;
                        if (temp_f26 < -0.1f)
                        {
                            temp_f27 = 16384.0f * (0.1f + temp_f26);
                            obj->unk36 += temp_f27 * (RAND_FLOAT() - 0.5f);
                            obj->unk38 += temp_f27 * (RAND_FLOAT() - 0.5f);
                            obj->unk3A += temp_f27 * (RAND_FLOAT() - 0.5f);
                        }
                        spFC = obj->vel;
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
                        obj->vel = spFC;
                    }
                }
            }
        }
    }
}

static void ending_banana_each_coli(void)
{
    float unused_f31;
    float unused_f30;

    int i;
    int j;
    struct EndingSceneObject *objA;
    struct EndingSceneObject *objB;

    // useless, but needed to match
    unused_f30 = 0.5f;
    unused_f31 = 0.25f;

    objA = endingInfo.work->objects;
    for (i = 512; i > 0; i--, objA++)
    {
        if (objA->isActive && (objA->unk8 & 8))
        {
            float temp_f29 = 0.5f * (objA->model->unk14 * objA->unk24.x);
            Vec sp38 = objA->pos;

            objB = objA + 1;
            for (j = i - 1; j > 0; j--, objB++)
            {
                if (objB->isActive && (objB->unk8 & 8))
                {
                    float temp_f28 = 0.5f * (objB->model->unk14 * objB->unk24.x);
                    float temp_f27 = temp_f29 + temp_f28;
                    Vec sp2C = objB->pos;
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
                        sp8.x = sp14.x + (temp_f29 * sp20.x) - sp38.x;
                        sp8.y = sp14.y + (temp_f29 * sp20.y) - sp38.y;
                        sp8.z = sp14.z + (temp_f29 * sp20.z) - sp38.z;
                        sp38.x += 0.5f * sp8.x;
                        sp38.y += 0.5f * sp8.y;
                        sp38.z += 0.5f * sp8.z;
                        objA->vel.x += sp8.x * temp_f0_4;
                        objA->vel.y += sp8.y * temp_f0_4;
                        objA->vel.z += sp8.z * temp_f0_4;
                        temp_f0_4 = 0.25f / temp_f28;
                        sp8.x = sp14.x - (temp_f28 * sp20.x) - sp2C.x;
                        sp8.y = sp14.y - (temp_f28 * sp20.y) - sp2C.y;
                        sp8.z = sp14.z - (temp_f28 * sp20.z) - sp2C.z;
                        sp2C.x += 0.5f * sp8.x;
                        sp2C.y += 0.5f * sp8.y;
                        sp2C.z += 0.5f * sp8.z;
                        objB->vel.x += sp8.x * temp_f0_4;
                        objB->vel.y += sp8.y * temp_f0_4;
                        objB->vel.z += sp8.z * temp_f0_4;
                        objB->pos = sp2C;
                    }
                }
            }
            objA->pos = sp38;
        }
    }
}

static void func_800BF2D0(void) {}

static void ending_banana_draw(void)
{
    Vec sp14;
    Point3d sp8;
    float opacity;
    float scale;
    int isNaomiModel;
    int i;
    float radius;
    struct EndingSceneObject *obj;
    struct SomeBigEndingStruct_sub4_sub2 *model;

    obj = endingInfo.work->objects;
    for (i = 512; i > 0; i--, obj++)
    {
        if (obj->isActive)
        {
            model = obj->model;
            opacity = obj->unk48;
            if (opacity <= 0.0f)
                continue;

            sp14 = obj->unk24;
            mathutil_mtxA_from_mtxB_translate(&obj->pos);
            mathutil_mtxA_rotate_y(obj->unk30.y);
            mathutil_mtxA_rotate_x(obj->unk30.x);
            mathutil_mtxA_rotate_z(obj->unk30.z);
            mathutil_mtxA_scale(&sp14);
            scale = MAX(MAX(sp14.x, sp14.y), sp14.z);
            isNaomiModel = obj->unk8 & 0x40;
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
                if (obj->unk8 & 4)
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
                    nlSetScaleFactor(scale);
                    if (opacity < 1.0f)
                        nlObjPutTrnsl((struct NlModel *)model, opacity);
                    else
                        nlObjPut((struct NlModel *)model);
                }
            }
        }
    }
}

static void set_house_ending_bananas(void)
{
    int modelId;
    int i;
    int j;
    struct Stage_Child90 *var_r29;
    struct EndingSceneObject *obj;

    endingInfo.flags |= 1;
    var_r29 = decodedStageLzPtr->unk90;

    j = decodedStageLzPtr->unk8C;
    obj = endingInfo.work->objects;
    for (i = 512; i > 0 && j > 0; i--, obj++)
    {
        if (!obj->isActive)
        {
            obj->isActive = TRUE;
            obj->unk2 = 1;
            obj->pos = var_r29->unk4;
            obj->vel.x = 0.0f;
            obj->vel.y = 0.0f;
            obj->vel.z = 0.0f;
            obj->unk30.x = 0;
            obj->unk30.y = 0;
            obj->unk30.z = 0;
            mathutil_mtxA_from_rotate_z(var_r29->unk14);
            mathutil_mtxA_rotate_y(var_r29->unk12);
            mathutil_mtxA_rotate_x(var_r29->unk10);
            mathutil_mtxA_to_euler(&obj->unk30);
            obj->unk24 = var_r29->unk18;
            obj->unk48 = 1.0f;
            obj->unk4C = (struct SomeBigEndingStruct_sub4_sub *) var_r29;
            obj->unk8 = 3;
            if (!(var_r29->unk0 & 1))
                modelId = HOUSE_BANANA;
            else
                modelId = SEAL_BANANA;
            obj->model = (struct SomeBigEndingStruct_sub4_sub2 *) decodedBgGma->modelEntries[modelId].model;
            var_r29++;
            j--;
        }
    }
}

static void func_800BF658(void)
{
    int i;
    struct EndingSceneObject *obj;

    obj = endingInfo.work->objects;
    for (i = 512; i > 0; i--, obj++)
    {
        if (obj->isActive && obj->unk2 == 1)
        {
            obj->unk2 = 2;
            obj->unk4 = 0;
            obj->unk8 &= 0xFFFFFFFE;
            obj->unk8 |= 2;
        }
    }
}

static void func_800BF774(void)
{
    Vec sp8;
    float temp_f25;
    int i;
    struct EndingSceneObject *obj;

    mathutil_mtxA_from_rotate_y((rand() & 0x3FF) - 0x200);
    mathutil_mtxA_rotate_z((rand() & 0x7FFF) - 0x4000);
    mathutil_mtxA_tf_vec_xyz(&sp8, 0.0f, -1.0f, 0.0f);
    obj = endingInfo.work->objects;
    for (i = 512; i > 0; i--, obj++)
    {
        if (obj->isActive && obj->unk2 == 2 && obj->unk4 <= 0)
        {
            temp_f25 = 1.0f + mathutil_vec_dot_normalized_safe(&obj->pos, &sp8);
            obj->unk4 = 1.0f + (60.0f * temp_f25 * (0.9f + (0.2f * RAND_FLOAT())));
        }
    }
}

static void set_freefall_ending_bananas(int count)
{
    int modelId;
    int i;
    struct EndingSceneObject *obj;

    obj = endingInfo.work->objects;
    for (i = 512; i > 0 && count > 0; i--, obj++)
    {
        if (!obj->isActive)
        {
            obj->isActive = TRUE;
            obj->unk2 = 4;
            obj->unk8 = 0x14;
            if (i & 1)
                modelId = HOUSE_BANANA;
            else
                modelId = SEAL_BANANA;
            count--;
            obj->model = (struct SomeBigEndingStruct_sub4_sub2 *) decodedBgGma->modelEntries[modelId].model;
        }
    }
}

static void set_cam_ride_ending_bananas(int count, int arg1)
{
    int modelId;
    int i;
    struct EndingSceneObject *obj;

    obj = endingInfo.work->objects;
    for (i = 512; i > 0 && count > 0; i--, obj++)
    {
        if (!obj->isActive)
        {
            obj->isActive = TRUE;
            obj->unk2 = 6;
            obj->unk4 = arg1;
            if (i & 1)
                modelId = HOUSE_BANANA;
            else
                modelId = SEAL_BANANA;
            count--;
            obj->model = (struct SomeBigEndingStruct_sub4_sub2 *) decodedBgGma->modelEntries[modelId].model;
        }
    }
}

static void spawn_beginner_banana(int arg0)
{
    int i;
    struct EndingSceneObject *obj;

    obj = endingInfo.work->objects;
    for (i = 512; i > 0; i--, obj++)
    {
        if (!obj->isActive)
        {
            obj->isActive = TRUE;
            obj->unk2 = 8;
            obj->unk4 = arg0;
            obj->unk48 = 0.0f;
            obj->model = (void *)NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_OBJ_BANANA_ENDING);
            return;
        }
    }
}

static void func_800BFA2C(void)
{
    int i;
    struct EndingSceneObject *obj;

    obj = endingInfo.work->objects;
    for (i = 512; i > 0; i--, obj++)
    {
        if ((obj->unk2 == 8 || obj->unk2 == 9) && obj->isActive)
        {
            obj->unk2 = 10;
            obj->unk48 = 1.0f;
            return;
        }
    }
}

static void rend_efc_ape_face(void)
{
    GXTexObj texObj;
    GXColor kcolor;
    Point3d cameraPos;
    Vec upVec;
    Vec cameraTarget;
    Mtx sp14;
    struct Ape *ape;
    struct EndingSceneCharacter *chara;
    struct EndingWork *work;
    float x1, y1, x2, y2;

    work = endingInfo.work;
    if (work == NULL)
        return;
    if (work->unkAAA8 <= 0.0f)
        return;
    chara = &work->characters[0];
    ape = chara->ape;
    if (ape == NULL)
        return;

    GXSetTexCopySrc(0, 0, 640, 448);
    GXSetTexCopyDst(640, 448, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(lbl_802F1B40, 0U);
    GXInitTexObj(&texObj, lbl_802F1B40, 640, 448, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    mathutil_mtxA_from_quat(&ape->unk60);
    mathutil_mtxA_to_mtx(sp14);
    mathutil_mtxA_from_translate(&ape->pos);
    mathutil_mtxA_scale_s(ape->modelScale);
    mathutil_mtxA_translate(&ape->unk3C);
    mathutil_mtxA_mult_right(sp14);
    mathutil_mtxA_mult_right(ape->unk0->joints[5].transformMtx);
    mathutil_mtxA_get_translate_alt(&cameraTarget);
    mathutil_mtxA_tf_point_xyz(&cameraTarget, 0.0f, 0.0f, 0.1f);
    mathutil_mtxA_tf_point_xyz(&cameraPos, work->unkAAAC, 0.0f, 0.2f);
    upVec.x = 0.0f;
    upVec.y = 1.0f;
    upVec.z = 0.0f;
    MTXLookAt(mathutilData->mtxB, &cameraPos, &upVec, &cameraTarget);
    chara->ape->flags &= 0xFFFFFFDF;
    draw_monkey();
    chara->ape->flags |= 0x20;
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetNumTevStages_cached(1);
    GXLoadTexObj_cached(&texObj, GX_TEXMAP0);
    kcolor.r = 255;
    kcolor.g = 255;
    kcolor.b = 255;
    kcolor.a = work->unkAAA8 * 255.0f;
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
    y1 = -10.0f * currentCamera->sub28.unk38;
    y2 = -y1;
    x1 = -10.0f * (currentCamera->sub28.unk38 * currentCamera->sub28.aspect);
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
    set_current_camera(u_cameraId1);
}
