/**
 * bg_end.c - Code for the ending cutscene background
 */
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "effect.h"
#include "gxcache.h"
#include "mathutil.h"
#include "stage.h"
#include "window.h"

static struct BGModelSearch endBgModelFind[] =
{
    { BG_MDL_CMP_PREFIX, "END_STG_CLOUD_" },
    { BG_MDL_CMP_FULL,   "END_YOUSAI_WATER_C" },
    { BG_MDL_CMP_FULL,   "END_YOUSAI_WATER_SPEC" },
    { BG_MDL_CMP_PREFIX, "END_FUNSUI_WATER_" },
    { BG_MDL_CMP_END,    NULL },
};

static struct BGModelSearch endBgObjFind[] =
{
    { BG_MDL_CMP_PREFIX, "END_PARADISE_" },
    { BG_MDL_CMP_END,    NULL },
};

static void lbl_800654F4(struct GCMMatState_Unit *arg0);
static void lbl_8006582C(struct GCMMatState_Unit *arg0);
static int model_find_proc(int, struct GMAModelEntry *);
static int obj_find_proc(int, struct StageBgObject *);

void bg_end_init(void)
{
    struct BGEndWork *work = backgroundInfo.work;

    bg_default_init();
    if (work->unk0 == 0)
    {
        find_background_gma_models(endBgModelFind, model_find_proc);
        work->unk0 = 1;
    }
    find_background_objects(
        decodedStageLzPtr->bgObjects,
        decodedStageLzPtr->bgObjectCount,
        endBgObjFind,
        obj_find_proc);
    find_background_objects(
        decodedStageLzPtr->fgObjects,
        decodedStageLzPtr->fgObjectCount,
        endBgObjFind,
        obj_find_proc);
    work->unk14 = lbl_8006582C;
    work->unk18 = lbl_800654F4;
    work->unk28 = 0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT());
    work->unk2C = 0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT());
    work->unk34 = 5.0f * (1.0f + RAND_FLOAT());
    work->unk38 = 5.0f * (1.0f + RAND_FLOAT());
    work->unk3C = 1.0f;
    work->unk58 = 0.002777777777777778 * (0.9f + 0.2f * RAND_FLOAT());
    work->unk5C = 0.002777777777777778 * (0.9f + 0.2f * RAND_FLOAT());
}

void bg_end_main(void)
{
    struct BGEndWork *work = backgroundInfo.work;
    float temp_f1;

    bg_default_main();
    if (debugFlags & 0xA)
        return;
    work->unk1C.x += work->unk28;
    work->unk1C.y += work->unk2C;
    temp_f1 = 1.0f + 0.005f * mathutil_sin(backgroundInfo.u_otherSeed << 8);
    work->unk40.x = work->unk34 * temp_f1;
    work->unk40.y = work->unk38 * temp_f1;
    work->unk40.z = 1.0f;
    work->unk4C.x += work->unk58;
    work->unk4C.y += work->unk5C;
}

void bg_end_finish(void) {}

void bg_end_draw(void)
{
    bg_default_draw();
}

void bg_end_interact(int arg0) {}

static void lbl_800654F4(struct GCMMatState_Unit *arg0)
{
    struct BGEndWork *work = backgroundInfo.work;
    struct TevStageInfo sp2C = arg0->unkC;
    float sp14[2][3];

    GXLoadTexObj_cached(work->waterSpecTex, sp2C.texMapId);
    GXLoadTexObj_cached(work->cloudTex, sp2C.texMapId + 1);
    sp14[0][0] = 0.0f;
    sp14[0][1] = 0.5f * mathutil_sin(backgroundInfo.u_otherSeed << 8);
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.0f;
    sp14[1][1] = 0.0f;
    sp14[1][2] = 0.5f * mathutil_sin((backgroundInfo.u_otherSeed << 8) + 0x4000);
    GXSetIndTexMtx(sp2C.unk1C, sp14, 0);
    mathutil_mtxA_push();
    mathutil_mtxA_from_mtx(work->unk64);
    mathutil_mtxA_set_translate(&work->unk4C);
    mathutil_mtxA_scale_s(12.0f);
    GXLoadTexMtxImm(mathutilData->mtxA, sp2C.unk8, GX_MTX2x4);
    mathutil_mtxA_from_identity();
    mathutil_mtxA_translate(&work->unk1C);
    mathutil_mtxA_scale(&work->unk40);
    GXLoadTexMtxImm(mathutilData->mtxA, sp2C.unk8 + 3, GX_MTX2x4);
    mathutil_mtxA_pop();
    GXSetTexCoordGen(sp2C.texCoordId, 1, 1, sp2C.unk8);
    GXSetTexCoordGen(sp2C.texCoordId + 1, 1, 4, sp2C.unk8 + 3);
    GXSetIndTexOrder(sp2C.tevIndStage, sp2C.texCoordId + 1, sp2C.texMapId + 1);
    GXSetTevIndirect(sp2C.tevStage, sp2C.tevIndStage, GX_ITF_8, GX_ITB_STU, sp2C.unk1C, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    GXSetTevSwapMode_cached(sp2C.tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder_cached(sp2C.tevStage, sp2C.texCoordId, sp2C.texMapId, GX_COLOR_NULL);
    GXSetTevColorIn_cached(sp2C.tevStage, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevColorOp_cached(sp2C.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(sp2C.tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevAlphaOp_cached(sp2C.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    sp2C.tevStage += 1;
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 2;
    sp2C.unk8 += 6;
    sp2C.unk1C += 1;
    sp2C.texMapId += 2;
    arg0->unkC = sp2C;
}

static void lbl_8006582C(struct GCMMatState_Unit *arg0)
{
    struct BGEndWork *work = backgroundInfo.work;
    struct TevStageInfo sp2C = arg0->unkC;
    float sp14[2][3];
    float temp_f2;

    GXLoadTexObj_cached(work->waterSpecTex, sp2C.texMapId);
    GXLoadTexObj_cached(work->cloudTex, sp2C.texMapId + 1);
    sp14[0][0] = 0.0f;
    sp14[0][1] = 0.5f * mathutil_sin(backgroundInfo.u_otherSeed << 8);
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.0f;
    sp14[1][1] = 0.0f;
    sp14[1][2] = 0.5f * mathutil_sin((backgroundInfo.u_otherSeed << 8) + 0x4000);
    GXSetIndTexMtx(sp2C.unk1C, sp14, 0);
    mathutil_mtxA_push();
    mathutil_mtxA_from_mtx(work->unk64);
    mathutil_mtxA_set_translate(&work->unk4C);
    mathutil_mtxA_scale_s(6.0f);
    GXLoadTexMtxImm(mathutilData->mtxA, sp2C.unk8, GX_MTX2x4);
    temp_f2 = 0.041666668f * backgroundInfo.u_otherSeed;
    mathutil_mtxA_from_translate_xyz(0.05f * temp_f2, temp_f2, 0.0f);
    mathutil_mtxA_scale_s(7.0f);
    GXLoadTexMtxImm(mathutilData->mtxA, sp2C.unk8 + 3, GX_MTX2x4);
    mathutil_mtxA_pop();
    GXSetTexCoordGen(sp2C.texCoordId, 1, 1, sp2C.unk8);
    GXSetTexCoordGen(sp2C.texCoordId + 1, 1, 4, sp2C.unk8 + 3);
    GXSetIndTexOrder(sp2C.tevIndStage, sp2C.texCoordId + 1, sp2C.texMapId + 1);
    GXSetTevIndirect(sp2C.tevStage, sp2C.tevIndStage, GX_ITF_8, GX_ITB_STU, sp2C.unk1C, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    GXSetTevSwapMode_cached(sp2C.tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder_cached(sp2C.tevStage, sp2C.texCoordId, sp2C.texMapId, GX_COLOR_NULL);
    GXSetTevColorIn_cached(sp2C.tevStage, GX_CC_TEXC, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
    GXSetTevColorOp_cached(sp2C.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(sp2C.tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
    GXSetTevAlphaOp_cached(sp2C.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    sp2C.tevStage += 1;
    sp2C.tevIndStage += 1;
    sp2C.texCoordId += 2;
    sp2C.unk8 += 6;
    sp2C.unk1C += 1;
    sp2C.texMapId += 2;
    arg0->unkC = sp2C;
}

static int model_find_proc(int index, struct GMAModelEntry *entry)
{
    struct BGEndWork *work = backgroundInfo.work;

    switch (index)
    {
    case 0:  // END_STG_CLOUD_
        work->cloudTex = &entry->model->texObjs[0];
        break;
    case 1:  // END_YOUSAI_WATER_C
        work->waterModel = entry->model;
        break;
    case 2:  // END_YOUSAI_WATER_SPEC
        work->waterSpecTex = &entry->model->texObjs[0];
        break;
    case 3:  // END_FUNSUI_WATER_
        work->fountainWaterModel = entry->model;
        break;
    }
    return 1;
}

static int obj_find_proc(int index, struct StageBgObject *bgObj)
{
    struct Effect effect;

    switch (index)
    {
    case 0:  // END_PARADISE_
        memset(&effect, 0, sizeof(effect));
        effect.type = ET_BGEND_WATER;
        effect.model = (void *)bgObj;
        spawn_effect(&effect);
        break;
    }
    return 1;
}
