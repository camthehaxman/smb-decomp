/**
 * bg_space.c - Code for the space background
 */
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "avdisp.h"
#include "background.h"
#include "effect.h"
#include "event.h"
#include "gxcache.h"
#include "info.h"
#include "lens_flare.h"
#include "mathutil.h"
#include "mode.h"
#include "polydisp.h"
#include "stage.h"
#include "window.h"

static struct BGModelSearch spaceBgModelFind[] =
{
    { BG_MDL_CMP_PREFIX, "SPA_INSEKI_" },
    { BG_MDL_CMP_FULL,   "SPA_DOSEI_IND_MAP" },
    { BG_MDL_CMP_END },
};

static struct BGModelSearch spaceBgObjFind[] =
{
    { BG_MDL_CMP_FULL, "SPA_YOUSAI_CORE" },
    { BG_MDL_CMP_FULL, "SPA_DOSEI" },
    { BG_MDL_CMP_END },
};

static void lbl_800609AC(struct GCMMatState_Unit *);
static int model_find_proc(int, struct GMAModelEntry *);
static int obj_find_proc(int, struct StageBgObject *);

void lens_flare_set_scale(float);

void bg_space_init(void)
{
    Point3d spB4;
    struct Effect effect;
    struct BGSpaceWork *work = backgroundInfo.work;
    int i;
    float temp_f31;

    bg_default_init();
    lens_flare_init();
    switch (currStageId)
    {
    case ST_101_BLUR_BRIDGE:
        spB4.x = -913.7f;
        spB4.y = 464.7f;
        spB4.z = 1066.2f;
        break;
    case ST_105_PUZZLE:
        spB4.x = -1005.8f;
        spB4.y = 469.1f;
        spB4.z = 974.0f;
        break;
    case ST_109_FACTORY:
        spB4.x = 4200.0f;
        spB4.y = 1407.3f;
        spB4.z = 0.0f;
        break;
    case ST_112_SANCTUARY:
        spB4.x = 2089.0f;
        spB4.y = 718.6f;
        spB4.z = 10.6f;
        break;
    default:
        spB4.x = 1350.0f;
        spB4.y = 470.0f;
        spB4.z = 0.0f;
        break;
    }
    lens_flare_set_light_position(&spB4);
    lens_flare_set_scale(0.5f);
    backgroundInfo.unk8 |= 1;
    if (work->unk0 == 0)
    {
        find_background_gma_models(spaceBgModelFind, model_find_proc);
        work->unk0 = 1;
    }
    work->corePos.x = 0.0f;
    work->corePos.y = 0.0f;
    work->corePos.z = 0.0f;
    work->coreScale = 230.0f;
    work->saturn = NULL;

    find_background_objects(
        decodedStageLzPtr->bgObjects,
        decodedStageLzPtr->bgObjectCount,
        spaceBgObjFind,
        obj_find_proc);
    find_background_objects(
        decodedStageLzPtr->fgObjects,
        decodedStageLzPtr->fgObjectCount,
        spaceBgObjFind,
        obj_find_proc);

    switch (modeCtrl.gameType)
    {
    case GAMETYPE_MAIN_COMPETITION:
        if (gameMode == MD_SEL)
            work->unk3C = 1.0f;
        else
            work->unk3C = (5 - modeCtrl.playerCount) / 4.0f;
        break;
    default:
        work->unk3C = 1.0f;
        break;
    }

    work->unk38 = 15;
    memset(&effect, 0, sizeof(effect));
    effect.type = ET_METEO_FIX;
    i = work->unk3C * ((rand() & 0xF) + 48);
    while (i > 0)
    {
        float temp_f1;

        effect.model = work->meteorModels[(rand() & 0x7FFF) % work->meteorModelCount];
        effect.scale.x = 12.0f * RAND_FLOAT() + 1.0f;
        effect.pos.x = (RAND_FLOAT() - 0.5f) * 2000.0f;
        effect.pos.y = (RAND_FLOAT() - 0.5f) * 500.0f + effect.pos.x * -0.5f;
        effect.pos.z = (RAND_FLOAT() - 0.5f) * 6000.0f;
        temp_f1 = effect.pos.x;
        if (mathutil_vec_len(&effect.pos) < 1.7391304f * work->coreScale)
        {
            if (abs(effect.pos.y) < 1.1920929e-7f)
                effect.pos.y = 0.01f;
            mathutil_vec_set_len(&effect.pos, &effect.pos, 1.7391304f * work->coreScale * (1.0f + RAND_FLOAT()));
        }
        effect.rotX = rand() & 0x7FFF;
        effect.rotY = rand() & 0x7FFF;
        effect.rotZ = rand() & 0x7FFF;
        temp_f31 = 2.0f / effect.scale.x;
        effect.unkA0 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA2 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA4 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.colorFactor = 1.0f;
        spawn_effect(&effect);
        i -= 1;
    }
    memset(&effect, 0, sizeof(effect));
    effect.type = ET_METEO;
    effect.colorFactor = 1.0f;
    i = work->unk3C * ((rand() & 0xF) + 64);
    while (i > 0)
    {
        effect.model = work->meteorModels[(rand() & 0x7FFF) % work->meteorModelCount];
        effect.scale.x = RAND_FLOAT() * 8.0f + 1.0f;
        effect.pos.x = (RAND_FLOAT() - 0.5f);
        effect.pos.y = (RAND_FLOAT() - 0.5f) * 0.5f + effect.pos.x * -0.75f;
        effect.pos.z = (RAND_FLOAT() - 0.5f);
        if (effect.pos.y == 0.0f)
            effect.pos.y = 0.1f;
        mathutil_vec_set_len(&effect.pos, &effect.pos, 1.3043479f * work->coreScale * (1.0f + RAND_FLOAT()));
        effect.unk94.x = 0.0f;
        effect.unk94.y = 0.0f;
        effect.unk94.z = 1.2000000476837158 + 0.20000000298023224 * (RAND_FLOAT() - 0.5);
        effect.vel.x = effect.unk94.x + 0.2f * (RAND_FLOAT() - 0.5f);
        effect.vel.y = effect.unk94.y + 0.2f * (RAND_FLOAT() - 0.5f);
        effect.vel.z = effect.unk94.z + 0.5f * RAND_FLOAT();
        effect.rotX = rand() & 0x7FFF;
        effect.rotY = rand() & 0x7FFF;
        effect.rotZ = rand() & 0x7FFF;
        temp_f31 = 1.0f / effect.scale.x;
        effect.unkA0 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA2 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unkA4 = temp_f31 * ((rand() & 0x3FF) - 0x200);
        effect.unk52 = (rand() & 0xFF) - 0x80;
        effect.unk54 = (rand() & 0xFF) - 0x80;
        effect.unk56 = (rand() & 0xFF) - 0x80;
        effect.colorFactor = 1.0f;
        spawn_effect(&effect);
        i -= 1;
    }
    work->unk54 = 0.0033333334f * (0.9f + 0.2f * RAND_FLOAT());
    work->unk58 = 0.0018518518f * (0.9f + 0.2f * RAND_FLOAT());
    work->unk5C = 0.0f;
    work->unk60 = ((0.2f * RAND_FLOAT()) + 0.9f) * 3.0f;
    work->unk94[0][0] = 0.05f * RAND_FLOAT() + 0.2f;
    work->unk94[0][1] = 0.0f;
    work->unk94[0][2] = 0.0f;
    work->unk94[1][0] = 0.0f;
    work->unk94[1][1] = 0.05f * RAND_FLOAT() + 0.2f;
    work->unk94[1][2] = 0.0f;
}

void bg_space_main(void)
{
    struct BGSpaceWork *work = backgroundInfo.work;

    lens_flare_main();
    bg_default_main();
    if (!(debugFlags & 0xA) || (eventInfo[EVENT_VIEW].state == EV_STATE_RUNNING))
    {
        work->unk38--;
        if (work->unk38 < 0)
        {
            struct Effect effect;
            float var_f31;
            float temp_f31;

            work->unk38 = (15.0f * (1.0f + RAND_FLOAT())) / work->unk3C;
            memset(&effect, 0, sizeof(effect));
            effect.type = ET_METEO;
            effect.model = work->meteorModels[(rand() & 0x7FFF) % work->meteorModelCount];
            effect.scale.x = RAND_FLOAT() * 8.0f + 1.0f;
            effect.pos.x = effect.scale.x * (175.0f * (RAND_FLOAT() - 0.5f));
            effect.pos.y = effect.scale.x * (150.0f * (RAND_FLOAT() - 0.5f)) + effect.pos.x * -0.75f;
            effect.pos.z = 1.3043479f * -work->coreScale * effect.scale.x;
            if (infoWork.timerMax == 0)
                var_f31 = 1.0f;
            else
                var_f31 = 2.0f - ((float)infoWork.timerCurr / (float)infoWork.timerMax);
            effect.unk94.x = 0.0f;
            effect.unk94.y = 0.0f;
            effect.unk94.z = var_f31 * (1.2000000476837158 + 0.20000000298023224 * (RAND_FLOAT() - 0.5));
            effect.vel.x = effect.unk94.x + 0.2f * (RAND_FLOAT() - 0.5f);
            effect.vel.y = effect.unk94.y + 0.2f * (RAND_FLOAT() - 0.5f);
            effect.vel.z = effect.unk94.z + 0.5f * RAND_FLOAT();
            effect.rotX = rand() & 0x7FFF;
            effect.rotY = rand() & 0x7FFF;
            effect.rotZ = rand() & 0x7FFF;
            temp_f31 = 1.0f / effect.scale.x;
            effect.unkA0 = (s16) (temp_f31 * (f32) ((rand() & 0x3FF) - 0x200));
            effect.unkA2 = (s16) (temp_f31 * (f32) ((rand() & 0x3FF) - 0x200));
            effect.unkA4 = (s16) (temp_f31 * (f32) ((rand() & 0x3FF) - 0x200));
            effect.unk52 = (rand() & 0xFF) - 0x80;
            effect.unk54 = (rand() & 0xFF) - 0x80;
            effect.unk56 = (rand() & 0xFF) - 0x80;
            effect.colorFactor = 0.0f;
            spawn_effect(&effect);
        }
        work->unk48.x += work->unk54;
        work->unk48.y += work->unk58;
        work->unk48.z += work->unk5C;
        mathutil_mtxA_from_translate(&work->unk48);
        mathutil_mtxA_scale_s(work->unk60);
        mathutil_mtxA_to_mtx(work->unk64);
    }
}

void bg_space_finish(void) {}

void bg_space_draw(void)
{
    struct StageBgObject *saturnObj = ((struct BGSpaceWork *)backgroundInfo.work)->saturn;
    float scale;
    u8 unused[8];

    if (saturnObj != NULL)
    {
        saturnObj->flags &= 0xFFFEFFFF;
        mathutil_mtxA_from_mtx(userWork->matrices[0]);
        mathutil_mtxA_translate(&saturnObj->pos);
        mathutil_mtxA_rotate_z(saturnObj->rotZ);
        mathutil_mtxA_rotate_y(saturnObj->rotY);
        mathutil_mtxA_rotate_x(saturnObj->rotX);
        mathutil_mtxA_scale(&saturnObj->scale);
        GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
        GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
        scale = MAX(MAX(saturnObj->scale.x, saturnObj->scale.y), saturnObj->scale.z);
        avdisp_set_bound_sphere_scale(scale);
        if (!(polyDisp.flags & 4))
        {
            u_avdisp_set_some_func_1(lbl_800609AC);
            avdisp_draw_model_culled_sort_translucent(saturnObj->model);
            u_avdisp_set_some_func_1(NULL);
        }
        else
            avdisp_draw_model_culled_sort_translucent(saturnObj->model);
    }
    bg_default_draw();
}

void bg_space_interact(int arg0) {}

static struct TevStageInfo lbl_8027CC28;

static void lbl_800609AC(struct GCMMatState_Unit *arg0)
{
    struct BGSpaceWork *work = backgroundInfo.work;
    struct TevStageInfo sp14 = arg0->unkC;

    if (arg0->unk0 == 0
     && sp14.tevStage == lbl_8027CC28.tevStage
     && sp14.texCoordId == lbl_8027CC28.texCoordId
     && sp14.texMtxId == lbl_8027CC28.texMtxId
     && sp14.texMapId == lbl_8027CC28.texMapId
     && sp14.tevIndStage == lbl_8027CC28.tevIndStage
     && sp14.indTexMtxId == lbl_8027CC28.indTexMtxId)
    {
        sp14.texCoordId += 1;
        sp14.texMtxId += 3;
        sp14.texMapId += 1;
        sp14.tevIndStage += 1;
        sp14.indTexMtxId += 1;
        arg0->unkC = sp14;
        return;
    }
    lbl_8027CC28 = sp14;
    GXLoadTexObj_cached(work->saturnIndMap, sp14.texMapId);
    GXSetTexCoordGen(sp14.texCoordId, GX_TG_MTX2x4, GX_TG_TEX0, sp14.texMtxId);
    GXLoadTexMtxImm(work->unk64, sp14.texMtxId, GX_MTX2x4);
    GXSetIndTexOrder(sp14.tevIndStage, sp14.texCoordId, sp14.texMapId);
    GXSetTevIndirect(sp14.tevStage - 1, sp14.tevIndStage, GX_ITF_8, GX_ITB_STU, sp14.indTexMtxId, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    GXSetIndTexMtx(sp14.indTexMtxId, work->unk94, -3);
    sp14.texCoordId += 1;
    sp14.texMtxId += 3;
    sp14.texMapId += 1;
    sp14.tevIndStage += 1;
    sp14.indTexMtxId += 1;
    arg0->unkC = sp14;
}

static int model_find_proc(int index, struct GMAModelEntry *entry)
{
    struct BGSpaceWork *work = backgroundInfo.work;

    switch (index)
    {
    case 0:  // SPA_INSEKI_ "meteorite"
        if (work->meteorModelCount < 8)
        {
            work->meteorModels[work->meteorModelCount] = entry->model;
            work->meteorModelCount++;
        }
        break;
    case 1:  // SPA_DOSEI_IND_MAP
        work->saturnIndMap = &entry->model->texObjs[0];
        break;
    }
    return 1;
}

static int obj_find_proc(int index, struct StageBgObject *bgObj)
{
    struct BGSpaceWork *work = backgroundInfo.work;

    switch (index)
    {
    case 0:  // SPA_YOUSAI_CORE
        work->corePos = bgObj->pos;
        work->coreScale = MAX(MAX(bgObj->scale.x, bgObj->scale.y), bgObj->scale.z);
        work->coreScale *= 230.0f;
        break;
    case 1:  // SPA_DOSEI  "Saturn"
        if (bgObj->model != NULL)
            work->saturn = bgObj;
        break;
    }
    return 1;
}
