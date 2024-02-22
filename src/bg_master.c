#include <stdlib.h>
#include <string.h>

#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "effect.h"
#include "gma.h"
#include "gxcache.h"
#include "info.h"
#include "mathutil.h"
#include "mode.h"
#include "polydisp.h"
#include "stage.h"
#include "window.h"

struct BGModelSearch masterBgModelFind[] =
{
    { BG_MDL_CMP_PREFIX, "MST_STG_CLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_MINI_CLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_SPRA_CLOUD_" },
    { BG_MDL_CMP_FULL,   "MST_YOUSAI_WATER_C" },
    { BG_MDL_CMP_FULL,   "MST_YOUSAI_WATER_SPEC" },
    { BG_MDL_CMP_END,    NULL },
};

struct BGModelSearch masterBgObjFind[] =
{
    { BG_MDL_CMP_PREFIX, "MST_BOTTOMCLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_PARADISE_CLOUD_" },
    { BG_MDL_CMP_PREFIX, "MST_CLOUD_GEN_" },
    { BG_MDL_CMP_FULL,   "MST_PARADISE_A" },
    { BG_MDL_CMP_FULL,   "MST_PARADISE_B" },
    { BG_MDL_CMP_END,    NULL },
};

static void lbl_80063AD8(struct GCMMatState_Unit *);
static int model_find_proc(int, struct GMAModelEntry *);
static int obj_find_proc(int, struct StageBgObject *);

void bg_master_init(void)
{
    Vec sp8;
    int i;
    int i2;
    struct BGMasterWork *work = backgroundInfo.work;
    struct BGMasterWork_sub *var_r29;
    struct BGMasterWork_sub2 *var_r29_2;

    bg_default_init();
    if (work->unk0 == 0)
    {
        work->miniCloudModelsCount = 0;
        find_background_gma_models(masterBgModelFind, model_find_proc);
        work->unk0 = 1;
    }
    work->unk8 = 0;
    work->unk170 = 0;
    find_background_objects(decodedStageLzPtr->bgObjects, decodedStageLzPtr->bgObjectCount, masterBgObjFind, obj_find_proc);
    find_background_objects(decodedStageLzPtr->fgObjects, decodedStageLzPtr->fgObjectCount, masterBgObjFind, obj_find_proc);
    if (work->unk8 != 0)
    {
        work->unk16C = 0;
        var_r29 = work->unkC;
        for (i = work->unk8; i > 0; i--, var_r29++)
        {
            var_r29->unk4.x = RAND_FLOAT();
            var_r29->unk4.y = RAND_FLOAT();
            var_r29->unk4.z = RAND_FLOAT();
            mathutil_mtxA_from_rotate_z(8192.0f * (RAND_FLOAT() - 0.5f));
            sp8.x = 0.0011904762f * (0.9f + (0.2f * RAND_FLOAT()));
            sp8.y = 0.0f;
            sp8.z = 0.0f;
            mathutil_mtxA_tf_vec(&sp8, &var_r29->unk1C);
            var_r29->unk10 = var_r29->unk1C;
        }
    }
    if (work->unk170 != 0)
    {
        var_r29_2 = work->unk174;
        for (i2 = work->unk170; i2 > 0; i2--, var_r29_2++)
        {
            var_r29_2->unk4.x = RAND_FLOAT();
            var_r29_2->unk4.y = 0.1f + (0.1f * RAND_FLOAT());
            var_r29_2->unk4.z = var_r29_2->unk4.y;
            var_r29_2->unk10 = rand() & 0x7FFF;
            var_r29_2->unk12 = (int)(128.0f * RAND_FLOAT()) + 64;
            if (i2 & 1)
                var_r29_2->unk12 = -var_r29_2->unk12;
            var_r29_2->unk20.x = 0.004166667f * (1.0f + (0.2f * RAND_FLOAT()));
            var_r29_2->unk20.y = 0.0013888889f * (RAND_FLOAT() - 0.5f);
            var_r29_2->unk2C.x = 0.9f + (0.2f * RAND_FLOAT());
            var_r29_2->unk2C.y = 0.9f + (0.2f * RAND_FLOAT());
            var_r29_2->unk2C.z = 1.0f;
        }
    }
    work->unkB14 = lbl_80063AD8;
    work->unkB24.x = (0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT()));
    work->unkB24.y = (0.005555555555555556 * (0.8f + 0.4f * RAND_FLOAT()));
    work->unkB30.x = 5.0f * (1.0f + RAND_FLOAT());
    work->unkB30.y = 5.0f * (1.0f + RAND_FLOAT());
    work->unkB30.z = 1.0f;
    work->unkB54.x = (0.001388888888888889 * (0.9f + (0.2f * RAND_FLOAT())));
    work->unkB54.y = (0.001388888888888889 * (0.9f + (0.2f * RAND_FLOAT())));
}

void bg_master_main(void)
{
    Vec spB4;
    struct Effect effect;
    float temp_f1_4;
    int var_r28;
    struct BGMasterWork *work;
    int var_r29;
    struct BGMasterWork_sub *var_r31;
    int var_r28_2;
    struct BGMasterWork_sub2 *var_r27;
    int var_r26;

    work = backgroundInfo.work;
    if (polyDisp.flags & 1)
        var_r29 = 0x10;
    else if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
        var_r29 = 1 << (modeCtrl.unk30 - 1);
    else
        var_r29 = 1;
    bg_default_main();
    if (debugFlags & 0xA)
        return;
    if (work->unk8 != 0)
    {
        if (work->unk16C == 0 && infoWork.timerCurr < 660.0f)
        {
            work->unk16C = 1;
            var_r26 = 1;
        }
        else
            var_r26 = 0;
        var_r31 = work->unkC;
        for (var_r28 = work->unk8; var_r28 > 0; var_r28--, var_r31++)
        {
            if (var_r26 != 0)
            {
                mathutil_mtxA_from_rotate_z(8192.0f * (RAND_FLOAT() - 0.5f));
                spB4.x = 0.0023809525f * (0.9f + (0.2f * RAND_FLOAT()));
                spB4.y = 0.0f;
                spB4.z = 0.0f;
                mathutil_mtxA_tf_vec(&spB4, &var_r31->unk1C);
            }
            var_r31->unk10.x += 0.05f * (var_r31->unk1C.x - var_r31->unk10.x);
            var_r31->unk10.y += 0.05f * (var_r31->unk1C.y - var_r31->unk10.y);
            var_r31->unk4.x += var_r31->unk10.x;
            var_r31->unk4.y += var_r31->unk10.y;
            mathutil_mtxA_from_translate(&var_r31->unk4);
            mathutil_mtxA_to_mtx(var_r31->unk28);
        }
    }
    if (work->unk170 != 0)
    {
        memset(&effect, 0, sizeof(effect));
        effect.type = ET_BGMST_GEN_CLOUD;
        effect.model = work->miniCloudModels[(rand() & 0x7FFF) % work->miniCloudModelsCount];
        var_r27 = work->unk174;
        for (var_r28_2 = work->unk170; var_r28_2 > 0; var_r28_2--, var_r27++)
        {
            if (var_r27->unk0->flags & var_r29)
            {
                var_r27->unk10 += var_r27->unk12;
                var_r27->unk4.x += var_r27->unk4.y;
                while (var_r27->unk4.x > 1.0f)
                {
                    effect.pos = var_r27->unk0->pos;
                    spB4.x = 0.0f;
                    spB4.y = 2.5f * (var_r27->unk4.y * (0.5f + RAND_FLOAT()));
                    spB4.z = 7.5f * (var_r27->unk4.y * (1.0f + RAND_FLOAT()));
                    if (rand() & 1)
                        mathutil_mtxA_from_rotate_y(var_r27->unk10);
                    else
                        mathutil_mtxA_from_rotate_y(var_r27->unk10 + 0x8000);
                    mathutil_mtxA_tf_vec(&spB4, &spB4);
                    effect.vel = spB4;
                    mathutil_vec_set_len(&spB4, &spB4, 25.0f);
                    effect.pos.x += spB4.x;
                    effect.pos.y += spB4.y;
                    effect.pos.z += spB4.z;
                    effect.rotZ = rand() & 0x7FFF;
                    effect.unk56 = 768.0f * (RAND_FLOAT() - 0.5f);
                    effect.scale.x = 0.25f + (0.5f * RAND_FLOAT());
                    effect.unk64 = var_r27->unk0->pos;
                    effect.unk70.x = 0.0f;
                    effect.unk70.y = 1.0f;
                    effect.unk70.z = 0.0f;
                    effect.unk88.x = 100.0f * var_r27->unk0->scale.x;
                    spawn_effect(&effect);
                    var_r27->unk4.x -= 1.0f;
                }
                var_r27->unk4.y += 0.05f * (var_r27->unk4.y - var_r27->unk4.z);
                var_r27->unk14.x += var_r27->unk20.x;
                var_r27->unk14.y += var_r27->unk20.y;
                mathutil_mtxA_from_translate(&var_r27->unk14);
                mathutil_mtxA_scale(&var_r27->unk2C);
                mathutil_mtxA_to_mtx(var_r27->unk38);
                mathutil_mtxA_scale_xyz(1.05f, -1.0f, 1.0f);
                mathutil_mtxA_to_mtx(var_r27->unk68);
            }
        }
    }
    work->unkB18.x += work->unkB24.x;
    work->unkB18.y += work->unkB24.y;
    temp_f1_4 = 1.0f + (0.05f * mathutil_sin(backgroundInfo.u_otherSeed << 8));
    work->unkB3C.x = work->unkB30.x * temp_f1_4;
    work->unkB3C.y = work->unkB30.y * temp_f1_4;
    work->unkB3C.z = 1.0f;
    work->unkB48.x += work->unkB54.x;
    work->unkB48.y += work->unkB54.y;
}

void bg_master_finish(void) {}

void bg_master_draw(void)
{
    struct BGMasterWork_sub *var_r28;
    int var_r28_2;
    struct BGMasterWork *temp_r31;
    int var_r30;
    int var_r27;
    struct BGMasterWork_sub2 *var_r27_2;
    int var_ctr;
    struct StageBgObject *temp_r29;
    Vec sp8;

    temp_r31 = backgroundInfo.work;
    if (polyDisp.flags & 1)
        var_r30 = 0x10;
    else if (modeCtrl.gameType == GAMETYPE_MAIN_COMPETITION)
        var_r30 = 1 << (modeCtrl.unk30 - 1);
    else
        var_r30 = 1;
    var_r28 = temp_r31->unkC;
    for (var_ctr = temp_r31->unk8; var_ctr > 0; var_ctr--, var_r28++)
        var_r28->unk0->flags &= 0xFFFEFFFF;
    if (temp_r31->unk8 != 0)
    {
        avdisp_enable_custom_tex_mtx(1U);
        var_r28 = temp_r31->unkC;
        for (var_r27 = temp_r31->unk8; var_r27 > 0; var_r27--, var_r28++)
        {
            temp_r29 = var_r28->unk0;
            if (temp_r29->flags & var_r30)
            {
                avdisp_set_custom_tex_mtx(0, var_r28->unk28);
                mathutil_mtxA_from_mtx(userWork->matrices[0]);
                mathutil_mtxA_translate(&temp_r29->pos);
                mathutil_mtxA_rotate_z(temp_r29->rotZ);
                mathutil_mtxA_rotate_y(temp_r29->rotY);
                mathutil_mtxA_rotate_x(temp_r29->rotX);
                mathutil_mtxA_scale(&temp_r29->scale);
                GXLoadPosMtxImm(mathutilData->mtxA, 0U);
                GXLoadNrmMtxImm(mathutilData->mtxA, 0U);
                avdisp_draw_model_culled_sort_translucent(temp_r29->model);
            }
        }
        avdisp_enable_custom_tex_mtx(0U);
    }
    bg_default_draw();
    if (!(polyDisp.flags & 4) && temp_r31->unk170 != 0)
    {
        avdisp_set_z_mode(1U, GX_LEQUAL, 0U);
        avdisp_enable_custom_tex_mtx(1U);
        var_r27_2 = temp_r31->unk174;
        for (var_r28_2 = temp_r31->unk170; var_r28_2 > 0; var_r28_2--, var_r27_2++)
        {
            temp_r29 = var_r27_2->unk0;
            if (temp_r29->flags & var_r30)
            {
                mathutil_mtxA_from_mtx(userWork->matrices[0]);
                mathutil_mtxA_translate(&temp_r29->pos);
                mathutil_mtxA_rotate_z(temp_r29->rotZ);
                mathutil_mtxA_rotate_y(temp_r29->rotY);
                mathutil_mtxA_rotate_x(temp_r29->rotX);
                mathutil_mtxA_scale(&temp_r29->scale);
                mathutil_mtxA_rotate_y(var_r27_2->unk10 - (var_r27_2->unk12 << 6));
                mathutil_mtxA_push();
                mathutil_mtxA_translate_xyz(150.0f, 0.0f, 0.0f);
                mathutil_mtxA_rigid_inv_tf_tl(&sp8);
                mathutil_mtxA_rotate_y(mathutil_atan2(sp8.x, sp8.z));
                GXLoadPosMtxImm(mathutilData->mtxA, 0U);
                avdisp_set_custom_tex_mtx(0, var_r27_2->unk38);
                avdisp_draw_model_culled_sort_translucent(temp_r31->unkB08);
                mathutil_mtxA_pop();
                mathutil_mtxA_translate_xyz(-150.0f, 0.0f, 0.0f);
                mathutil_mtxA_rigid_inv_tf_tl(&sp8);
                mathutil_mtxA_rotate_y(mathutil_atan2(sp8.x, sp8.z));
                GXLoadPosMtxImm(mathutilData->mtxA, 0U);
                avdisp_set_custom_tex_mtx(0, var_r27_2->unk68);
                avdisp_draw_model_culled_sort_translucent(temp_r31->unkB08);
            }
        }
        avdisp_set_z_mode(1U, GX_LEQUAL, 1U);
        avdisp_enable_custom_tex_mtx(0U);
    }
}

void bg_master_interact(int arg0) {}

static void lbl_80063AD8(struct GCMMatState_Unit *arg0)
{
    struct BGMasterWork *work = backgroundInfo.work;
    struct Struct80061BC4_sub sp2C = arg0->unkC;
    float texMtx[2][3];

    GXLoadTexObj_cached(work->waterTex, sp2C.u_texMapId);
    GXLoadTexObj_cached(work->stgCloudTex, sp2C.u_texMapId + 1);
    texMtx[0][0] = 0.0f;
    texMtx[0][1] = 0.8f * mathutil_sin(backgroundInfo.u_otherSeed << 8);
    texMtx[0][2] = 0.0f;
    texMtx[1][0] = 0.0f;
    texMtx[1][1] = 0.0f;
    texMtx[1][2] = 0.8f * mathutil_cos(backgroundInfo.u_otherSeed << 8);
    GXSetIndTexMtx(sp2C.unk1C, texMtx, 0);
    mathutil_mtxA_push();
    mathutil_mtxA_from_mtx(work->unkB60);
    mathutil_mtxA_set_translate(&work->unkB48);
    mathutil_mtxA_scale_s(6.0f);
    GXLoadTexMtxImm(mathutilData->mtxA, sp2C.unk8, GX_MTX2x4);
    mathutil_mtxA_from_identity();
    mathutil_mtxA_translate(&work->unkB18);
    mathutil_mtxA_scale(&work->unkB3C);
    GXLoadTexMtxImm(mathutilData->mtxA, sp2C.unk8 + 3, GX_MTX2x4);
    mathutil_mtxA_pop();
    GXSetTexCoordGen(sp2C.unk4, GX_TG_MTX2x4, GX_TG_NRM, sp2C.unk8);
    GXSetTexCoordGen(sp2C.unk4 + 1, GX_TG_MTX2x4, GX_TG_TEX0, sp2C.unk8 + 3);
    GXSetIndTexOrder(sp2C.unk10, sp2C.unk4 + 1, sp2C.u_texMapId + 1);
    GXSetTevIndirect(sp2C.unk0, sp2C.unk10, GX_ITF_8, GX_ITB_STU, sp2C.unk1C, GX_ITW_OFF, GX_ITW_OFF, 0U, 0, 0);
    GXSetTevSwapMode_cached(sp2C.unk0, 0, 0);
    GXSetTevOrder_cached(sp2C.unk0, sp2C.unk4, sp2C.u_texMapId, 0xFF);
    GXSetTevColorIn_cached(sp2C.unk0, 8, 0xF, 0xF, 0);
    GXSetTevColorOp_cached(sp2C.unk0, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn_cached(sp2C.unk0, 7, 7, 7, 0);
    GXSetTevAlphaOp_cached(sp2C.unk0, 0, 0, 0, 1, 0);
    sp2C.unk0++;
    sp2C.unk10++;
    sp2C.unk4 += 2;
    sp2C.unk8 += 6;
    sp2C.unk1C++;
    sp2C.u_texMapId += 2;
    arg0->unkC = sp2C;
}

static int model_find_proc(int index, struct GMAModelEntry *entry)
{
    struct BGMasterWork *work = backgroundInfo.work;

    switch (index)
    {
    case 0:  // MST_STG_CLOUD_
        work->stgCloudTex = &entry->model->texObjs[0];
        break;
    case 1:  // MST_MINI_CLOUD_
        if (work->miniCloudModelsCount < 4)
        {
            work->miniCloudModels[work->miniCloudModelsCount] = entry->model;
            work->miniCloudModelsCount++;
        }
        break;
    case 2:  // MST_SPRA_CLOUD_
        work->unkB08 = entry->model;
        break;
    case 3:  // MST_YOUSAI_WATER_C
        work->unkB0C = entry->model;
        break;
    case 4:  // MST_YOUSAI_WATER_SPEC
        work->waterTex = &entry->model->texObjs[0];
        break;
    }
    return 1;
}

static int obj_find_proc(int index, struct StageBgObject *bgObj)
{
    struct BGMasterWork *work = backgroundInfo.work;
    struct Effect effect;

    switch (index)
    {
    case 0:  // MST_BOTTOMCLOUD_
    case 1:  // MST_PARADISE_CLOUD_
        if (bgObj->model != NULL && work->unk8 < 8)
        {
            work->unkC[work->unk8].unk0 = bgObj;
            work->unk8++;
        }
        break;
    case 2:  // MST_CLOUD_GEN_
        if (bgObj->model != NULL && work->unk170 < 8)
        {
            work->unk174[work->unk170].unk0 = bgObj;
            work->unk170++;
        }
        break;
    case 3:  // MST_PARADISE_A
    case 4:  // MST_PARADISE_B
        memset(&effect, 0, sizeof(effect));
        effect.type = ET_BGMST_WATER;
        effect.model = (struct GMAModel *)bgObj;  // huh?
        spawn_effect(&effect);
        break;
    }
    return 1;
}
