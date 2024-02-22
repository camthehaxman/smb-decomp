#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>
#include <dolphin/gx/GXEnum.h>

#include "global.h"
#include "adv.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "ending.h"
#include "event.h"
#include "gma.h"
#include "gxcache.h"
#include "gxutil.h"
#include "info.h"
#include "input.h"
#include "item.h"
#include "lens_flare.h"
#include "light.h"
#include "load.h"
#include "mathutil.h"
#include "minimap.h"
#include "mode.h"
#include "name_entry.h"
#include "nl2ngc.h"
#include "ord_tbl.h"
#include "polydisp.h"
#include "pool.h"
#include "rend_efc.h"
#include "shadow.h"
#include "sprite.h"
#include "stage.h"
#include "stobj.h"
#include "thread.h"
#include "window.h"
#include "world.h"

#define SCREEN_ASPECT (640.0f / 480.0f)

#include "../data/common.gma.h"
#include "../data/common.nlobj.h"

static struct
{
    float unk0;
    float unk4;
    float unk8;
    float unkC;
} lbl_801EEC80;

struct PolyDisp polyDisp;

static void draw_3d_scene(void);
static void draw_adv_3d_scene(void);
static void draw_intro_av_logo(void);
static void draw_adv_demo_scene(void);
static void u_draw_tutorial_button_and_joystick(void);
static void func_8000C388(void);
static void func_8000C7A4(void);
static void draw_live_arrow(void);
static void draw_continue_scene(void);
static void draw_extra_scene(void);
static void draw_results_scene(void);
static void draw_test_camera_target(void);
static void draw_timer_bomb_fuse(void);
static void set_backdrop_color(void);
static void func_8000E134(void);
static void func_8000E180(void);

void polydisp_init(void)
{
    u_init_bg_fog_params();
}

static inline void show_loading_msg(void)
{
    const char throbber[] = {'-', '\\', '|', '/'};
    int chr;
    int asterisks;

    window_set_cursor_pos(14, 15);
    chr = throbber[(globalAnimTimer/2) % 4];
    window_printf_2("%c", chr);
    u_debug_print("NOW LOADING");
    window_printf_2("%c", chr);
    window_set_cursor_pos(15, 16);
    u_debug_print("LEFT: ");
    asterisks = get_load_queue_count();
    while (asterisks > 0)
    {
        u_debug_print("*");
        asterisks--;
    }
}

void polydisp_main(void)
{
    if (gameMode == MD_SEL
     && (gameSubmode == SMD_SEL_STAGE_INIT || gameSubmode == SMD_SEL_STAGE_MAIN)
     && is_load_queue_not_empty())
        show_loading_msg();

    polyDisp.flags &= ~0x11;
    if (func_8009D5D8() != 0)
        polyDisp.flags |= 0x10;

    light_main();
    fog_main();

    if (eventInfo[EVENT_VIEW].state != EV_STATE_RUNNING)
        func_8000E134();

    set_backdrop_color();
    if (eventInfo[EVENT_VIEW].state != EV_STATE_RUNNING)
    {
        if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            rend_efc_draw(1);
        draw_3d_scene();
        if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            rend_efc_draw(2);
    }
    else
        view_draw();
    func_8000E180();
    if (eventInfo[EVENT_NAME_ENTRY].state == EV_STATE_RUNNING)
        draw_name_entry_hud();
    if (modeCtrl.unk30 <= 1
     && eventInfo[EVENT_VIEW].state == EV_STATE_INACTIVE
     && eventInfo[EVENT_MINIMAP].state == EV_STATE_RUNNING)
        minimap_draw();
    if (spriteClassMask & 4)
        draw_timer_bomb_fuse();
}

static void draw_3d_scene(void)
{
    ord_tbl_reset();
    u_draw_naomi_ball();
    switch (gameMode)
    {
    default:
        draw_test_camera_target();
        break;
    case MD_ADV:
        draw_adv_3d_scene();
        break;
    case MD_SEL:
        draw_monkey();
        draw_test_camera_target();
        func_800125A4();
        break;
    case MD_GAME:
        switch (gameSubmode)
        {
        default:
        case SMD_GAME_NAMEENTRY_INIT:
        case SMD_GAME_NAMEENTRY_MAIN:
        case SMD_GAME_RESTART_INIT:
        case SMD_GAME_NAMEENTRY_READY_MAIN:
        case SMD_GAME_BONUS_CLEAR_INIT:
        case SMD_GAME_BONUS_CLEAR_MAIN:
        case SMD_GAME_OVER_SAVE:
        case SMD_GAME_OVER_DEST:
            draw_normal_game_scene();
            break;
        case SMD_GAME_RESULT_INIT:
        case SMD_GAME_RESULT_MAIN:
        case SMD_GAME_RESULT_MENU:
            draw_results_scene();
            break;
        case SMD_GAME_ENDING_INIT:
        case SMD_GAME_ENDING_MAIN:
            u_ending_draw();
            draw_test_camera_target();
            break;
        case SMD_GAME_ROLL_INIT:
        case SMD_GAME_ROLL_MAIN:
            func_80093E28();
            break;
        case SMD_GAME_CONTINUE_INIT:
        case SMD_GAME_CONTINUE_MAIN:
            draw_continue_scene();
            draw_test_camera_target();
            break;
        case SMD_GAME_EXTRA_INIT:
        case SMD_GAME_EXTRA_WAIT:
            draw_extra_scene();
            draw_test_camera_target();
            break;
        case SMD_GAME_OVER_INIT:
        case SMD_GAME_OVER_MAIN:
        case SMD_GAME_NAMEENTRY_READY_INIT:
            switch (modeCtrl.gameType)
            {
            case GAMETYPE_MAIN_COMPETITION:
                draw_normal_game_scene();
                break;
            default:
                if (modeCtrl.courseFlags & (1 << 5))
                    draw_normal_game_scene();
                break;
            }
            break;
        case SMD_GAME_INTR_SEL_INIT:
        case SMD_GAME_INTR_SEL_MAIN:
        case SMD_GAME_OVER_POINT_INIT:
        case SMD_GAME_OVER_POINT_MAIN:
            break;
        }
        break;
    case MD_TEST:
        draw_monkey();
        if (lbl_802F1B70 != NULL)
            lbl_802F1B70();
        draw_test_camera_target();
        break;
    case MD_MINI:
        switch (gameSubmode)
        {
        case SMD_MINI_TEST0_INIT:
        case SMD_MINI_TEST0_MAIN:
            func_80093E28();
            break;
        case SMD_MINI_COMMEND_INIT:
        case SMD_MINI_COMMEND_MAIN:
            func_8009BEF8();
            break;
        case SMD_MINI_ENDING_MAIN:
            u_draw_ending_viewer_text();
            break;
        default:
            u_minigame_draw();
            break;
        case SMD_MINI_SELECT_INIT:
        case SMD_MINI_SELECT_MAIN:
            break;
        }
        draw_test_camera_target();
        break;
    case MD_OPTION:
        if (lbl_802F1B70 != NULL)
            lbl_802F1B70();
        draw_test_camera_target();
        break;
    }
    reset_camera_perspective();
    ord_tbl_draw_nodes();
}

static void draw_adv_3d_scene(void)
{
    switch (advSubmode)
    {
    case SMD_ADV_LOGO_MAIN:
        draw_intro_av_logo();
        draw_test_camera_target();
        break;
    case SMD_ADV_DEMO_MAIN:
        draw_adv_demo_scene();
        break;
    case SMD_ADV_INFO_MAIN:
        draw_normal_game_scene();
        u_draw_tutorial_button_and_joystick();
        break;
    case SMD_ADV_RANKING_INIT:
    case SMD_ADV_RANKING_MAIN:
    case SMD_ADV_GAME_READY_INIT:
    case SMD_ADV_GAME_READY_MAIN:
    case SMD_ADV_GAME_PLAY_INIT:
    case SMD_ADV_GAME_PLAY_MAIN:
        draw_normal_game_scene();
        ord_tbl_draw_nodes();
        func_8000C388();
        break;
    }
}

struct Struct80173FA8
{
    u32 modelId;
    s16 xrot;
    s16 yrot;
    Vec pos;
} lbl_80173FA8[] =
{
    { NLMODEL_common_BSKBALL_L_HALF, -25344, 22272, { 0.7, -0.3, 0.2 } },
    { NLMODEL_common_BSKBALL_R_HALF,   8832, 20736, { 0.6, -0.2, 0.2 } },
};

float lbl_80173FD0[] = { 0.4, 0.25, 0.25, 0.5 };

static void draw_intro_av_logo(void)
{
    mathutil_mtxA_from_mtxB();
    mathutil_mtxA_translate(&advLogoInfo.pos);
    mathutil_mtxA_rotate_x(advLogoInfo.xrot);
    mathutil_mtxA_rotate_z(advLogoInfo.zrot);
    nl2ngc_draw_model_sort_translucent_alt2(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_av_ball));
}

static void draw_adv_demo_scene(void)
{
    u_draw_ball_shadow();
    background_light_assign();
    if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
        rend_efc_draw(4);
    if (!(advDemoInfo.flags & (1 << 4))
     && !(advDemoInfo.flags & ADV_FLAG_SHOW_BALLS))
    {
        int i;
        for (i = 0; i < 3; i++)
        {
            float f30;
            if (ballInfo[i].ape->flags & (1 << 5))
                continue;
            mathutil_mtxA_from_mtxB();
            mathutil_mtxA_translate_xyz(
                ballInfo[i].ape->pos.x,
                ballInfo[i].ape->pos.y - 0.45,
                ballInfo[i].ape->pos.z);
            f30 = lbl_80173FD0[ballInfo[i].ape->charaId];
            mathutil_mtxA_scale_s(f30);
            mathutil_mtxA_rotate_x(0x4000);
            avdisp_set_post_mult_color(0.38f, 0.39f, 0.4f, 1.0f);
            avdisp_set_bound_sphere_scale(f30);
            avdisp_draw_model_culled_sort_all(commonGma->modelEntries[polyshadow01].model);
        }
        fade_color_base_default();
    }

    // Draw blimp
    if (advDemoInfo.flags & ADV_FLAG_SHOW_BLIMP)
    {
        mathutil_mtxA_from_mtxB();
        mathutil_mtxA_translate_xyz(
            ballInfo[0].ape->pos.x,
            ballInfo[0].ape->pos.y - 0.25,
            ballInfo[0].ape->pos.z);
        if (advDemoInfo.unk8 >= 0x440 && advDemoInfo.unk8 < 0x51A)
            mathutil_mtxA_translate_xyz(-0.24f, 0.0f, 0.0f);
        nl2ngc_draw_model_sort_translucent_alt2(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_AIRSHIP));
    }

    // Draws old arcade ball. Leftover from Monkey Ball?
    if (advDemoInfo.flags & (1 << 10))
    {
        struct Struct80173FA8 *r27 = lbl_80173FA8;
        int i;
        for (i = 0; i < 2; i++, r27++)
        {
            mathutil_mtxA_from_mtxB();
            mathutil_mtxA_translate(&ballInfo[0].ape->pos);
            mathutil_mtxA_translate(&r27->pos);
            mathutil_mtxA_rotate_y(r27->yrot);
            mathutil_mtxA_rotate_x(r27->xrot);
            nl2ngc_draw_model_sort_translucent_alt2(NLOBJ_MODEL(g_commonNlObj, r27->modelId));
        }
    }

    if (!(advDemoInfo.flags & (1 << 4)))
    {
        if (advDemoInfo.flags & ADV_FLAG_SHOW_STAGE)
            stage_draw();
        if (advDemoInfo.flags & (1 << 11))
            poly_shadow_draw();

        ord_tbl_set_depth_offset(400.0f);
        background_draw();
        ord_tbl_set_depth_offset(0.0f);

        if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
            rend_efc_draw(16);

        if ((advDemoInfo.flags & (1 << 5))
         && !(advDemoInfo.flags & (1 << 12))
         && eventInfo[EVENT_ITEM].state == EV_STATE_RUNNING)
            item_draw();

        if (eventInfo[EVENT_STOBJ].state == EV_STATE_RUNNING
         || (advDemoInfo.flags & (1 << 3)))
            stobj_draw();

        if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
            effect_draw();

        if (advDemoInfo.flags & ADV_FLAG_SHOW_BALLS)
            ball_draw();
    }
    draw_monkey();
    if (backgroundInfo.bgId == BG_TYPE_JUN || backgroundInfo.bgId == BG_TYPE_SPA)
        lens_flare_draw_mask(0);
    draw_test_camera_target();
    ord_tbl_draw_nodes();
    if (backgroundInfo.bgId == BG_TYPE_JUN || backgroundInfo.bgId == BG_TYPE_SPA)
        lens_flare_draw(0);
    if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
        rend_efc_draw(8);
    default_camera_env();
}

static void u_draw_tutorial_button_and_joystick(void)
{
    Vec sp48;
    int i;
    float baseScale;  // scale value for the base model
    float stickX;
    float stickY;
    int stickXRot;
    int stickZRot;
    Mtx44 projMtx;

    MTXPerspective(projMtx, 1.0f, 1.33333333f, 0.1f, 100000.0f);
    GXSetProjection(projMtx, 0);
    mathutil_mtxA_from_identity();
    load_light_group_uncached(LIGHT_GROUP_SINGLE_UNIT);
    sp48.x = -0.0055f;
    sp48.y = -0.003f;
    sp48.z = -0.718f;

    if (!(debugFlags & 0xA))
    {
        if (advTutorialInfo.state > 0)
            advTutorialInfo.transitionValue += 0.05 * -advTutorialInfo.transitionValue;
        else
            advTutorialInfo.transitionValue += 0.05 * (1.0 - advTutorialInfo.transitionValue);
    }
    baseScale = 0.0005f;

    // Draw the analog stick base
    mathutil_mtxA_from_identity();
    mathutil_mtxA_translate(&sp48);
    mathutil_mtxA_rotate_x(3328.0 + 32768.0 * (1.0 - advTutorialInfo.transitionValue));
    mathutil_mtxA_scale_xyz(baseScale, baseScale, baseScale);
    nlSetScaleFactor(baseScale);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
    avdisp_draw_model_unculled_sort_translucent(commonGma->modelEntries[lever_analogue_base].model);

    // Draw the simulated analog stick
    mathutil_mtxA_translate_xyz(0.0f, -2.7f, 0.0f);
    mathutil_mtxA_push();
    mathutil_mtxA_rotate_x(CLAMP(advTutorialInfo.stickXRot * 12, -0x1000, 0x1000));
    mathutil_mtxA_rotate_z(CLAMP(advTutorialInfo.stickZRot * 8, -0x1000, 0x1000));
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
    avdisp_set_alpha(advTutorialInfo.transitionValue);
    avdisp_draw_model_unculled_sort_translucent(commonGma->modelEntries[lever_analogue].model);
    mathutil_mtxA_pop();

    // Draw the transparent stick based on the player's analog stick position
    stickX = 0.0f;
    stickY = 0.0f;
    for (i = 0; i < 4; i++)
    {
        if (controllerInfo[i].held.err == 0)
        {
            stickX = (float)controllerInfo[i].held.stickX / 60.0;
            stickY = -(float)controllerInfo[i].held.stickY / 60.0;
            break;
        }
    }
    if (stickX < -1.0)
        stickX = -1.0f;
    else if (stickX > 1.0)
        stickX = 1.0f;
    if (stickY < -1.0)
        stickY = -1.0f;
    else if (stickY > 1.0)
        stickY = 1.0f;
    stickXRot = (4187.0f * stickY);
    stickZRot = (4187.0f * -stickX);
    mathutil_mtxA_rotate_x(CLAMP(stickXRot * 2, -0x1000, 0x1000));
    mathutil_mtxA_rotate_z(CLAMP(stickZRot * 2, -0x1000, 0x1000));
    mathutil_mtxA_scale_s(0.99f);
    nlSetScaleFactor(0.99f);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
    avdisp_set_alpha(advTutorialInfo.transitionValue * 0.5);
    avdisp_draw_model_unculled_sort_translucent(commonGma->modelEntries[lever_analogue].model);

    // Draw the button base
    mathutil_mtxA_from_identity();
    mathutil_mtxA_translate(&sp48);
    mathutil_mtxA_rotate_x(3328.0f + (-32768.0f * advTutorialInfo.transitionValue));
    mathutil_mtxA_translate_xyz(0.0f, 0.00058f, 0.0f);
    mathutil_mtxA_scale_xyz(baseScale, baseScale, baseScale);
    nlSetScaleFactor(baseScale);
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
    avdisp_draw_model_unculled_sort_translucent(commonGma->modelEntries[button_base].model);

    // Draw the A button
    if (advTutorialInfo.state == 2)
    {
        mathutil_mtxA_scale_xyz(1.0f, 0.25f, 1.0f);
        nlSetScaleFactor(1.0f);
    }
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
    avdisp_set_alpha(1.0 - advTutorialInfo.transitionValue);
    avdisp_draw_model_unculled_sort_translucent(commonGma->modelEntries[button].model);
    ord_tbl_draw_nodes();
}

const GXColor lbl_802F2978 = {0, 0, 0, 0};

struct DepthMaskParams
{
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
};

static void draw_depth_mask(struct DepthMaskParams *a)
{
    float x1 = a->unk0;
    float y1 = a->unk4;
    float x2 = a->unk8;
    float y2 = a->unkC;
    float z = a->unk10;
    u8 filler[8];

    gxutil_set_vtx_attrs((1 << GX_VA_POS));
    GXSetBlendMode_cached(GX_BM_BLEND, GX_BL_ZERO, GX_BL_ONE, GX_LO_CLEAR);
    GXSetZMode_cached(GX_ENABLE, GX_ALWAYS, GX_ENABLE);
    GXSetFog_cached(GX_FOG_NONE, 0.0f, 100.0f, 0.1f, 20000.0f, lbl_802F2978);
    GXSetCullMode_cached(GX_CULL_NONE);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevKAlphaSel_cached(GX_TEVSTAGE0, GX_TEV_KASEL_1);
    GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
    GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_DIVIDE_2, GX_TRUE, GX_TEVPREV);
    GXSetNumTevStages_cached(1);
    mathutil_mtxA_push();
    mathutil_mtxA_from_identity();
    GXLoadPosMtxImm(mathutilData->mtxA, 0);
    mathutil_mtxA_pop();

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(x2, y1, z);
        GXPosition3f32(x1, y1, z);
        GXPosition3f32(x1, y2, z);
        GXPosition3f32(x2, y2, z);
    GXEnd();

    GXSetZMode_cached(GX_ENABLE, 3, GX_ENABLE);
}

static void func_8000C388(void)
{
    struct DepthMaskParams sp14;
    Vec sp8;
    float f3;

    f3 = -(10000.0f * currentCamera->sub28.unk38);
    sp14.unk0 = f3 * currentCamera->sub28.aspect;
    sp14.unk4 = 0.0f;
    sp14.unk8 = 0.0f;
    sp14.unkC = f3;
    sp14.unk10 = -10000.0f;
    draw_depth_mask(&sp14);
    if (advTutorialInfo.state == 0)
    {
        s16 r30;
        s16 v3;
        sp8.x = 0.0f;
        sp8.y = 1.0f;
        sp8.z = 0.0f;
        mathutil_mtxA_from_identity();
        mathutil_mtxA_rotate_y(-currentCamera->rotY);
        mathutil_mtxA_rotate_z(-worldInfo[0].zrot);
        mathutil_mtxA_rotate_x(-worldInfo[0].xrot);
        mathutil_mtxA_tf_vec(&sp8, &sp8);
        r30 = -mathutil_atan2(sp8.z, sp8.y);
        v3 = mathutil_atan2(sp8.x, mathutil_sqrt(mathutil_sum_of_sq_2(sp8.z, sp8.y)));
        r30 *= 0.2;
        v3 *= 0.2;
        advTutorialInfo.stickXRot = advTutorialInfo.stickXRot + 0.2 * ((float)r30 - (float)advTutorialInfo.stickXRot);
        advTutorialInfo.stickZRot = advTutorialInfo.stickZRot + 0.2 * ((float)v3 - (float)advTutorialInfo.stickZRot);
        u_draw_tutorial_button_and_joystick();
    }
}

void draw_normal_game_scene(void)
{
    int i;
    struct Ball *oldBall = currentBall;
    for (i = 0; i < 4; i++)
    {
        if (cameraInfo[i].sub28.vp.width > 0.0f && cameraInfo[i].sub28.vp.height > 0.0f)
        {
            if (g_poolInfo.playerPool.statusList[i] == STAT_NULL
             || g_poolInfo.playerPool.statusList[i] == STAT_FREEZE
             || (cameraInfo[i].flags & (1 << 6)))
            {
                if (!(cameraInfo[i].flags & (1 << 7)))
                    continue;
            }
            currentBall = &ballInfo[i];
            change_current_camera(i);
            u_draw_ball_shadow();
            background_light_assign();
            reset_light_group(i);
            if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                rend_efc_draw(4);
            draw_monkey();
            if (eventInfo[EVENT_STAGE].state == EV_STATE_RUNNING
             || eventInfo[EVENT_STAGE].state == EV_STATE_SUSPENDED)
                stage_draw();
            poly_shadow_draw();
            if (eventInfo[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
            {
                ord_tbl_set_depth_offset(400.0f);
                background_draw();
                ord_tbl_set_depth_offset(0.0f);
            }
            if (eventInfo[EVENT_STAGE].state == EV_STATE_RUNNING
             || eventInfo[EVENT_STAGE].state == EV_STATE_SUSPENDED)
                draw_stage_preview();
            if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                rend_efc_draw(16);
            if (eventInfo[EVENT_ITEM].state == EV_STATE_RUNNING)
                item_draw();  // draws bananas, but not the ones being picked up
            if (eventInfo[EVENT_STOBJ].state == EV_STATE_RUNNING)
                stobj_draw();
            if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
                effect_draw();  // draws sparks and stars
            if (eventInfo[EVENT_BALL].state == EV_STATE_RUNNING)
                ball_draw();
            if (backgroundInfo.unk8 & 1)
                lens_flare_draw_mask(i);
            draw_test_camera_target();
            ord_tbl_draw_nodes();
            if (backgroundInfo.unk8 & 1)
                lens_flare_draw(i);
            if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                rend_efc_draw(8);
        }
    }
    func_8000C7A4();
    currentBall = oldBall;
    default_camera_env();
}

static void func_8000C7A4(void)
{
    int i;

    polyDisp.flags |= (1 << 3);
    for (i = 0; i < 4; i++)
    {
        if (cameraInfo[i].sub28.vp.width > 0.0f && cameraInfo[i].sub28.vp.height > 0.0f
         && (cameraInfo[i].flags & (1 << 6)))
        {
            currentBall = &ballInfo[i];
            change_current_camera(i);
            reset_light_group(i);
            if (eventInfo[EVENT_STAGE].state == EV_STATE_RUNNING
             || eventInfo[EVENT_STAGE].state == EV_STATE_SUSPENDED)
                stage_draw();
            if (eventInfo[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
            {
                ord_tbl_set_depth_offset(400.0f);
                background_draw();
                ord_tbl_set_depth_offset(0.0f);
            }
            if (eventInfo[EVENT_ITEM].state == EV_STATE_RUNNING)
                item_draw();
            if (eventInfo[EVENT_STOBJ].state == EV_STATE_RUNNING)
                stobj_draw();
            if (eventInfo[EVENT_BALL].state == EV_STATE_RUNNING)
                ball_draw();
            draw_live_arrow();
            draw_test_camera_target();
            ord_tbl_draw_nodes();
        }
    }
    polyDisp.flags &= ~(1 << 3);
}

static u16 arrowModelIDs[4] = { ARROW_1P, ARROW_2P, ARROW_3P, ARROW_4P };

static void draw_live_arrow(void)
{
    struct Ball *ball;
    s8 *status = g_poolInfo.playerPool.statusList;
    int i;
    Vec sp8;
    float f27;

    ball = ballInfo;
    for (i = 0; i < g_poolInfo.playerPool.count; i++, ball++, status++)
    {
        if (*status == STAT_NULL || *status == STAT_FREEZE)
            continue;
        if (ball->flags & BALL_FLAG_INVISIBLE)
            continue;
        mathutil_mtxA_from_identity();
        f27 = 0.8 - 0.1 * (((globalAnimTimer / 16) + i) % 3);
        mathutil_mtxA_scale_s(f27);
        mathutil_mtxA_mult_right(mathutilData->mtxB);
        mathutil_mtxA_translate(&ball->pos);
        mathutil_mtxA_rotate_y(cameraInfo[i].rotY - 0x8000);
        mathutil_mtxA_rotate_x(-0x4000);
        mathutil_mtxA_translate_xyz(0.0f, ball->currRadius, 0.0f);
        mathutil_mtxA_get_translate_alt(&sp8);
        if (sp8.z < -4.0 * f27)
            mathutil_mtxA_scale_s(sp8.z / (-4.0 * f27));
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_unculled_sort_translucent(commonGma->modelEntries[arrowModelIDs[i]].model);
    }
}

static void draw_continue_scene(void)
{
    EnvMapFunc func;
    EnvMapFunc prevFunc;
    int r4;
    u32 r5;
    Vec sp5C;
    Mtx sp2C;

    polyDisp.flags |= 1;
    change_current_camera(modeCtrl.currPlayer);
    u_draw_ball_shadow();
    background_light_assign();
    reset_light_group(modeCtrl.currPlayer);
    draw_monkey();

    if (u_stageBoxModel != NULL)
    {
        func = backgroundInfo.stageEnvMapFunc;
        if (func != NULL)
            prevFunc = u_avdisp_set_some_func_1(func);
        mathutil_mtxA_from_mtxB();
        u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
        avdisp_draw_model_unculled_sort_translucent(u_stageBoxModel);
        if (func != NULL)
            u_avdisp_set_some_func_1(prevFunc);
    }
    else
    {
        mathutil_mtxA_from_mtxB();
        nl2ngc_draw_model_sort_translucent_alt2(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_BOX));
    }

    if (eventInfo[EVENT_BALL].state == EV_STATE_RUNNING)
        ball_draw();
    if (eventInfo[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
        background_draw();
    if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
        effect_draw();
    ord_tbl_draw_nodes();
    r4 = modeCtrl.submodeTimer;
    if (r4 > 60)
        r4 = 60;

    r5 = FALSE;
    if ((gameSubmode == SMD_GAME_CONTINUE_INIT || gameSubmode == SMD_GAME_CONTINUE_MAIN)
     && (modeCtrl.courseFlags & (1 << 2))
     && modeCtrl.unk10 == 1)
        r5 = TRUE;

    if (!r5)
    {
        float f1;
        float f26, f27;

        if (gameSubmode != SMD_GAME_CONTINUE_INIT && gameSubmode != SMD_GAME_CONTINUE_MAIN)
            f1 = 0.0f;
        else if ((modeCtrl.courseFlags & (1 << 2)) && modeCtrl.unk10 == 0)
            f1 = r4 / 60.0f;
        else
            f1 = 1.0f;

        nlObjPutSetFadeColorBase(f1, f1, f1);
        mathutil_mtxA_from_mtxB();
        sp5C.x = currentBall->pos.x;
        sp5C.y = currentBall->pos.y - currentBall->currRadius + 0.01;
        sp5C.z = currentBall->pos.z;
        mathutil_mtxA_translate(&sp5C);
        f26 = 0.9f;
        f27 = 4.6f;
        mathutil_mtxA_scale_xyz(f26, f27, f26);
        nlSetScaleFactor(MAX(f26, f27));
        nl2ngc_draw_model_sort_none_alt2(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_SPOT_LIGHT));
        nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);
        mathutil_mtxA_from_mtxB();
        mathutil_mtxA_translate_xyz(0.0f, f27, 0.0f);
        mathutil_mtxA_rotate_x(-16384);
        mathutil_mtxA_scale_xyz(0.25f, 0.25f, 0.25f);
        nlSetScaleFactor(0.25f);
        nl2ngc_draw_model_sort_none_alt2(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_spotl1));
    }
    else
    {
        Vec sp20;
        Vec sp14;
        Vec sp8;
        int r30;
        int i;
        float f28 = r4 / 60.0f;

        f28 = (f28 - 0.5) * 2.0;
        f28 = CLAMP(f28, 0.0, 1.0);
        f28 *= f28;
        mathutil_mtxA_from_identity();
        mathutil_mtxA_to_mtx(sp2C);
        sp2C[0][0] = 0.9f;
        sp2C[2][2] = 0.9f;
        sp2C[1][1] = 4.6f;
        sp2C[1][3] = 0.01f;

        for (i = 0; i < 4; i++)
        {
            float f26, f27;

            r30 = i * 0x4000 + 0x2000;
            f26 = mathutil_sin(r30);
            f27 = mathutil_cos(r30);

            nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);
            sp20.x = 3.5 * f26;
            sp20.y = 4.6f;
            sp20.z = 3.5 * f27;
            sp14.x = (3.5 * f26) * f28;
            sp14.y = 0.0f;
            sp14.z = (3.5 * f27) * f28;
            sp2C[0][1] = sp20.x - sp14.x;
            sp2C[2][1] = sp20.z - sp14.z;
            sp2C[0][3] = sp14.x;
            sp2C[2][3] = sp14.z;
            mathutil_mtxA_from_mtx(sp2C);
            mathutil_mtxA_mult_left(mathutilData->mtxB);
            nlSetScaleFactor(4.6f);
            nl2ngc_draw_model_sort_none_alt2(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_SPOT_LIGHT_MULTI));
            nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);
            mathutil_mtxA_from_mtxB_translate(&sp20);
            sp8.x = sp14.x - sp20.x;
            sp8.y = sp14.y - sp20.y;
            sp8.z = sp14.z - sp20.z;
            mathutil_mtxA_rotate_y(mathutil_atan2(sp8.x, sp8.z) - 32768);
            mathutil_mtxA_rotate_x(mathutil_atan2(sp8.y, mathutil_sqrt(mathutil_sum_of_sq_2(sp8.x, sp8.z))));
            mathutil_mtxA_scale_xyz(0.25f, 0.25f, 0.25f);
            nlSetScaleFactor(0.25f);
            nl2ngc_draw_model_sort_none_alt2(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_spotl1));
        }
    }

    fade_color_base_default();
}

static void draw_extra_scene(void)
{
    change_current_camera(modeCtrl.currPlayer);
    u_draw_ball_shadow();
    background_light_assign();
    draw_monkey();
    if (eventInfo[EVENT_BALL].state == EV_STATE_RUNNING)
        ball_draw();
    if (eventInfo[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
        background_draw();
    if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
        effect_draw();
    ord_tbl_draw_nodes();
}

static void draw_results_scene(void)
{
    int i;
    struct Ball *ball = currentBall;

    for (i = 0; i < 4; i++)
    {
        if (cameraInfo[i].sub28.vp.width > 0.0f && cameraInfo[i].sub28.vp.height > 0.0f)
        {
            if ((g_poolInfo.playerPool.statusList[i] == STAT_NULL || g_poolInfo.playerPool.statusList[i] == STAT_FREEZE)
             && !(cameraInfo[i].flags & (1 << 6)))
                continue;

            if (cameraInfo[i].flags & (1 << 6))
                polyDisp.flags |= 8;
            currentBall = &ballInfo[i];
            change_current_camera(i);
            u_draw_ball_shadow();
            background_light_assign();
            reset_light_group(i);
            if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                rend_efc_draw(4);
            draw_monkey();
            if (eventInfo[EVENT_STAGE].state == EV_STATE_RUNNING
             || eventInfo[EVENT_STAGE].state == EV_STATE_SUSPENDED)
                stage_draw();
            poly_shadow_draw();
            if (eventInfo[EVENT_BACKGROUND].state == EV_STATE_RUNNING)
            {
                ord_tbl_set_depth_offset(400.0f);
                background_draw();
                ord_tbl_set_depth_offset(0.0f);
            }
            if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                rend_efc_draw(16);
            if (eventInfo[EVENT_ITEM].state == EV_STATE_RUNNING)
                item_draw();
            if (eventInfo[EVENT_STOBJ].state == EV_STATE_RUNNING)
                stobj_draw();
            if (eventInfo[EVENT_EFFECT].state == EV_STATE_RUNNING)
                effect_draw();
            if (eventInfo[EVENT_BALL].state == EV_STATE_RUNNING)
                ball_draw();
            if (backgroundInfo.unk8 & 1)
                lens_flare_draw_mask(i);
            draw_test_camera_target();
            ord_tbl_draw_nodes();
            if (backgroundInfo.unk8 & 1)
                lens_flare_draw(i);
            if (eventInfo[EVENT_REND_EFC].state == EV_STATE_RUNNING)
                rend_efc_draw(8);
            if (cameraInfo[i].flags & (1 << 6))
                polyDisp.flags &= ~(1 << 3);
        }
    }
    currentBall = ball;
    default_camera_env();
}

static void draw_test_camera_target(void)
{
    GXColor amb = {0xFF, 0xFF, 0xFF, 0xFF};
    GXColor mat = {0x00, 0x00, 0x00, 0xFF};
    GXColor red   = {0xFF, 0x00, 0x00, 0xFF};
    GXColor green = {0x00, 0xFF, 0x00, 0xFF};
    GXColor blue  = {0x00, 0x00, 0xFF, 0xFF};
    GXLightObj lightObj;
    Vec light2pos;
    Vec light3pos;
    Vec light4pos;
    Vec pos;

    if ((dipSwitches & DIP_TEST_CAM) && !(dipSwitches & DIP_NO_INTR))
    {
        mathutil_mtxA_from_translate(&currentCamera->lookAt);
        mathutil_mtxA_rotate_y((powerOnTimer << 8) * 1.2f);
        mathutil_unk_inline(10.0f, &light2pos);

        mathutil_mtxA_from_translate(&currentCamera->lookAt);
        mathutil_mtxA_rotate_x(powerOnTimer << 8);
        mathutil_unk_inline(-10.0f, &light3pos);

        mathutil_mtxA_from_translate(&currentCamera->lookAt);
        mathutil_mtxA_rotate_z((powerOnTimer << 8) * 0.8f);

        mathutil_mtxA_tf_point_xyz(&light4pos, 0.0f, 10.0f, 0.0f);
        mathutil_mtxA_from_mtx(userWork->matrices[0]);

        // Light 2
        GXInitLightSpot(&lightObj, 0.0f, GX_SP_OFF);
        GXInitLightDistAttn(&lightObj, 150.0f, 0.2f, GX_DA_STEEP);
        mathutil_mtxA_tf_point(&light2pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXInitLightColor(&lightObj, green);
        GXLoadLightObjImm(&lightObj, GX_LIGHT2);

        // Light 3
        GXInitLightSpot(&lightObj, 0.0f, GX_SP_OFF);
        GXInitLightDistAttn(&lightObj, 150.0f, 0.2f, GX_DA_STEEP);
        mathutil_mtxA_tf_point(&light3pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXInitLightColor(&lightObj, blue);
        GXLoadLightObjImm(&lightObj, GX_LIGHT3);

        // Light 4
        GXInitLightSpot(&lightObj, 0.0f, GX_SP_OFF);
        GXInitLightDistAttn(&lightObj, 150.0f, 0.2f, GX_DA_STEEP);
        mathutil_mtxA_tf_point(&light4pos, &pos);
        GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
        GXInitLightColor(&lightObj, red);
        GXLoadLightObjImm(&lightObj, GX_LIGHT4);

        GXSetChanMatColor(GX_COLOR0, amb);
        GXSetChanAmbColor(GX_COLOR0, mat);
        GXSetChanCtrl(
            GX_COLOR0A0,  // chan
            GX_ENABLE,  // enable
            GX_SRC_REG,  // amb_src
            GX_SRC_REG,  // mat_src
            GX_LIGHT2 | GX_LIGHT3 | GX_LIGHT4,  // light_mask
            GX_DF_CLAMP,  // diff_fn
            GX_AF_NONE);  // attn_fn
        GXSetNumChans(1);
        GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
        GXSetTevOp_cached(GX_TEVSTAGE0, GX_PASSCLR);
        GXSetNumTexGens(0);
        GXSetNumTevStages_cached(1);

        mathutil_mtxA_from_mtx(userWork->matrices[0]);
        mathutil_mtxA_translate(&currentCamera->lookAt);
        mathutil_mtxA_scale_xyz(0.05f, 0.05f, 0.05f);
        GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
        GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
        GXDrawSphere(8, 8);
    }
}

static struct
{
    int state;
    int unused;
    u32 angle;
    s32 angleDelta;
    float scale;
    float scaleDelta;
} bombSpark;

void func_8000D5B8(void)
{
    bombSpark.state = 0;
    bombSpark.angle = 0;
    bombSpark.angleDelta = 0;
    bombSpark.scale = 1.0f;
    bombSpark.scaleDelta = 0.0f;
}

static struct Keyframe bombSparkXKeyframes[] =
{
    { 1,   0,  8.7540102,          0,          0 },
    { 1,  72, 1.83571005,  -0.090412,  -0.090412 },
    { 1,  74,    1.66866,   -0.07464,   -0.07464 },
    { 1,  76,      1.536, -0.0599843, -0.0599843 },
    { 1,  84,    1.11173, -0.0567173, -0.0567173 },
    { 1,  89,   0.801074, -0.0672826, -0.0672826 },
    { 1,  92,   0.582271, -0.0860023, -0.0860023 },
    { 1,  93,    0.48935, -0.0852437, -0.0852437 },
    { 1,  95,    0.33974, -0.0706416, -0.0706416 },
    { 1, 100,  0.0010883,          0,          0 },
};

static struct Keyframe bombSparkYKeyframes[] =
{
    { 1,   0,   -1.00663,            0,            0 },
    { 1,  49,   -1.00662, -0.000118196, -0.000118196 },
    { 1,  70,  -0.994419,     0.010028,     0.010028 },
    { 1,  72,  -0.957594,    0.0308768,    0.0308768 },
    { 1,  73,  -0.919437,    0.0458789,    0.0458789 },
    { 1,  75,  -0.800302,    0.0691816,    0.0691816 },
    { 1,  83,   -0.16478,     0.079378,     0.079378 },
    { 1,  88,   0.210374,    0.0705764,    0.0705764 },
    { 1,  91,   0.411975,    0.0589553,    0.0589553 },
    { 1,  92,   0.465595,    0.0239604,    0.0239604 },
    { 1,  93,   0.459896,   -0.0309341,   -0.0309341 },
    { 1,  95,   0.340102,    -0.065165,    -0.065165 },
    { 1, 100, -0.0010541,            0,            0 },
};

static void draw_timer_bomb_fuse(void)
{
    struct NlModel *tempModel;
    struct Sprite *sprite;
    float t;  // portion of clock time remaining (from 0.0 to 1.0)
    float x;
    float y;
    float scale;
    Vec sp94;
    Mtx44 mtx;
    Vec sp48;
    Vec sp3C;
    Vec sp30;
    Vec sp24;
    Point3d sparkPos;
    float f4;
    float f3;
    struct NlVtxTypeB *vtx;
    int i;
    int r7;
    int faceCount;
    float f1;
    struct NlMesh *mesh;

    if (eventInfo[EVENT_VIEW].state == EV_STATE_RUNNING || infoWork.timerCurr <= 0)
        return;

    sprite = find_sprite_with_tag(SPRITE_TAG_TIMER);
    if (sprite == NULL)
    {
        x = 0.0f;
        y = 0.0f;
    }
    else
    {
        x = (sprite->x - 320.0f) / 320.0f;
        y = (56.0f - sprite->y) / 240.0f;
    }

    MTXPerspective(mtx, 60.0f, 1.3333332538604736f, 0.00989999994635582f, 20000.0f);
    mtx[0][2] -= mtx[0][0] * x * 1.3333332538604736f * 0.5773502588272095f;
    mtx[1][2] -= mtx[1][1] * y * 0.5773502588272095f;
    GXSetProjection(mtx, 0);

    /* NOTE: Most of the code here deals with manipulating vertices for the
     * arcade fuse model, which is never drawn in-game.
     */

    tempModel = lbl_802F1B4C;
    t = (float)infoWork.timerCurr / (float)infoWork.timerMax;

    // Make a temporary copy of the timer fuse, which we will modify
    memcpy(
        tempModel,
        NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_OBJ_COLOR_BAR_03),
        NLMODEL_HEADER(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_OBJ_COLOR_BAR_03))->unk4->modelSize);

    mesh = (struct NlMesh *)tempModel->meshStart;
    faceCount = ((struct NlDispList *)(((struct NlMesh *)tempModel->meshStart)->dispListStart))->faceCount;

    f4 = 2.0 * (t - 0.5);
    f4 = CLAMP(f4, 0.0, 1.0);

    f3 = f4 * (faceCount - 2.0);
    r7 = mathutil_floor(f3 * 0.5) * 2.0f;
    f1 = (f3 - r7) * 0.5;

    vtx = (struct NlVtxTypeB *)((struct NlDispList *)mesh->dispListStart)->vtxData;
    for (i = faceCount - 1; i >= 0; i--, vtx++)
    {
        if (t < 0.5)
            vtx->s = 0.25;
        else if (i < r7)
            vtx->s = 0.75f;
        else if (i < r7 + 2)
            vtx->s = f1 * 0.25 + 0.5;
        else if (i < r7 + 4)
            vtx->s = 0.25 + f1 * 0.25;
        else
            vtx->s = 0.25;
    }

    // Calculate something based on vertex positions?
    // The result is never used, so this is pointless.
    if (t >= 0.5)
    {
        int index = faceCount - 4 - r7;
        float f2 = 1.0 - f1;

        vtx = &((struct NlVtxTypeB *)((struct NlDispList *)mesh->dispListStart)->vtxData)[index];

        sp48.x = vtx[0].x * f1 + vtx[2].x * f2;
        sp48.y = vtx[0].y * f1 + vtx[2].y * f2;
        sp48.z = vtx[0].z * f1 + vtx[2].z * f2;

        sp3C.x = vtx[1].x * f1 + vtx[3].x * f2;
        sp3C.y = vtx[1].y * f1 + vtx[3].y * f2;
        sp3C.z = vtx[1].z * f1 + vtx[3].z * f2;

        sp94.x = 0.5 * (sp48.x + sp3C.x);
        sp94.y = 0.5 * (sp48.y + sp3C.y);
        sp94.z = 0.001 + 0.5 * (sp48.z + sp3C.z);
    }

    // WTF??
    mesh = (void *)((u32 *)mesh + (((s32)mesh->dispListSize >> 2) + 0x14));

    faceCount = ((struct NlDispList *)mesh->dispListStart)->faceCount;

    f4 = t * 2.0;
    f4 = CLAMP(f4, 0.0, 1.0);

    f3 = f4 * (faceCount - 2.0);
    r7 = mathutil_floor(f3 * 0.5) * 2.0f;
    f1 = (f3 - r7) * 0.5;

    vtx = (void *)((struct NlDispList *)mesh->dispListStart)->vtxData;
    for (i = faceCount - 1; i >= 0; i--, vtx++)
    {
        if (t > 0.5)
            vtx->s = 0.75;
        else if (i < r7)
            vtx->s = 0.75;
        else if (i < r7 + 2)
            vtx->s = 0.5 + f1 * 0.25;
        else if (i < r7 + 4)
            vtx->s = 0.25 + f1 * 0.25;
        else
            vtx->s = 0.25;
    }

    // Calculate something based on vertex positions?
    // The result is never used, so this is pointless.
    if (t < 0.5)
    {
        int index = faceCount - 4 - r7;
        float f2 = 1.0 - f1;

        vtx = &((struct NlVtxTypeB *)((struct NlDispList *)mesh->dispListStart)->vtxData)[index];

        sp30.x = vtx[0].x * f1 + vtx[2].x * f2;
        sp30.y = vtx[0].y * f1 + vtx[2].y * f2;
        sp30.z = vtx[0].z * f1 + vtx[2].z * f2;

        sp24.x = vtx[1].x * f1 + vtx[3].x * f2;
        sp24.y = vtx[1].y * f1 + vtx[3].y * f2;
        sp24.z = vtx[1].z * f1 + vtx[3].z * f2;

        sp94.x = 0.5 * (sp30.x + sp24.x);
        sp94.y = 0.5 * (sp30.y + sp24.y);
        sp94.z = 0.001 + 0.5 * (sp30.z + sp24.z);
    }

    switch (bombSpark.state)
    {
    case 0:
        if (!(infoWork.flags & INFO_FLAG_TIMER_PAUSED))
        {
            bombSpark.state = 1;
            bombSpark.scaleDelta = 0.125f;
            bombSpark.angleDelta = -((rand() & 0x3FF) + 0x400);
        }
        break;
    case 1:
        bombSpark.scaleDelta -= 0.5/60.0;//0.0083333333333333332;
        bombSpark.scale += bombSpark.scaleDelta;
        if (bombSpark.scale < 1.0)
        {
            bombSpark.scale = 1.0f;
            bombSpark.scaleDelta = 0.0f;
            bombSpark.state = 2;
        }
        break;
    case 2:
        if (infoWork.flags & INFO_FLAG_TIMER_PAUSED)
            bombSpark.state = 3;
        break;
    case 3:
        bombSpark.state = 4;
        break;
    case 4:
        bombSpark.state = 0;
        break;
    }
    if (infoWork.flags & INFO_FLAG_TIMER_PAUSED)
        bombSpark.angleDelta -= (bombSpark.angleDelta >> 3);
    else if (t > 0.5)
        bombSpark.angleDelta += (-768 - bombSpark.angleDelta) >> 4;
    else
        bombSpark.angleDelta += (-1536 - bombSpark.angleDelta) >> 4;
    if (!(debugFlags & 0xA))
        bombSpark.angle += bombSpark.angleDelta;

    nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);
    avdisp_set_post_mult_color(1.0f, t, 0.0f, 1.0f);
    mathutil_mtxA_from_translate_xyz(0.0f, (1.0 - t) - 0.5, 0.0f);
    avdisp_set_custom_tex_mtx(0, mathutilData->mtxA);

    // Draw new bomb fuse
    mathutil_mtxA_from_identity();
    mathutil_mtxA_translate_xyz(0.00094f, 0.00519f, -0.01f);
    scale = 0.0007f;
    mathutil_mtxA_scale_s(scale);
    u_gxutil_upload_some_mtx(mathutilData->mtxA, 0);
    avdisp_set_bound_sphere_scale(scale);
    avdisp_enable_custom_tex_mtx(1);
    avdisp_draw_model_unculled_sort_translucent(commonGma->modelEntries[BOMB_FUSE].model);
    avdisp_enable_custom_tex_mtx(0);

    // Draw spark
    sparkPos.x = interpolate_keyframes(ARRAY_COUNT(bombSparkXKeyframes), bombSparkXKeyframes, (1.0 - t) * 100.0);
    sparkPos.y = interpolate_keyframes(ARRAY_COUNT(bombSparkYKeyframes), bombSparkYKeyframes, (1.0 - t) * 100.0);
    sparkPos.z = 0.141f;
    mathutil_mtxA_translate(&sparkPos);
    mathutil_mtxA_sq_from_identity();
    mathutil_mtxA_rotate_z(bombSpark.angle);
    mathutil_mtxA_scale_s(0.0149f);
    mathutil_mtxA_scale_xyz(bombSpark.scale, bombSpark.scale, bombSpark.scale);
    nlObjPutImm(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_TIMER_FIRE));
    fade_color_base_default();
}

static void set_backdrop_color(void)
{
    BOOL useFogColor = TRUE;
    GXColor color;

    switch (gameMode)
    {
    case MD_GAME:
        switch (gameSubmode)
        {
        case SMD_GAME_INTR_SEL_INIT:
        case SMD_GAME_INTR_SEL_MAIN:
        case SMD_GAME_OVER_POINT_INIT:
        case SMD_GAME_OVER_POINT_MAIN:
            if ((modeCtrl.courseFlags & ((1 << 5)|(1 << 6))) == 0)
            {
                color.r = 0;
                color.g = 0;
                color.b = 0;
                color.a = 0;
            }
            else
            {
                color.r = 255;
                color.g = 255;
                color.b = 255;
                color.a = 0;
            }
            useFogColor = FALSE;
            break;
        default:
            color = backgroundInfo.backdropColor;
            break;
        }
        break;
    case MD_SEL:
    case MD_MINI:
        color = backgroundInfo.backdropColor;
        break;
    case MD_ADV:
        switch (gameSubmode)
        {
        case SMD_ADV_LOGO_INIT:
        case SMD_ADV_LOGO_MAIN:
        case SMD_ADV_WARNING_INIT:
        case SMD_ADV_WARNING_MAIN:
        case SMD_ADV_RATING_INIT:
        case SMD_ADV_RATING_MAIN:
        case SMD_ADV_START_INIT:
        case SMD_ADV_START_MAIN:
            color.r = introBackdropColor >> 16;
            color.g = introBackdropColor >> 8;
            color.b = introBackdropColor >> 0;
            color.a = introBackdropColor >> 24;
            break;
        default:
            color = backgroundInfo.backdropColor;
            introBackdropColor = RGBA(color.r, color.g, color.b, color.a);
            break;
        }
        break;
    case MD_OPTION:
        switch (gameSubmode)
        {
        case SMD_OPTION_REPLAY_PLAY_INIT:
        case SMD_OPTION_REPLAY_PLAY_MAIN:
            color = backgroundInfo.backdropColor;
            break;
        default:
            color.r = color.g = color.b = 0;
            color.a = 255;
            break;
        }
        break;
    case MD_TEST:
    default:
        color.r = 64;
        color.g = 64;
        color.b = 64;
        color.a = 0;
        break;
    }

    if (useFogColor && fogInfo.enabled)
    {
        color.r = fogInfo.r;
        color.g = fogInfo.g;
        color.b = fogInfo.b;
    }
    GXSetCopyClear(color, 0x00FFFFFF);
}

void draw_monkey(void)
{
    ord_tbl_add_depth_offset(0.5f);
    thread_loop(ALL_THREAD_MASK & ~(1 << THREAD_GROUP_7));
    ord_tbl_add_depth_offset(-0.5f);
}

static void func_8000E134(void)
{
    if (eventInfo[EVENT_BALL].state == EV_STATE_RUNNING)
        u_ball_shadow_something_1();
    if (eventInfo[EVENT_ITEM].state == EV_STATE_RUNNING)
        item_draw_shadows();
}

static void func_8000E180(void)
{
    shadowerase_main();
    poly_shadow_init();
}

void polydisp_set_some_color_based_on_curr_mode(float opacity)
{
    switch (gameSubmode)
    {
    case SMD_GAME_CONTINUE_INIT:
    case SMD_GAME_CONTINUE_MAIN:
        nlObjPutSetFadeColorBase(0.8f, 0.8f, 0.8f);
        avdisp_set_post_mult_color(0.8f, 0.8f, 0.8f, opacity);
        break;
    case SMD_GAME_OVER_INIT:
    case SMD_GAME_OVER_MAIN:
    case SMD_GAME_NAMEENTRY_READY_INIT:
        if (!(modeCtrl.courseFlags & (1 << 5)) && modeCtrl.gameType != GAMETYPE_MAIN_COMPETITION)
        {
            nlObjPutSetFadeColorBase(0.8f, 0.8f, 0.8f);
            avdisp_set_post_mult_color(0.8f, 0.8f, 0.8f, opacity);
        }
        else
        {
            nlObjPutSetFadeColorBase(lbl_801EEC80.unk4, lbl_801EEC80.unk8, lbl_801EEC80.unkC);
            avdisp_set_post_mult_color(lbl_801EEC80.unk4, lbl_801EEC80.unk8, lbl_801EEC80.unkC, opacity);
        }
        break;
    default:
        if (modeCtrl.courseFlags & COURSE_FLAG_MASTER)
        {
            nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);
            avdisp_set_post_mult_color(1.0f, 1.0f, 1.0f, opacity);
        }
        else
        {
            nlObjPutSetFadeColorBase(lbl_801EEC80.unk4, lbl_801EEC80.unk8, lbl_801EEC80.unkC);
            avdisp_set_post_mult_color(lbl_801EEC80.unk4, lbl_801EEC80.unk8, lbl_801EEC80.unkC, opacity);
        }
        break;
    }
}

void polydisp_set_nlobj_fade_color_from_goal_type(int goalType)
{
    float r, g, b;

    switch (goalType)
    {
    case 'R':
        r = 1.0f;
        b = 0.0f;
        g = 0.0f;
        break;
    case 'G':
        b = 0.0f;
        g = 1.0f;
        r = 0.0f;
        break;
    default:
        g = 0.0f;
        r = 0.0f;
        b = 1.0f;
        break;
    }
    nlObjPutSetFadeColorBase(lbl_801EEC80.unk4 * r, lbl_801EEC80.unk8 * g, lbl_801EEC80.unkC * b);
}

void fade_color_base_default(void)
{
    nlObjPutSetFadeColorBase(lbl_801EEC80.unk4, lbl_801EEC80.unk8, lbl_801EEC80.unkC);
    avdisp_set_post_mult_color(lbl_801EEC80.unk4, lbl_801EEC80.unk8, lbl_801EEC80.unkC, 1.0f);
}

void fade_color_base_default_set(float a, float b, float c)
{
    lbl_801EEC80.unk0 = 0.0f;
    lbl_801EEC80.unk4 = a;
    lbl_801EEC80.unk8 = b;
    lbl_801EEC80.unkC = c;
}

float get_height_mirror_plane(Vec *pos)
{
    Vec spC;

    mathutil_mtxA_tf_point(pos, &spC);
    spC.x -= polyDisp.unk1C.x;
    spC.y -= polyDisp.unk1C.y;
    spC.z -= polyDisp.unk1C.z;
    return mathutil_vec_dot_prod(&spC, &polyDisp.unk28);
}

float get_height_world_mirror_plane(Vec *pos)
{
    Vec spC;

    spC.x = pos->x - polyDisp.unk4.x;
    spC.y = pos->y - polyDisp.unk4.y;
    spC.z = pos->z - polyDisp.unk4.z;
    return mathutil_vec_dot_prod(&spC, &polyDisp.unk10);
}

float func_8000E53C(Vec *pos)
{
    Vec spC;

    spC.x = pos->x - polyDisp.unk34.x;
    spC.y = pos->y - polyDisp.unk34.y;
    spC.z = pos->z - polyDisp.unk34.z;
    return mathutil_vec_dot_prod(&spC, &polyDisp.unk40);
}
