#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "avdisp.h"
#include "camera.h"
#include "gxcache.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "shadow.h"

struct SomeShadowStruct
{
    struct Struct80092B98 unk0;
    float unk40;
    Vec unk44;
    float unk50;
    float unk54;
    float unk58;
    Vec unk5C;
    Vec unk68;
    Vec unk74;
    Mtx unk80;
    Mtx unkB0;
};

static void func_800934B0(void);
static void func_80093920(void);
static int lbl_800939F0(struct NlMesh *, int);
static u32 lbl_80093A0C(struct UnkStruct27 *);

static s32 lbl_802F2150;
static struct SomeShadowStruct *lbl_802F2154;
static struct SomeShadowStruct lbl_802B57A0[64];
static GXTexObj shadowTexObj;
static s8 lbl_802B57A0_0x3820[0x100];
static Mtx lbl_802B57A0_0x3920;

void func_80092498(void)
{
    func_800924CC();
    lbl_802F2150 = 0;
    lbl_802F2154 = lbl_802B57A0;
}

void func_800924CC(void)
{
    GXTexObj *texObj;
    void *pixels;
    s8 *ptr;
    int j;
    int i;
    u8 unused[8];

    texObj = &shadowTexObj;
    pixels = lbl_802B57A0_0x3820;
    ptr = pixels;
    memset(ptr, 0,    32);
    ptr += 32;
    memset(ptr, 0xFF, 32);
    ptr += 32;
    for (i = 4; i > 0; i--)
    {
        for (j = 8; j > 0; j--, ptr++)
            *ptr = 255.0f * (0.021276595f * (j + 0x27));
    }
    for (i = 4; i > 0; i--)
    {
        for (j = 8; j > 0; j--, ptr++)
            *ptr = 255.0f * (0.021276595f * (j + 0x1F));
    }
    for (i = 4; i > 0; i--)
    {
        for (j = 8; j > 0; j--, ptr++)
            *ptr = 255.0f * (0.021276595f * (j + 0x17));
    }
    for (i = 4; i > 0; i--)
    {
        for (j = 8; j > 0; j--, ptr++)
            *ptr = 255.0f * (0.021276595f * (j + 0xF));
    }
    for (i = 4; i > 0; i--)
    {
        for (j = 8; j > 0; j--, ptr++)
            *ptr = 255.0f * (0.021276595f * (j + 7));
    }
    for (i = 4; i > 0; i--)
    {
        for (j = 8; j > 0; j--, ptr++)
            *ptr = 255.0f * (0.021276595f * (j - 1));
    }
    DCFlushRange(pixels, 0x100);
    GXInitTexObj(texObj, pixels, 0x40U, 4U, GX_TF_I8, GX_CLAMP, GX_CLAMP, 0U);
    GXInitTexObjLOD(texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    lbl_802B57A0_0x3920[0][0] = 0.0f;
    lbl_802B57A0_0x3920[0][1] = 0.0f;
    lbl_802B57A0_0x3920[1][0] = 0.0f;
    lbl_802B57A0_0x3920[1][1] = 0.0f;
    lbl_802B57A0_0x3920[1][2] = 0.0f;
    lbl_802B57A0_0x3920[1][3] = 0.0f;
    lbl_802B57A0_0x3920[2][0] = 0.0f;
    lbl_802B57A0_0x3920[2][1] = 0.0f;
    lbl_802B57A0_0x3920[2][2] = 0.0f;
    lbl_802B57A0_0x3920[2][3] = 1.0f;
}

void shadowerase_main(void)
{
    lbl_802F2150 = 0;
    lbl_802F2154 = lbl_802B57A0;
}

int u_init_shadow_stuff_probably(struct Struct80092B98 *arg0)
{
    Vec spC;
    f32 temp_f2_2;
    f32 var_f30;

    if (lbl_802F2150 > 0x40)
        return 0;

    temp_f2_2 = (arg0->unk0.x - arg0->unk10.x) * (arg0->unk0.x - arg0->unk10.x)
              + (arg0->unk0.y - arg0->unk10.y) * (arg0->unk0.y - arg0->unk10.y)
              + (arg0->unk0.z - arg0->unk10.z) * (arg0->unk0.z - arg0->unk10.z);
    if (temp_f2_2 - (arg0->unk1C * arg0->unk1C) > 0.0f)
        var_f30 = mathutil_sqrt((arg0->unk1C * arg0->unk1C) / (temp_f2_2 - (arg0->unk1C * arg0->unk1C)));
    else
        var_f30 = 5.671282f;

    memcpy(&lbl_802F2154->unk0, arg0, 0x40U);
    lbl_802F2154->unk50 = var_f30;
    lbl_802F2154->unk54 = 1.0f / var_f30;
    lbl_802F2154->unk58 = mathutil_sqrt(temp_f2_2);
    spC.x = arg0->unk10.x - arg0->unk0.x;
    spC.y = arg0->unk10.y - arg0->unk0.y;
    spC.z = arg0->unk10.z - arg0->unk0.z;
    mathutil_vec_set_len(&spC, &spC, 0.5f * arg0->unkC);
    spC.x += arg0->unk10.x;
    spC.y += arg0->unk10.y;
    spC.z += arg0->unk10.z;
    lbl_802F2154->unk44 = spC;
    lbl_802F2154++;
    lbl_802F2150++;
    return 1;
}

int func_80092D34(void)
{
    return lbl_802F2150;
}

void u_draw_ball_shadow(void)
{
    Mtx sp38;
    Mtx sp8;
    float temp_f0_2;
    float temp_f1;
    float temp_f29;
    float temp_f2_2;
    float temp_f3;
    int var_r30;
    struct SomeShadowStruct *var_r29;
    u16 temp_r28;

    mathutil_mtxA_push();
    temp_r28 = 1 << currentCamera->unk204;
    mathutil_mtxA_from_identity();
    mathutilData->mtxA[0][2] = 0.5f;
    mathutilData->mtxA[1][2] = 0.5f;
    mathutil_mtxA_to_mtx(sp38);
    var_r29 = lbl_802B57A0;
    temp_f29 = currentCamera->sub28.unk3C;

    for (var_r30 = lbl_802F2150; var_r30 > 0; var_r30--, var_r29++)
    {
        var_r29->unk40 = 0.0f;
        if (var_r29->unk0.unk2E & temp_r28)
        {
            mathutil_mtxA_from_mtxB();
            if (clip_z(&var_r29->unk44, 0.5f * var_r29->unk0.unkC))
            {
                mathutil_mtxA_tf_point(&var_r29->unk0.unk10, &var_r29->unk68);
                temp_f0_2 = (var_r29->unk0.unk1C * temp_f29) / mathutil_vec_len(&var_r29->unk68);
                temp_f2_2 = var_r29->unk0.unk30;
                if (temp_f0_2 < temp_f2_2)
                    continue;
                temp_f3 = var_r29->unk0.unk34;
                if (temp_f0_2 > temp_f3)
                    var_r29->unk40 = 1.0f;
                else
                    var_r29->unk40 = (temp_f0_2 - temp_f2_2) / (temp_f3 - temp_f2_2);
                mathutil_mtxA_tf_point(&var_r29->unk0.unk0, &var_r29->unk5C);
                mathutil_mtxA_tf_vec(&var_r29->unk0.unk20, &var_r29->unk74);
                MTXLookAt(sp8, &var_r29->unk5C, &var_r29->unk74, &var_r29->unk68);
                mathutil_mtxA_from_mtx(sp8);
                lbl_802B57A0_0x3920[0][2] = -0.875f / var_r29->unk0.unkC;
                lbl_802B57A0_0x3920[0][3] = 0.125f + ((-0.875f * var_r29->unk58) / var_r29->unk0.unkC);
                mathutil_mtxA_mult_left(lbl_802B57A0_0x3920);
                mathutil_mtxA_to_mtx(var_r29->unkB0);
                mathutil_mtxA_from_mtx(sp38);
                switch (var_r29->unk0.unk3C->unk0)
                {
                case 0:
                    temp_f1 = 0.5f * var_r29->unk54;
                    mathutil_mtxA_scale_xyz(temp_f1, temp_f1, 1.0f);
                    break;
                default:
                    temp_f1 = 0.5f / var_r29->unk0.unk1C;
                    mathutil_mtxA_scale_xyz(temp_f1, temp_f1, 0.0f);
                    mathutilData->mtxA[0][3] = 0.5f;
                    mathutilData->mtxA[1][3] = 0.5f;
                    mathutilData->mtxA[2][3] = 1.0f;
                    break;
                }
                mathutil_mtxA_mult_right(sp8);
                mathutil_mtxA_to_mtx(var_r29->unk80);
            }
        }
    }
    mathutil_mtxA_pop();
}

void func_80092F90(struct Struct80092F90 *arg0)
{
    struct Struct80092F90 sp60;
    Vec sp54;
    Vec sp48;
    Vec sp3C;
    Vec sp30;
    struct
    {
        s32 sp14;
        u32 sp18;
        s32 sp1C;
        u32 sp20;
        s32 sp24;
        s32 sp28;
        u8 sp2C;
        u8 sp2D;
    } stackVars;
    GXColor color;
    float temp_f0;
    float temp_f1;
    float var_f29;
    float temp_f28;
    float temp_f5;
    s32 var_r28;
    struct SomeShadowStruct *var_r27;
    Func802F20F0 var_r26;
    u8 r24;

    if (lbl_802F2150 != 0)
    {
        sp60 = *arg0;
        if (sp60.unk2 >= 3U)
            var_r26 = u_avdisp_set_some_func_2(lbl_80093A0C);
        switch (sp60.unk2)
        {
        case 0:
        case 1:
        case 2:
            sp54 = sp60.model.gma->boundSphereCenter;
            var_f29 = sp60.model.gma->boundSphereRadius;
            break;
        default:
            sp54 = sp60.model.gma->boundSphereCenter;
            var_f29 = sp60.model.gma->boundSphereRadius;
            break;
        }
        func_800934B0();
        stackVars.sp14 = 0;
        stackVars.sp18 = 0;
        stackVars.sp1C = 0;
        stackVars.sp20 = 0x1E;
        stackVars.sp24 = 0;
        stackVars.sp28 = 0;
        stackVars.sp2C = 0xFF;
        stackVars.sp2D = 0xFF;
        var_r27 = lbl_802B57A0;
        for (var_r28 = lbl_802F2150; var_r28 > 0; var_r28--, var_r27++)
        {
            if (var_r27->unk40 != 0.0 && (var_r27->unk0.unk2C & sp60.unk0))
            {
                temp_f28 = var_r27->unk0.unkC;
                mathutil_mtxA_rigid_inv_tf_point(&var_r27->unk68, &sp48);
                mathutil_mtxA_rigid_inv_tf_point(&var_r27->unk5C, &sp30);
                sp3C.x = sp48.x - sp30.x;
                sp3C.y = sp48.y - sp30.y;
                sp3C.z = sp48.z - sp30.z;
                mathutil_vec_normalize_len(&sp3C);
                sp30.x = sp54.x - sp48.x;
                sp30.y = sp54.y - sp48.y;
                sp30.z = sp54.z - sp48.z;
                temp_f5 = mathutil_vec_dot_prod(&sp3C, &sp30);
                if (temp_f5 < -var_f29 || temp_f5 > (temp_f28 + var_f29))
                    continue;
                temp_f0 = var_r27->unk0.unk1C + (var_r27->unk50 * temp_f28);
                if (mathutil_vec_sq_len(&sp30) - (temp_f5 * temp_f5) > (temp_f0 + var_f29) * (temp_f0 + var_f29))
                    continue;
                temp_f1 = var_r27->unk40;
                color.r = (s8) ((f32) var_r27->unk0.unk38.r * temp_f1);
                color.g = (s8) ((f32) var_r27->unk0.unk38.g * temp_f1);
                color.b = (s8) ((f32) var_r27->unk0.unk38.b * temp_f1);
                GXSetTevKColor_cached(stackVars.sp24, color);
                stackVars.sp24 += 1;
                mathutil_mtxA_push();
                mathutil_mtxA_mult_left(var_r27->unk80);
                GXLoadTexMtxImm(mathutilData->mtxA, stackVars.sp20, GX_MTX3x4);
                stackVars.sp20 += 3;
                mathutil_mtxA_pop();
                mathutil_mtxA_push();
                mathutil_mtxA_mult_left(var_r27->unkB0);
                GXLoadTexMtxImm(mathutilData->mtxA, stackVars.sp20, GX_MTX3x4);
                stackVars.sp20 += 3;
                mathutil_mtxA_pop();
                GXLoadTexObj_cached(&var_r27->unk0.unk3C->unk4, stackVars.sp28);
                stackVars.sp28 += 1;
                GXLoadTexObj_cached(&shadowTexObj, stackVars.sp28);
                stackVars.sp28 += 1;
                if (stackVars.sp14 == 0)
                    stackVars.sp18 += 2;
                else
                    stackVars.sp18 += 3;
                stackVars.sp1C += 2;
                stackVars.sp14++;
                if (stackVars.sp14 >= 4)
                {
                    r24 = stackVars.sp18;
                    if (stackVars.sp2C != r24)
                    {
                        GXSetNumTevStages_cached(r24);
                        stackVars.sp2C = r24;
                    }
                    r24 = stackVars.sp1C;
                    if (stackVars.sp2D != r24)
                    {
                        GXSetNumTexGens(r24);
                        stackVars.sp2D = r24;
                    }
                    switch (sp60.unk2)
                    {
                    case 0:
                    case 1:
                    case 2:
                        u_nl2ngc_draw_model_with_mesh_func(sp60.model.naomi, lbl_800939F0);
                        break;
                    case 3:
                    case 4:
                    case 5:
                        u_avdisp_draw_model_4(sp60.model.gma);
                        break;
                    case 6:
                        avdisp_draw_model_unculled_sort_none(sp60.model.gma);
                        break;
                    }
                    stackVars.sp14 = 0;
                    stackVars.sp18 = 0;
                    stackVars.sp1C = 0;
                    stackVars.sp20 = 0x1E;
                    stackVars.sp24 = 0;
                    stackVars.sp28 = 0;
                }
            }
        }
        if (stackVars.sp14 != 0)
        {
            u8 temp;

            temp = stackVars.sp18;
            if (stackVars.sp2C != temp)
                GXSetNumTevStages_cached(stackVars.sp18);
            temp = stackVars.sp1C;
            if (stackVars.sp2D != temp)
                GXSetNumTexGens(stackVars.sp1C);
            switch (sp60.unk2)
            {
            case 0:
            case 1:
            case 2:
                u_nl2ngc_draw_model_with_mesh_func(sp60.model.naomi, lbl_800939F0);
                break;
            case 3:
            case 4:
            case 5:
                u_avdisp_draw_model_4(sp60.model.gma);
                break;
            case 6:
                avdisp_draw_model_unculled_sort_none(sp60.model.gma);
                break;
            }
        }
        if (sp60.unk2 >= 3)
            u_avdisp_set_some_func_2(var_r26);
        func_80093920();
    }
}

static void func_800934B0(void)
{
    s32 var_r31;
    s32 var_r30;
    s32 var_r29;
    s32 var_r28;
    s32 var_r27;
    s32 var_r26;
    s32 var_r3;

    GXSetNumChans(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_POS, 0x1EU, GX_FALSE, 0x7DU);
    GXSetTevOrder_cached(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode_cached(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevKColorSel_cached(GX_TEVSTAGE0, GX_TEV_KCSEL_K0);
    GXSetTevColorIn_cached(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevColorOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevAlphaOp_cached(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    var_r27 = 0xD;
    GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX3x4, GX_TG_POS, 0x21U, GX_FALSE, 0x7DU);
    GXSetTevOrder_cached(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevDirect(GX_TEVSTAGE1);
    GXSetTevSwapMode_cached(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevColorIn_cached(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_CPREV, GX_CC_ONE);
    GXSetTevColorOp_cached(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaIn_cached(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevAlphaOp_cached(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    var_r3 = lbl_802F2150;
    var_r30 = GX_TEVSTAGE2;
    var_r29 = GX_TEXCOORD2;
    var_r28 = 0x24;
    var_r26 = 2;
    if (var_r3 > 4)
        var_r3 = 4;
    var_r31 = var_r3 - 1;
    for (var_r31 = var_r3 - 1; var_r31 > 0; var_r31--)
    {
        GXSetTexCoordGen2(var_r29, GX_TG_MTX3x4, GX_TG_POS, var_r28, GX_FALSE, 0x7DU);
        GXSetTevOrder_cached(var_r30, var_r29, var_r26, GX_COLOR0A0);
        GXSetTevDirect(var_r30);
        GXSetTevSwapMode_cached(var_r30, GX_TEV_SWAP0, GX_TEV_SWAP1);
        GXSetTevKColorSel_cached(var_r30, var_r27);
        GXSetTevColorIn_cached(var_r30, GX_CC_ZERO, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
        GXSetTevColorOp_cached(var_r30, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
        GXSetTevAlphaIn_cached(var_r30, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
        GXSetTevAlphaOp_cached(var_r30, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
        var_r28 += 3;
        var_r27 += 1;
        var_r28 == 0;  // needed to match
        GXSetTexCoordGen(var_r29 + 1, GX_TG_MTX3x4, GX_TG_POS, var_r28);
        GXSetTevOrder_cached(var_r30 + 1, var_r29 + 1, var_r26 + 1, GX_COLOR_NULL);
        GXSetTevDirect(var_r30 + 1);
        GXSetTevSwapMode_cached(var_r30 + 1, GX_TEV_SWAP0, GX_TEV_SWAP1);
        GXSetTevColorIn_cached(var_r30 + 1, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ONE);
        GXSetTevColorOp_cached(var_r30 + 1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
        GXSetTevAlphaIn_cached(var_r30 + 1, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
        GXSetTevAlphaOp_cached(var_r30 + 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
        var_r29 += 2;
        var_r28 += 3;
        var_r26 += 2;
        GXSetTevOrder_cached(var_r30 + 2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
        GXSetTevDirect(var_r30 + 2);
        GXSetTevColorIn_cached(var_r30 + 2, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
        GXSetTevColorOp_cached(var_r30 + 2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        GXSetTevAlphaIn_cached(var_r30 + 2, GX_CA_A0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
        GXSetTevAlphaOp_cached(var_r30 + 2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
        var_r30 += 3;
    }
    GXSetNumIndStages(0U);
    GXSetBlendMode_cached(1, 0, 2, 0);
    GXSetZMode_cached(GX_ENABLE, GX_EQUAL, GX_ENABLE);
    {
        GXColor color = {0};
        GXSetFog_cached(0, 0.0f, 100.0f, 0.1f, 20000.0f, color);
    }
}

static void func_80093920(void)
{
    s32 var_r0;
    s32 var_r31;
    s32 var_r30;

    var_r30 = GX_TEVSTAGE0;
    var_r0 = lbl_802F2150;
    if (var_r0 > 4)
        var_r0 = 4;
    for (var_r31 = var_r0; var_r31 > 0; var_r31--)
    {
        GXSetTevSwapMode_cached(var_r30, GX_TEV_SWAP0, GX_TEV_SWAP0);
        GXSetTevSwapMode_cached(var_r30 + 1, GX_TEV_SWAP0, GX_TEV_SWAP0);
        var_r30 += 2;
    }
    GXSetZMode_cached(GX_ENABLE, GX_LEQUAL, GX_ENABLE);
}

static int lbl_800939F0(struct NlMesh *mesh, int size)
{
    return !(mesh->flags & (1 << 7)) ? 0 : 1;
}

static u32 lbl_80093A0C(struct UnkStruct27 *arg0)
{
    return 1;
}
