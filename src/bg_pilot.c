#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "avdisp.h"
#include "background.h"
#include "camera.h"
#include "gxcache.h"
#include "light.h"
#include "mathutil.h"
#include "mode.h"
#include "polydisp.h"
#include "rend_efc.h"
#include "stage.h"
#include "window.h"

struct BGModelSearch table1[] =
{
	{1, "PIL_WATER_SPEC"},
	{1, "PIL_IND_MAP_A"},
	{1, "PIL_SEA_MASK"},
	{1, "PIL_VTX_SEA_BALL_A"},
	{1, "PIL_IND_MAP_B"},
	{1, "PIL_IND_MAP_C"},
	{0, "PIL_HAMON_"},
	{3, NULL},
};

struct BGModelSearch table2[] =
{
	{0, "PIL_SEA_"},
	{1, "PIL_SANGOSYO_A"},
	{1, "PIL_ISLAND_A"},
	{1, "PIL_ISLAND_B"},
	{1, "PIL_ISLAND_C"},
	{3, NULL},
};

void func_80064C30(struct GCMMatState_Unit *);
int func_80065084(int, struct GMAModelEntry *);
int func_80065128(int, struct StageBgObject *);

extern struct RenderEffectFuncs lbl_801D3D78;

void bg_pilot_init(void)
{
    struct RenderEffect sp14;
    Vec sp8;
    struct BGPilotWork_sub *var_r30;
    int i;
    struct BGPilotWork *temp_r29;

    temp_r29 = backgroundInfo.work;
    bg_default_init();
    if (temp_r29->unk0 == 0)
    {
        temp_r29->unk2C = 0;
        find_background_gma_models(table1, func_80065084);
    }
    temp_r29->unk14 = 0;
    find_background_objects(decodedStageLzPtr->bgObjects, decodedStageLzPtr->bgObjectCount, table2, func_80065128);
    find_background_objects(decodedStageLzPtr->fgObjects, decodedStageLzPtr->fgObjectCount, table2, func_80065128);
    memset(&sp14, 0, sizeof(sp14));
    sp14.cameraMask = 0xFFFF;
    sp14.funcs = &lbl_801D3D78;
    rend_efc_enable(0, 0, &sp14);
    var_r30 = temp_r29->unkE4;
    var_r30[0].unk0.x = RAND_FLOAT();
    var_r30[0].unk0.y = RAND_FLOAT();
    var_r30[0].unk0.z = RAND_FLOAT();
    sp8.x = 0.0013888889f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.y = 0.0023809525f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.z = 0.0f;
    mathutil_mtxA_from_rotate_z(rand() & 0x7FFF);
    mathutil_mtxA_tf_vec(&sp8, &var_r30[0].unkC);
    var_r30[0].unk18.x = 0.0033333334f * (0.95f + (0.1f * RAND_FLOAT()));
    var_r30[0].unk18.y = 0.0033333334f * (0.95f + (0.1f * RAND_FLOAT()));
    var_r30[0].unk18.z = 1.0f;
    var_r30[0].unk24 = rand() & 0x7FFF;
    var_r30[1].unk0.x = RAND_FLOAT();
    var_r30[1].unk0.y = RAND_FLOAT();
    var_r30[1].unk0.z = RAND_FLOAT();
    sp8.x = 0.0042735045f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.y = 0.0055555557f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.z = 0.0f;
    mathutil_mtxA_from_rotate_z(rand() & 0x7FFF);
    mathutil_mtxA_tf_vec(&sp8, &var_r30[1].unkC);
    var_r30[1].unk18.x = 0.0125f * (0.95f + (0.1f * RAND_FLOAT()));
    var_r30[1].unk18.y = 0.0125f * (0.95f + (0.1f * RAND_FLOAT()));
    var_r30[1].unk18.z = 1.0f;
    var_r30[1].unk24 = (int)(4096.0f * RAND_FLOAT()) + 0x1000 + var_r30[0].unk24;
    var_r30[2].unk0.x = RAND_FLOAT();
    var_r30[2].unk0.y = RAND_FLOAT();
    var_r30[2].unk0.z = RAND_FLOAT();
    sp8.x = 0.0042735045f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.y = 0.0055555557f * (0.8f + (0.4f * RAND_FLOAT()));
    sp8.z = 0.0f;
    mathutil_mtxA_from_rotate_z(rand() & 0x7FFF);
    mathutil_mtxA_tf_vec(&sp8, &var_r30[2].unkC);
    var_r30[2].unk18.x = 0.05f * (0.95f + (0.1f * RAND_FLOAT()));
    var_r30[2].unk18.y = 0.05f * (0.95f + (0.1f * RAND_FLOAT()));
    var_r30[2].unk18.z = 1.0f;
    var_r30[2].unk24 = (int)(4096.0f * RAND_FLOAT()) + 0x1000 + var_r30[1].unk24;
    for (i = 3; i > 0; i--, var_r30++)
    {
        mathutil_mtxA_from_translate(&var_r30->unk0);
        mathutil_mtxA_scale(&var_r30->unk18);
        mathutil_mtxA_rotate_z(var_r30->unk24);
        mathutil_mtxA_rotate_x(0x4000);
        mathutil_mtxA_to_mtx(var_r30->unk28);
    }
}

void bg_pilot_main(void)
{
    int i;
    struct BGPilotWork *temp_r31;
    struct BGPilotWork_sub *var_r30;

    temp_r31 = backgroundInfo.work;
    bg_default_main();
    if (debugFlags & 0xA)
		return;
	var_r30 = temp_r31->unkE4;
	for (i = 3; i > 0; i--, var_r30++)
	{
		var_r30->unk0.x += var_r30->unkC.x;
		var_r30->unk0.y += var_r30->unkC.y;
		var_r30->unk0.z += var_r30->unkC.z;
		mathutil_mtxA_from_translate(&var_r30->unk0);
		mathutil_mtxA_scale(&var_r30->unk18);
		mathutil_mtxA_rotate_z(var_r30->unk24);
		mathutil_mtxA_rotate_x(0x4000);
		mathutil_mtxA_to_mtx(var_r30->unk28);
	}
}

void bg_pilot_finish(void) {}

#ifdef NONMATCHING
void bg_pilot_draw(void)
{
	Mtx/*44*/ spC0;
	Mtx sp90;
	Mtx44 sp50;
    Vec sp44;
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    float temp_f1;
    float temp_f1_2;
    float temp_f31;
    struct StageBgObject *temp_r31;
    struct BGPilotWork *temp_r30;
    struct Struct800993A8 *temp_r29;
    EnvMapFunc temp_r27;
    int unused;

    temp_r30 = backgroundInfo.work;
    if (polyDisp.flags & 1)
        unused = 0;
    else if (gameMode == 2 || gameMode == 4)
        unused = 0;
    else
        unused = 0;
    if (temp_r30->unk14 != NULL)
        temp_r30->unk14->flags &= 0xFFFEFFFF;
    bg_default_draw();
    mathutil_mtxA_from_mtxB();
    GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
    GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
    if (temp_r30->unk4 != NULL)
        avdisp_draw_model_culled_sort_none(temp_r30->unk4);
    if (temp_r30->unk8 != NULL)
        avdisp_draw_model_culled_sort_none(temp_r30->unk8);
    if (temp_r30->unkC != NULL)
        avdisp_draw_model_culled_sort_none(temp_r30->unkC);
    if (temp_r30->unk10 != NULL)
        avdisp_draw_model_culled_sort_none(temp_r30->unk10);
    temp_r31 = temp_r30->unk14;
    if (temp_r31 != NULL && !(polyDisp.flags & 4))
    {
        temp_r29 = temp_r30->unkE0;
        if (temp_r29 != NULL)
        {
            C_MTXPerspective(sp50, 0.005493164f * currentCamera->sub28.fov, currentCamera->sub28.aspect, 0.1f, 20000.0f);
            mathutil_mtxA_from_identity();
            mathutilData->mtxA[0][2] = 0.5 - (0.5 * (currentCamera->sub28.unk38 * (currentCamera->sub28.aspect * (sp50[0][0] * currentCamera->sub28.unk28))));
            mathutilData->mtxA[1][2] = 0.5 - (0.5 * (currentCamera->sub28.unk38 * (sp50[1][1] * currentCamera->sub28.unk2C)));
            temp_f1 = 0.5f / mathutil_tan((int)(0.5 * currentCamera->sub28.fov));
            mathutilData->mtxA[0][0] = temp_f1 / currentCamera->sub28.aspect;
            mathutilData->mtxA[1][1] = temp_f1;
            mathutil_mtxA_to_mtx(sp90);
            mathutil_mtxA_from_mtx(sp90);
            mathutil_mtxA_to_mtx(spC0);
            mathutil_mtxA_from_mtxB();
            mathutil_mtxA_mult_right(temp_r29->unk50);
            mathutil_mtxA_mult_left(spC0);
            mathutil_mtxA_to_mtx(temp_r30->unk50);
            mathutil_mtxA_from_mtx(spC0);
            mathutil_mtxA_scale_xyz(-1.0f, 1.0f, 1.0f);
            mathutil_mtxA_mult_right(mathutilData->mtxB);
            mathutil_mtxA_mult_right(temp_r29->unk50);
            mathutil_mtxA_to_mtx(temp_r30->unk80);
            mathutil_mtxA_from_mtx(temp_r29->unk50);
            mathutil_mtxA_tf_point_xyz(&sp14, 0.0f, 0.0f, 0.0f);
            mathutil_mtxA_tf_vec_xyz(&sp20, 0.0f, 1.0f, 0.0f);
            mathutil_mtxA_from_rotate_y(0x6800);
            mathutil_mtxA_rotate_x(g_bgLightInfo.infLightRotX);
            mathutil_mtxA_tf_vec_xyz(&sp44, 0.0f, 0.0f, 1.0f);
            temp_f1_2 = -2.0f * mathutil_vec_dot_prod(&sp20, &sp44);
            sp38.x = sp44.x + (temp_f1_2 * sp20.x);
             sp38.y = sp44.y + (temp_f1_2 * sp20.y);
            sp38.z = sp44.z + (temp_f1_2 * sp20.z);
            sp38.x = -sp38.x;
            sp38.y = -sp38.y;
            sp38.z = -sp38.z;
            mathutil_mtxA_from_mtxB();
            mathutil_mtxA_rigid_inv_tf_tl(&sp2C);
            sp8.x = sp2C.x - sp14.x;
            sp8.y = sp2C.y - sp14.y;
            sp8.z = sp2C.z - sp14.z;
            // reg swaps
            temp_f1_2 = temp_f31 = mathutil_vec_dot_prod(&sp8, &sp20) * mathutil_vec_dot_prod(&sp38, &sp20);
            mathutil_vec_set_len(&sp38, &sp8, temp_f1_2);
            sp2C.x += sp8.x;
            sp2C.y += sp8.y;
            sp2C.z += sp8.z;
            sp8.x = sp2C.x + sp44.x;
            sp8.y = sp2C.y + sp44.y;
            sp8.z = sp2C.z + sp44.z;
            C_MTXLookAt(spC0, &sp2C, &sp20, &sp8);
            mathutil_mtxA_from_translate_xyz(0.5f, 0.5f, 0.0f);
            mathutil_mtxA_scale_s(0.5f / (4.0f * temp_f31));
            mathutil_mtxA_mult_right(spC0);
            mathutil_mtxA_to_mtx(temp_r30->unkB0);
            mathutil_mtxA_from_mtxB();
            mathutil_mtxA_mult_right(temp_r29->unk50);
            GXLoadPosMtxImm(mathutilData->mtxA, GX_PNMTX0);
            GXLoadNrmMtxImm(mathutilData->mtxA, GX_PNMTX0);
            temp_r27 = u_avdisp_set_some_func_1(func_80064C30);
            avdisp_set_z_mode(GX_ENABLE, GX_LEQUAL, GX_DISABLE);
            avdisp_draw_model_culled_sort_none(temp_r31->model);
            avdisp_set_z_mode(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
            u_avdisp_set_some_func_1(temp_r27);
        }
        else
        {
			mathutil_mtxA_from_mtxB_translate(&temp_r31->pos);
			mathutil_mtxA_rotate_z(temp_r31->rotZ);
			mathutil_mtxA_rotate_y(temp_r31->rotY);
			mathutil_mtxA_rotate_x(temp_r31->rotX);
			mathutil_mtxA_scale(&temp_r31->scale);
			GXLoadPosMtxImm(mathutilData->mtxA, 0U);
			GXLoadNrmMtxImm(mathutilData->mtxA, 0U);
			avdisp_draw_model_culled_sort_translucent(temp_r31->model);
		}
    }
}
#else
void bg_pilot_c_force_float_order(void)
{
	volatile float f;
	volatile double d;
	f = 0.0054931640625f;
	f = 20000.0f;
	d = 0.5;
	f = 0.5f;
}
const float neg1 = -1.0f;
const float neg2 = -2.0f;
asm void bg_pilot_draw(void)
{
	nofralloc
#include "../asm/nonmatchings/bg_pilot_draw.s"
}
#pragma peephole on
#endif

void bg_pilot_interact(int arg0) {}

#ifdef NONMATCHING
void func_80064C30(struct GCMMatState_Unit *arg0)
{
    struct TevStageInfo sp44;
    float sp2C[2][3];
    float sp14[2][3];
    struct BGPilotWork *temp_r31;
    struct Struct800993A8 *temp_r25;

    temp_r31 = backgroundInfo.work;
    temp_r25 = temp_r31->unkE0;
    sp44 = arg0->unkC;
    GXLoadTexObj_cached(&temp_r25->unk0, sp44.texMapId);
    GXLoadTexObj_cached(&temp_r25->unk2C, sp44.texMapId + 1);
    GXLoadTexObj_cached(temp_r31->unk1C, sp44.texMapId + 2);
    GXLoadTexMtxImm(temp_r31->unk50, sp44.unk8, GX_MTX3x4);
    GXLoadTexMtxImm(temp_r31->unk80, sp44.unk8 + 3, GX_MTX3x4);
    GXSetTevKColorSel_cached(sp44.tevStage, GX_TEV_KCSEL_3_4);
    GXSetTexCoordGen(sp44.texCoordId, GX_TG_MTX3x4, GX_TG_POS, sp44.unk8);
    GXSetTevSwapMode_cached(sp44.tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevOrder_cached(sp44.tevStage, sp44.texCoordId, sp44.texMapId, GX_COLOR_NULL);
    GXSetTevColorIn_cached(sp44.tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
    GXSetTevColorOp_cached(sp44.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(sp44.tevStage, GX_CA_ZERO, GX_CA_TEXA, GX_CA_APREV, GX_CA_APREV);
    GXSetTevAlphaOp_cached(sp44.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTexCoordGen(sp44.texCoordId + 1, GX_TG_MTX3x4, GX_TG_POS, sp44.unk8 + 3);
    GXSetIndTexOrder(sp44.tevIndStage, sp44.texCoordId + 1, sp44.texMapId + 1);
    sp2C[0][0] = 0.0f;
    sp2C[0][1] = 0.2f;
    sp2C[0][2] = 0.0f;
    sp2C[1][0] = 0.2f;
    sp2C[1][1] = 0.0f;
    sp2C[1][2] = 0.0f;
    GXSetIndTexMtx(sp44.unk1C, sp2C, 0);
    GXSetTevIndirect(sp44.tevStage, sp44.tevIndStage, GX_ITF_8, GX_ITB_STU, sp44.unk1C, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    sp44.unk1C += 1;
    sp44.tevStage += 1;
    sp44.texCoordId += 2;
    sp44.unk8 += 6;
    sp44.texMapId += 2;
    GXSetTevKColorSel_cached(sp44.tevStage, GX_TEV_KCSEL_7_8);
    GXSetTevKAlphaSel_cached(sp44.tevStage, GX_TEV_KASEL_1);
    GXLoadTexMtxImm(temp_r31->unkB0, sp44.unk8, GX_MTX2x4);
    GXSetTexCoordGen(sp44.texCoordId, GX_TG_MTX2x4, GX_TG_POS, sp44.unk8);
    GXSetTevSwapMode_cached(sp44.tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevOrder_cached(sp44.tevStage, sp44.texCoordId, sp44.texMapId, GX_COLOR_NULL);
    GXSetTevColorIn_cached(sp44.tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_KONST, GX_CC_CPREV);
    GXSetTevColorOp_cached(sp44.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(sp44.tevStage, GX_CA_APREV, GX_CA_KONST, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevAlphaOp_cached(sp44.tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    sp14[0][0] = 0.0f;
    sp14[0][1] = -0.8f;
    sp14[0][2] = 0.0f;
    sp14[1][0] = 0.9f;
    sp14[1][1] = 0.0f;
    sp14[1][2] = 0.0f;
    GXSetIndTexMtx(sp44.unk1C, sp14, 0);
    GXSetTevIndirect(sp44.tevStage, sp44.tevIndStage, GX_ITF_8, GX_ITB_STU, sp44.unk1C, GX_ITW_OFF, GX_ITW_OFF, GX_FALSE, GX_FALSE, GX_ITBA_OFF);
    sp44.tevIndStage += 1;
    sp44.unk1C += 2;
    sp44.tevStage += 1;
    sp44.texCoordId += 1;
    sp44.unk8 += 3;
    sp44.texMapId += 1;
    arg0->unkC = sp44;
}
#else
const float lbl_802F45E0 = 0.2f;
const float lbl_802F45E4 = -0.8f;
const float lbl_802F45E8 = 0.9f;
asm void func_80064C30(struct GCMMatState_Unit *arg0)
{
	nofralloc
#include "../asm/nonmatchings/func_80064C30.s"
}
#pragma peephole on
#endif

int func_80065084(int arg0, struct GMAModelEntry *arg1)
{
	struct BGPilotWork *work = backgroundInfo.work;

    switch (arg0)             
    {
    case 0:
        work->unk1C = &arg1->model->texObjs[0];
        break;
    case 1:
        work->unk20 = &arg1->model->texObjs[0];
        break;
    case 2:
        work->unk24 = arg1->model;
        break;
    case 3:
        work->unk28 = arg1->model;
        break;
    case 4:
    case 5:
    case 6:
        if (work->unk2C < 8)
        {
            work->unk30[work->unk2C] = arg1->model;
            work->unk2C++;
        }
        break;
    }
    return 1;
}

int func_80065128(int arg0, struct StageBgObject *arg1)
{
	struct BGPilotWork *work = backgroundInfo.work;

    switch (arg0)          
    {
    case 0:
        work->unk14 = (void *)arg1;
        work->unk18 = arg1->model;
        break;
    case 1:
        work->unk4 = arg1->model;
        arg1->model = NULL;
        break;
    case 2:
        work->unk8 = arg1->model;
        arg1->model = NULL;
        break;
    case 3:
        work->unkC = arg1->model;
        arg1->model = NULL;
        break;
    case 4:
        work->unk10 = arg1->model;
        arg1->model = NULL;
        break;
    }
    return 1;
}
