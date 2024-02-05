#include <dolphin.h>

#include "global.h"
#include "ball.h"
#include "camera.h"
#include "gma.h"
#include "mathutil.h"
#include "mode.h"
#include "pool.h"

#include "../data/common.gma.h"

static Vec flareWork[4][13];
static float lensFlareOpacity[4];
static Vec lensFlareLightPos;
static float shineCoefficients[4];
static float lensFlareScale;

void lens_flare_init(void)
{
    lensFlareLightPos.x = 0.0f;
    lensFlareLightPos.y = 10000.0f;
    lensFlareLightPos.z = 0.0f;
    lensFlareScale = 1.0f;
}

static inline float inline_asm_stuff(register float a, register float b, register float c, register float d)
{
#ifdef C_ONLY
    return (a - b) * (a - b) + (c - d) * (c - d);
#else
    asm
    {
        fsubs a, a, b
        fsubs c, c, d
        fmuls a, a, a
        fmadds a, c, c, a
    }
    return a;
#endif
}

struct FlareSpot
{
    float unk0;
    float scale;
    s16 modelId;
};

const struct FlareSpot flareData[] =
{
    {  0.990f, 0.010993125f, 29 },
    {  0.663f, 0.004125f,    23 },
    {  0.473f, 0.002475f,    28 },
    {  0.370f, 0.003135f,    28 },
    {  0.304f, 0.001155f,    28 },
    {  0.145f, 0.001320f,    26 },
    { -0.141f, 0.000825f,    27 },
    { -0.288f, 0.001815f,    26 },
    { -0.353f, 0.002970f,    26 },
    { -0.424f, 0.001155f,    26 },
    { -0.489f, 0.001155f,    25 },
    { -0.625f, 0.003630f,    24 },
    { -0.729f, 0.006105f,    23 },
};

const Vec lbl_80171AA4 = { 0.0f, 0.0f, -1.0f };

void lens_flare_main(void)
{
    Vec sp38;
    Vec sp2C;
    Vec sp20;
    Vec sp14;
    Vec sp8;
    struct Camera *camera;
    int i;
    struct Ball *ball;
    s8 *status;
    int j;
    float temp_f1;
    float temp_f23;
    float temp_f2;
    float temp_f2_3;
    int var_ctr;

    camera = cameraInfo;
    for (i = 0; i < modeCtrl.playerCount; i++, camera++)
    {
        if ((cameraInfo[i].sub28.vp.width > 0.0f) && (cameraInfo[i].sub28.vp.height > 0.0f))
        {
            mathutil_mtxA_from_mtx(camera->unk1A4);
            mathutil_mtxA_tf_point(&lensFlareLightPos, &sp38);
            for (var_ctr = 0; var_ctr < 13; var_ctr++)
            {
                temp_f2 = (-0.11 / sp38.z) * flareData[var_ctr].unk0;
                flareWork[i][var_ctr].x = sp38.x * temp_f2;
                flareWork[i][var_ctr].y = sp38.y * temp_f2;
                flareWork[i][var_ctr].z = sp38.z * temp_f2;
                flareWork[i][var_ctr].z = -0.11 + (1e-05 * var_ctr);
            }
            sp20 = lbl_80171AA4;
            sp2C.x = lensFlareLightPos.x - camera->eye.x;
            sp2C.y = lensFlareLightPos.y - camera->eye.y;
            sp2C.z = lensFlareLightPos.z - camera->eye.z;
            mathutil_mtxA_from_mtx(camera->unk1A4);
            mathutil_mtxA_tf_point(&sp2C, &sp2C);
            temp_f1 = mathutil_vec_dot_normalized_safe(&sp2C, &sp20);
            if (temp_f1 <= 0.70710678)
            {
                shineCoefficients[i] = 0.0f;
                lensFlareOpacity[i] = 0.0f;
            }
            else
            {
                shineCoefficients[i] = 3.4142135485416842 * (temp_f1 - 0.70710678);
                lensFlareOpacity[i] = 0.99 * shineCoefficients[i];
            }
            if (lensFlareOpacity[i] > 0.0f)
            {
                ball = ballInfo;
                status = g_poolInfo.playerPool.statusList;
                for (j = g_poolInfo.playerPool.count; j > 0; j--, ball++, status++)
                {
                    if (*status != 0 && *status != 4 && !(ball->flags & 0x10))
                    {
                        mathutil_mtxA_tf_point(&ball->pos, &sp14);
                        if (sp14.z >= 0.0f)
                            continue;
                        temp_f2 = -1.0f / (sp14.z * camera->sub28.unk38);
                        sp14.x *= temp_f2;
                        sp14.y *= temp_f2;
                        temp_f23 = ball->currRadius * temp_f2;
                        sp8 = flareWork[i][0];
                        temp_f2_3 = -1.0f / (sp8.z * camera->sub28.unk38);
                        sp8.x *= temp_f2_3;
                        sp8.y *= temp_f2_3;
                        temp_f2 = mathutil_sqrt(inline_asm_stuff(sp14.x, sp8.x, sp14.y, sp8.y)) / temp_f23;
                        if (temp_f2 < 1.0f)
                        {
                            temp_f2 = 1.0f - mathutil_sqrt(1.0f - (temp_f2 * temp_f2));
                            shineCoefficients[i] *= temp_f2;
                            lensFlareOpacity[i] *= temp_f2;
                        }
                    }
                }
            }
        }
    }
}

void lens_flare_draw(int cameraId)
{
    float scale;
    u32 i;
    Vec *pos;
    const struct FlareSpot *r28;

    if (!(cameraInfo[cameraId].sub28.vp.width > 0.0f) || !(cameraInfo[cameraId].sub28.vp.height > 0.0f))
        return;
    if (lensFlareOpacity[cameraId] == 0.0)
        return;

    if (gameMode == MD_ADV)
    {
        switch (gameSubmode)
        {
        case SMD_ADV_RANKING_INIT:
        case SMD_ADV_RANKING_MAIN:
        case SMD_ADV_GAME_READY_INIT:
        case SMD_ADV_GAME_READY_MAIN:
        case SMD_ADV_GAME_PLAY_INIT:
        case SMD_ADV_GAME_PLAY_MAIN:
            break;
        default:
            return;
        }
    }

    pos = flareWork[cameraId];
    r28 = flareData;
    for (i = 0; i < ARRAY_COUNT(flareData); i++, pos++, r28++)
    {
        avdisp_set_alpha(lensFlareOpacity[cameraId]);
        mathutil_mtxA_from_translate(pos);
        scale = r28->scale;
        scale *= lensFlareScale;
        mathutil_mtxA_scale_xyz(scale, scale, 1.0f);
        GXLoadNrmMtxImm(mathutilData->mtxA, 0);
        GXLoadPosMtxImm(mathutilData->mtxA, 0);
        avdisp_draw_model_unculled_sort_none(commonGma->modelEntries[r28->modelId].model);
    }
}

void lens_flare_draw_mask(int cameraId)
{
    if (!(cameraInfo[cameraId].sub28.vp.width > 0.0f) || !(cameraInfo[cameraId].sub28.vp.height > 0.0f))
        return;
    if (lensFlareOpacity[cameraId] == 0.0)
        return;

    if (gameMode == MD_ADV)
    {
        switch (gameSubmode)
        {
        case SMD_ADV_RANKING_INIT:
        case SMD_ADV_RANKING_MAIN:
        case SMD_ADV_GAME_READY_INIT:
        case SMD_ADV_GAME_READY_MAIN:
        case SMD_ADV_GAME_PLAY_INIT:
        case SMD_ADV_GAME_PLAY_MAIN:
            break;
        default:
            return;
        }
    }

    mathutil_mtxA_from_translate_xyz(0.0f, 0.0f, -0.2f);
    avdisp_set_alpha(0.75f * lensFlareOpacity[cameraId]);
    avdisp_draw_model_culled_sort_all(commonGma->modelEntries[BLACK_SCREEN].model);
}

void lens_flare_set_light_position(Vec *pos)
{
    lensFlareLightPos = *pos;
}

void lens_flare_set_light_angle(int angleY, int angleX)
{
    mathutil_mtxA_from_rotate_y(angleY);
    mathutil_mtxA_rotate_x(angleX);
    mathutil_mtxA_get_col2_scaled(&lensFlareLightPos, -1000000.0);
}

void lens_flare_set_scale(float scale)
{
    lensFlareScale = scale;
}

void func_80094750(int arg0)
{
    lbl_802C5D60.unk0 = arg0;
    DCFlushRange(&lbl_802C5D60, sizeof(lbl_802C5D60));
    HIOWrite(0x1000, &lbl_802C5D60, sizeof(lbl_802C5D60));
    HIOWriteMailbox(arg0);
}

void func_800947B0(void)
{
    HIORead(0x1000, &lbl_802C5D60, sizeof(lbl_802C5D60));
    DCInvalidateRange(&lbl_802C5D60, sizeof(lbl_802C5D60));
}

int func_800947F8(void)
{
    u32 spC;
    u8 filler[4];

    OSGetTick();
    while (1)
    {
        HIOReadMailbox(&spC);
        if (spC == 2)
            return 0;
        if (spC == 1)
            return 1;
    }
}

void func_80094840(void)
{
    u32 sp8;

    do
    {
        HIOReadMailbox(&sp8);
    } while (sp8 != 0);
}

static s32 lbl_802F2180 ATTRIBUTE_ALIGN(8);
static u8 *lbl_802F2184;
static u32 lbl_802F2188;

void *func_80094870(void)
{
    int i;

    if (lbl_802F2180 == 0)
        return 0;
    for (i = 0; i < 16; i++)
    {
        u32 bit = 1 << i;
        if (!(lbl_802F2188 & bit))
        {
            lbl_802F2188 |= bit;
            return lbl_802F2184 + i * 0xC;
        }
    }
    return 0;
}

void func_800948D0(struct Struct80094870 *arg0)
{
    lbl_802F2188 &= ~(1 << arg0->unk8);
}

#pragma force_active on
int func_800948EC(void)
{
    return lbl_802F2180;
}
#pragma force_active reset
