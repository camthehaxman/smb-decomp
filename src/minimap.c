#include <stdlib.h>
#include <dolphin.h>

#include "global.h"
#include "ape_icon.h"
#include "ball.h"
#include "bitmap.h"
#include "camera.h"
#include "event.h"
#include "gxcache.h"
#include "gxutil.h"
#include "item.h"
#include "light.h"
#include "mathutil.h"
#include "minimap.h"
#include "mode.h"
#include "mt_effect.h"
#include "nl2ngc.h"
#include "ord_tbl.h"
#include "polydisp.h"
#include "sprite.h"
#include "stage.h"
#include "stobj.h"
#include "window.h"
#include "world.h"

#include "../data/common.nlobj.h"

struct MinimapInfo
{
    u32 unused;
    s32 state;
    s16 unk8;
    u16 unkA;
    float unkC;
    float unk10;
    float unk14;
    float unk18;
    int zoomLevel;
    float stageScale;
    Point3d focusPos;  // point in world space which the minimap is centered on
    s16 someX;
    s16 someY;
    s16 size;
    s16 rotation;
    s16 rotationVel;
    u16 unk3A;
    float unk3C;
    float unk40;
    float unk44;
    float unk48;
};

static struct MinimapInfo minimapInfo =
{
    0xFFFFFFFF,
    MINIMAP_STATE_0,
    0, 0,
    1.0f,
    -0.8666666746139526f,
    -0.17499999701976776f,
    0.23333333432674408f,
    0,
    0.26499998569488525f,
    { 0.0f, 0.0f, 0.0f },
    640, 448,
    0, 0,
    0, 0,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
};

void ev_minimap_init(void)
{
    MTEfcChangeAttributeITI(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_hideball_mark), 0x1BFFFFFF, 0x24000000);
    MTEfcChangeAttributeITI(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_mmapring), 0x1FFFFFFF, 0xE0000000);
    MTEfcChangeAttributePC(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_mmapbase), 0xF8FFFFFF, 0x02000000);
    MTEfcChangeAttributeITI(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_mmapbase), 0x1FFFFFFF, 0xE0000000);
    MTEfcChangeAttributeTI(NLOBJ_MODEL(g_commonNlObj, NLMODEL_common_mmapbase), 0x90EFFFFF, 0x08000000);
    if (gameSubmode == SMD_ADV_INFO_INIT)
    {
        minimapInfo.someY = 424;
        minimapInfo.unk10 = -0.76666665077209473f;
    }
    else
    {
        minimapInfo.someY = 448;
        minimapInfo.unk10 = -0.86666667461395264f;
    }
}

static struct
{
    Vec unk0;
    float unkC;
    u8 filler10[0x120];
} lbl_80292D30;

void ev_minimap_main(void)
{
    struct Ball *ball = currentBall;
    float f1;
    float f3;
    Point3d target;

    if (debugFlags & 0xA)
        return;
    if (minimapInfo.state == 0)
        return;

    switch (minimapInfo.state)
    {
    case MINIMAP_STATE_CLOSE:
        minimapInfo.size -= 6;
        minimapInfo.rotationVel -= 71;
        if (minimapInfo.size < 0)
        {
            minimapInfo.state = MINIMAP_STATE_0;
            minimapInfo.size = 0;
        }
        break;
    case MINIMAP_STATE_2:
        minimapInfo.rotationVel -= minimapInfo.rotationVel >> 4;
        break;
    case MINIMAP_STATE_OPEN:
        minimapInfo.size += 8;
        minimapInfo.rotationVel += 33;
        if (minimapInfo.size > 0x140)
        {
            minimapInfo.state = MINIMAP_STATE_2;
            minimapInfo.size = 0x140;
        }
        break;
    }

    minimapInfo.unk8 = minimapInfo.size * 0x3000 / 0x140 + 0x1000;
    minimapInfo.rotation += minimapInfo.rotationVel;
    f1 = lbl_80292D30.unkC;
    switch (minimapInfo.zoomLevel)
    {
    case 0:
        if (f1 > 18.0)
            f3 = 0.14722222089767456f;
        else
            f3 = 2.6500000000000004 / f1;
        target = ball->pos;
        minimapInfo.unk40 = 0.0f;
        minimapInfo.unk48 = 0.6f;
        break;
    case 1:
        f3 = 1.3250000000000002 / f1;
        target = lbl_80292D30.unk0;
        target.y = ball->pos.y;
        minimapInfo.unk40 = 1.0f;
        minimapInfo.unk48 = 0.0f;
        break;
    case 2:
    default:
        if (f1 > 18.0)
            f3 = 0.29444444179534912f;
        else
            f3 = 5.3000000000000007 / f1;
        target = ball->pos;
        minimapInfo.unk40 = 0.0f;
        minimapInfo.unk48 = 0.6f;
        break;
    }

    minimapInfo.stageScale += (f3 - minimapInfo.stageScale) * 0.1;
    minimapInfo.focusPos.x += (target.x - minimapInfo.focusPos.x) * 0.1;
    minimapInfo.focusPos.y += (target.y - minimapInfo.focusPos.y) * 0.1;
    minimapInfo.focusPos.z += (target.z - minimapInfo.focusPos.z) * 0.1;

    if (minimapInfo.unk3C < minimapInfo.unk40)
    {
        minimapInfo.unk3C += 0.066666666666666666;
        if (minimapInfo.unk3C > minimapInfo.unk40)
            minimapInfo.unk3C = minimapInfo.unk40;
    }
    else if (minimapInfo.unk3C > minimapInfo.unk40)
    {
        minimapInfo.unk3C -= 0.033333333333333333;
        if (minimapInfo.unk3C < minimapInfo.unk40)
            minimapInfo.unk3C = minimapInfo.unk40;
    }

    if (minimapInfo.unk44 < minimapInfo.unk48)
    {
        minimapInfo.unk44 += 0.066666666666666666;
        if (minimapInfo.unk44 > minimapInfo.unk48)
            minimapInfo.unk44 = minimapInfo.unk48;
    }
    else if (minimapInfo.unk44 > minimapInfo.unk48)
    {
        minimapInfo.unk44 -= 0.033333333333333333;
        if (minimapInfo.unk44 < minimapInfo.unk48)
            minimapInfo.unk44 = minimapInfo.unk48;
    }
}

void ev_minimap_dest(void)
{
    minimapInfo.state = 0;
    minimapInfo.size = 0;
    minimapInfo.rotationVel = 0;
    minimapInfo.unk3C = 0.0f;
}

void minimap_set_state(enum MinimapState state)
{
    switch (state)
    {
    case MINIMAP_STATE_0:
        minimapInfo.size = 0;
        minimapInfo.unk3C = 0.0f;
        break;
    case MINIMAP_STATE_2:
        minimapInfo.size = 0x140;
        break;
    case MINIMAP_STATE_OPEN:
        if (minimapInfo.size == 0)
            minimapInfo.rotationVel = (rand() & 0x3FF) + 0x200;
        break;
    case MINIMAP_STATE_4:
        state = minimapInfo.state;
        minimapInfo.size = 0;
        minimapInfo.unk3C = 0.0f;
        minimapInfo.zoomLevel = 0;
        minimapInfo.stageScale = 0.26499998569488525f;
        break;
    }

    if (minimapInfo.state == 0)
    {
        minimapInfo.focusPos.x = 0.0f;
        minimapInfo.focusPos.y = 0.0f;
        minimapInfo.focusPos.z = 0.0f;
    }
    minimapInfo.state = state;
}

void func_80084794(void *unused)
{
    func_800463E8(&lbl_80292D30.unk0, &lbl_80292D30.unkC);
}

void minimap_zoom(void)
{
    if (minimapInfo.state != MINIMAP_STATE_0)
    {
        minimapInfo.zoomLevel++;
        if (minimapInfo.zoomLevel > 2)
            minimapInfo.zoomLevel = 0;
    }
}

struct Struct800847FC
{
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
};

static void u_draw_some_quad(struct Struct800847FC *a)
{
    float x2 = a->unk0;
    float y1 = a->unk4;
    float x1 = a->unk8;
    float y2 = a->unkC;
    float z = a->unk10;

    gxutil_set_vtx_attrs(1 << GX_VA_POS);
    GXSetBlendMode_cached(1, 0, 1, 0);
    GXSetZMode_cached(1, 7, 1);
    {
        GXColor color = {0, 0, 0, 0};
        GXSetFog_cached(0, 0.0f, 100.0f, 0.1f, 20000.0f, color);
    }
    GXSetCullMode_cached(0);
    GXSetTevDirect(0);
    GXSetTevOrder_cached(0, 0xFF, 0xFF, 0xFF);
    GXSetTevKAlphaSel_cached(0, 0);
    GXSetTevColorIn_cached(0, 15, 15, 15, 15);
    GXSetTevColorOp_cached(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(0, 7, 7, 7, 6);
    GXSetTevAlphaOp_cached(0, 0, 0, 3, 1, 0);
    GXSetNumTevStages_cached(1);
    mathutil_mtxA_push();
    mathutil_mtxA_from_identity();
    GXLoadPosMtxImm(mathutilData->mtxA, 0);
    mathutil_mtxA_pop();
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(x1, y1, z);
    GXPosition3f32(x2, y1, z);
    GXPosition3f32(x2, y2, z);
    GXPosition3f32(x1, y2, z);
    GXSetZMode_cached(1, 3, 1);
}

static u32 s_speechBubbleColors[] =
{
    RGBA(255, 255, 255, 0),
    RGBA(255, 255, 255, 0),
    RGBA(255, 224, 128, 0),
    RGBA(255, 255, 255, 0),
};

void minimap_draw(void)
{
    float temp_f31;
    float var_f30;
    float temp_f29;
    float temp_f28;
    float scale;
    struct Ball *ball;
    struct World *world;
    int temp_r27;
    int var_r26;
    int temp_r25;
    int var_r24;
    int temp_r23;
    int temp_r22;
    int temp_r6;
    float new_var;
    float new_var4;
    Mtx sp124;
    Mtx44 spE4;
    Mtx44 spA4;
    struct Struct800847FC sp90;
    Point3d focusPos;
    NLsprarg params;
    Vec sp28;
    Vec sp1C;
    struct Struct800847FC sp8;

    ball = currentBall;
    world = currentWorld;
    if (minimapInfo.size != 0)
    {
        u_gxutil_set_fog_enabled(FALSE);
        switch (gameMode)
        {
        case MD_GAME:
            switch (modeCtrl.gameType)
            {
            case GAMETYPE_MAIN_NORMAL:
                change_current_camera_matrix(modeCtrl.currPlayer);
                break;
            default:
                change_current_camera_matrix(0);
                break;
            }
            break;
        default:
            change_current_camera_matrix(0);
            break;
        }

        temp_r6 = minimapInfo.size;
        temp_r6 == 0.0f;
        temp_r27 = minimapInfo.someX - (temp_r6 >> 1);
        temp_r25 = minimapInfo.someY - (temp_r6 >> 1);
        var_r26 = temp_r27 + temp_r6;
        var_r24 = temp_r25 + temp_r6;
        if (var_r26 > 640)
            var_r26 = 640;
        if (var_r24 > 480)
            var_r24 = 480;
        temp_r23 = temp_r25 * currRenderMode->xfbHeight / 480;
        temp_r22 = var_r24 * currRenderMode->xfbHeight / 480;
        MTXPerspective(spA4, 60.0f, 1.3333334f, 0.1f, 20000.0f);
        GXSetProjection(spA4, GX_PERSPECTIVE);
        GXSetViewport(0.0f, 0.0f, currRenderMode->fbWidth, currRenderMode->xfbHeight, 0.0f, 1.0f);
        GXSetScissor(0, 0, currRenderMode->fbWidth, currRenderMode->xfbHeight);
        new_var4 = 0.00024056263f;
        sp90.unk0 = (temp_r27 - 320) * new_var4;
        sp90.unk4 = (240 - (temp_r25 - 1)) * new_var4;
        sp90.unk8 = (var_r26 - 320) * new_var4;
        sp90.unkC = (240 - var_r24) * new_var4;
        sp90.unk10 = -0.1f;
        u_draw_some_quad(&sp90);
        mathutil_mtx_copy(mathutilData->mtxB, sp124);
        MTXPerspective(spE4, 60.0f, 1.3333334f, 0.1f, 20000.0f);
        spE4[0][2] += -0.57735026f * (1.3333334f * (spE4[0][0] * minimapInfo.unkC));
        spE4[1][2] += -0.57735026f * (spE4[1][1] * minimapInfo.unk10);
        GXSetProjection(spE4, GX_PERSPECTIVE);
        push_light_group();
        mathutil_mtxA_from_mtxB();
        load_light_group_uncached(1);
        nlObjPutSetFadeColorBase(1.0f, 1.0f, 1.0f);

        scale = (415.0 * minimapInfo.size) / 320.0;
        mathutil_mtxA_from_translate_xyz(0.0f, 0.0f, -1000.0f);
        mathutil_mtxA_rotate_z(minimapInfo.rotation);
        mathutil_mtxA_scale_xyz(scale, scale, scale);
        nlSetScaleFactor(scale);
        nl2ngc_draw_model_sort_none_alt2(g_commonNlObj->models[NLMODEL_common_mmapbase]);

        scale = (0.46200000062584873 * minimapInfo.size) / 320.0;
        mathutil_mtxA_from_translate_xyz(0.0f, 0.0f, -1.1f);
        mathutil_mtxA_rotate_z(minimapInfo.rotation);
        mathutil_mtxA_scale_xyz(scale, scale, scale);
        nlSetScaleFactor(scale);
        nl2ngc_draw_model_sort_none_alt2(g_commonNlObj->models[NLMODEL_common_mmapring]);

        GXSetViewport(temp_r27, temp_r23, (float)var_r26 - (float)temp_r27, (float)temp_r22 - (float)temp_r23, 0.0f, 1.0f);
        GXSetScissor(temp_r27, temp_r23, var_r26 - temp_r27, temp_r22 - temp_r23);

        new_var = 0.057735026f;
        !&minimapInfo.unk10;  // needed to match
        temp_f29 = minimapInfo.unkC + minimapInfo.unk14;
        temp_f28 = minimapInfo.unk10 + minimapInfo.unk18;
        MTXFrustum(
            spE4,
            new_var * (((240.0f - temp_r25) / 240.0f) - temp_f28),
            new_var * (((240.0f - var_r24) / 240.0f) - temp_f28),
            new_var * (1.3333334f * (((temp_r27 - 320.0f) / 320.0f) - temp_f29)),
            new_var * (1.3333334f * (((var_r26 - 320.0f) / 320.0f) - temp_f29)),
            0.1f,
            20000.0f);
        GXSetProjection(spE4, GX_PERSPECTIVE);

        scale = minimapInfo.stageScale;
        mathutil_mtxA_from_identity();
        mathutil_mtxA_rotate_x(minimapInfo.unk8);
        mathutil_mtxA_rotate_y(-currentCamera->rotY);
        if (eventInfo[EVENT_WORLD].state == EV_STATE_RUNNING)
        {
            mathutil_mtxA_rotate_x(world->xrot);
            mathutil_mtxA_rotate_z(world->zrot);
        }
        focusPos.x = 0.0f;
        focusPos.y = 0.0f;
        focusPos.z = -10.0f;
        mathutil_mtxA_set_translate(&focusPos);
        mathutil_mtxA_scale_xyz(scale, scale, scale);
        focusPos = minimapInfo.focusPos;
        mathutil_mtxA_translate_neg(&focusPos);
        mathutil_mtxA_to_mtx(mathutilData->mtxB);
        mathutil_mtxA_scale_s(minimapInfo.unk44 / scale);
        mathutil_mtxA_to_mtx(userWork->matrices[2]);
        mathutil_mtxA_from_mtxB();

        var_f30 = 0.0f;
        if ((minimapInfo.size - 160) / 160.0f > 0.0f)
            var_f30 = (minimapInfo.size - 160) / 160.0f;

        params.listType = NLSPR_LISTTYPE_TRANS;
        params.offset_color = 0;
        params.u0 = 0.0f;
        params.v0 = 0.0f;
        params.u1 = 1.0f;
        params.v1 = 1.0f;
        u_math_unk16(&ball->pos, &sp28, 0.57735026f);

        temp_f31 = minimapInfo.unk3C;
        if (temp_f31 > 0.0)
        {
            sp28.z = -2.0f;
            sp1C.x = -sp28.y - (0.5 * sp28.x);
            sp1C.y = sp28.x - (0.5 * sp28.y);
            sp1C.z = 0.0f;
            params.ang = mathutil_atan2(sp1C.y, (4.0 / 3.0) * sp1C.x) + 0x4000;
            if (sp1C.x == 0.0 && sp1C.y == 0.0)
                sp1C.y = 1.0f;
            mathutil_vec_set_len(&sp1C, &sp1C, 0.1125 * temp_f31);
            sp28.x += sp1C.x;
            sp28.y += sp1C.y;
            params.sprno = BMP_NML_fukidashi;
            params.base_color = s_speechBubbleColors[playerCharacterSelection[modeCtrl.currPlayer]];
            params.trnsl = 0.75 * var_f30;
            params.x = 320.0 + (320.0 * (sp28.x + temp_f29));
            params.y = 240.0 + (-240.0 * (sp28.y + temp_f28));
            params.z = -sp28.z;
            params.zm_x = temp_f31;
            params.zm_y = temp_f31;
            params.attr = NLSPR_DISP_CC | NLSPR_UNKFLAG_17;
            nlSprPut(&params);

            mathutil_vec_set_len(&sp1C, &sp1C, 0.025 * temp_f31);
            sp28.x = sp28.x + sp1C.x;
            sp28.y += sp1C.y;
            mathutil_mtxA_translate(&ball->pos);
            mathutil_mtxA_scale_xyz(1.25 * temp_f31, 1.25 * temp_f31, 1.25 * temp_f31);
            nlSetScaleFactor(1.25 * scale * temp_f31);
            nlObjPut(g_commonNlObj->models[NLMODEL_common_BALL_BLK]);
        }

        params.sprno = u_get_monkey_bitmap_id(0, 0, playerCharacterSelection[modeCtrl.currPlayer]);
        params.base_color = 0xFFFFFFFF;
        params.trnsl = (double)var_f30;
        params.x = 320.0 + (320.0 * (sp28.x + temp_f29));
        params.y = 240.0 + (-240.0 * (sp28.y + temp_f28));
        params.z = -sp28.z - 0.5;
        params.ang = 0;
        params.zm_x = 0.28846154f;
        params.zm_y = 0.1875f;
        params.attr = NLSPR_DISP_CC | NLSPR_UNKFLAG_12;
        nlSprPut(&params);

        u_set_some_minimap_light_param(minimapInfo.stageScale);
        mathutil_mtxA_from_mtxB();
        load_light_group_uncached(3);
        if (eventInfo[EVENT_STAGE].state == EV_STATE_RUNNING
         || eventInfo[EVENT_STAGE].state == EV_STATE_SUSPENDED)
            stage_draw();
        if (minimapInfo.unk44 > 0.0f && eventInfo[EVENT_ITEM].state == EV_STATE_RUNNING)
            item_draw();
        if (eventInfo[EVENT_STOBJ].state == EV_STATE_RUNNING)
            stobj_draw();
        if (minimapInfo.size == 320)
        {
            mathutil_mtxA_from_mtxB();
            mathutil_mtxA_translate(&ball->pos);
            mathutil_mtxA_sq_from_identity();
            mathutil_mtxA_rotate_z((globalAnimTimer << 8) + (globalAnimTimer << 7));
            mathutil_mtxA_translate_xyz(0.0f, 0.0f, 0.5 * ball->modelScale * scale);
            mathutil_mtxA_scale_xyz(
                2.0 * (ball->modelScale * scale),
                2.0 * (ball->modelScale * scale),
                2.0 * (ball->modelScale * scale));
            nlSetScaleFactor(2.0 * (ball->modelScale * scale));
            nl2ngc_draw_model_sort_translucent_alt(g_commonNlObj->models[NLMODEL_common_hideball_mark]);
        }
        ord_tbl_draw_nodes();
        GXSetProjection(spA4, GX_PERSPECTIVE);
        GXSetViewport(0.0f, 0.0f, currRenderMode->fbWidth, currRenderMode->xfbHeight, 0.0f, 1.0f);
        GXSetScissor(0, 0, currRenderMode->fbWidth, currRenderMode->xfbHeight);
        fade_color_base_default();
        new_var4 = 48.11012f;
        sp8.unk0 = (temp_r27 - 320) * new_var4;
        sp8.unk4 = (240 - (temp_r25 - 1)) * new_var4;
        sp8.unk8 = (var_r26 - 320) * new_var4;
        sp8.unkC = (240 - var_r24) * new_var4;
        sp8.unk10 = -19999.0f;
        u_draw_some_quad(&sp8);
        default_camera_env();
        u_set_some_minimap_light_param(1.0f);
        pop_light_group();
        u_gxutil_fog_something_2();
    }
}
