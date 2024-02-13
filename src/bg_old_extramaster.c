#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "camera.h"
#include "effect.h"
#include "mathutil.h"
#include "nl2ngc.h"
#include "stage.h"

char starpointObjName[] = "obj_STARPOINT_";

void bg_old_extramaster_init(void)
{
    float scale;
    int i;
    int j;
    struct NlModel **var_r25;
    u32 nameLen;
    struct NlModel *model;
    struct Struct802099E8 *var_r27;
    struct Struct8020A348 *var_r27_2;
    struct Struct8020A348_child *var_r28;
    struct Effect effect;

    MTEfcChangeAttributeITI(g_bgNlObj->models[1], 0xFBFFFFFF, 0x04000000);
    backgroundInfo.unk80 = g_bgNlObj->models[39];
    backgroundInfo.unk84 = 1.0f;
    mathutil_mtxA_from_identity();
    mathutil_mtxA_scale_xyz(0.6666667f, 0.6666667f, 0.6666667f);
    mathutil_mtxA_to_mtx(backgroundInfo.unk48);
    backgroundInfo.unk14.x = RAND_FLOAT() - 0.5;
    backgroundInfo.unk14.y = RAND_FLOAT() - 0.5;
    backgroundInfo.unk14.z = RAND_FLOAT() - 0.5;
    mathutil_vec_set_len(&backgroundInfo.unk14, &backgroundInfo.unk14, 40.0 * RAND_FLOAT());
    mathutil_mtxA_from_translate(&backgroundInfo.unk14);
    mathutil_vec_set_len(&backgroundInfo.unk14, &backgroundInfo.unk20, -0.0005555555555555556 * RAND_FLOAT());
    backgroundInfo.unk2C.x = 256.0 * (RAND_FLOAT() - 0.5);
    backgroundInfo.unk2C.y = 65536.0f * RAND_FLOAT();
    backgroundInfo.unk2C.z = 256.0 * (RAND_FLOAT() - 0.5);
    backgroundInfo.unk38.x = 18.204444444444444 * (RAND_FLOAT() - 0.5);
    backgroundInfo.unk38.y = 128.0 * (0.5 + RAND_FLOAT());
    backgroundInfo.unk38.z = 18.204444444444444 * (RAND_FLOAT() - 0.5);
    if (rand() & 1)
        backgroundInfo.unk38.y = -backgroundInfo.unk38.y;
    scale = 1.0 / (3.5 + (1.5 * mathutil_sin(globalAnimTimer << 6)));
    mathutil_mtxA_rotate_y((s16)backgroundInfo.unk2C.y);
    mathutil_mtxA_rotate_x((s16)backgroundInfo.unk2C.x);
    mathutil_mtxA_rotate_z((s16)backgroundInfo.unk2C.z);
    mathutil_mtxA_scale_xyz(scale, scale, scale);
    mathutil_mtxA_to_mtx(backgroundInfo.unk48);

    memset(&effect, 0, sizeof(effect));
    effect.type = ET_TWINKLE_STAR;
    effect.model = (void *)g_bgNlObj->models[1];
    effect.u_otherTimer = 0;
    nameLen = strlen(starpointObjName);
    var_r25 = g_bgNlObj->models;
    while (*var_r25 != NULL)
    {
        if (strncmp(NLMODEL_HEADER(*var_r25)->unk0->unk0, starpointObjName, nameLen) == 0)
        {
            effect.pos = (*var_r25)->boundSphereCenter;
            spawn_effect(&effect);
            effect.u_otherTimer += 30.0;
        }
        var_r25++;
    }

    memset(&effect, 0, sizeof(effect));
    effect.type = ET_BALL_GLOW;
    spawn_effect(&effect);

    memset(&effect, 0, sizeof(effect));
    effect.type = ET_EXM_GUIDE_LIGHT;
    if (decodedStageLzPtr->animGroupModels == NULL)
    {
        var_r27 = &lbl_802099E8[1];
        for (i = 1; i < animGroupCount; i++, var_r27++)
        {
            effect.u_otherTimer = i;
            for (j = 0; j < var_r27->unk8; j++)
            {
                model = (void *)var_r27->unk0[j];
                if (model != NULL)
                {
                    effect.unk88 = model->boundSphereCenter;
                    spawn_effect(&effect);
                }
            }
        }
    }
    else
    {
        var_r27_2 = &lbl_8020A348[1];
        for (i = 1; i < animGroupCount; i++, var_r27_2++)
        {
            effect.u_otherTimer = i;
            var_r28 = var_r27_2->unk0;
            for (j = 0; j < var_r27_2->unk4; j++, var_r28++)
            {
                if (var_r28->flags == 1U)
                {
                    model = (void *)var_r28->model;
                    if (model != NULL)
                    {
                        effect.unk88 = model->boundSphereCenter;
                        spawn_effect(&effect);
                    }
                }
            }
        }
    }
}

void bg_old_extramaster_main(void)
{
    float scale;

    if (debugFlags & 0xA)
        return;
    backgroundInfo.unk14.x += backgroundInfo.unk20.x;
    backgroundInfo.unk14.y += backgroundInfo.unk20.y;
    backgroundInfo.unk14.z += backgroundInfo.unk20.z;
    mathutil_mtxA_from_translate(&backgroundInfo.unk14);
    backgroundInfo.unk2C.x += backgroundInfo.unk38.x;
    backgroundInfo.unk2C.y += backgroundInfo.unk38.y;
    backgroundInfo.unk2C.z += backgroundInfo.unk38.z;
    if (backgroundInfo.unk2C.x < 0.0)
        backgroundInfo.unk2C.x += 65536.0f;
    else if (backgroundInfo.unk2C.x > 65536.0f)
        backgroundInfo.unk2C.x -= 65536.0f;
    if (backgroundInfo.unk2C.y < 0.0)
        backgroundInfo.unk2C.y += 65536.0f;
    else if (backgroundInfo.unk2C.y > 65536.0f)
        backgroundInfo.unk2C.y -= 65536.0f;
    if (backgroundInfo.unk2C.z < 0.0)
        backgroundInfo.unk2C.z += 65536.0f;
    else if (backgroundInfo.unk2C.z > 65536.0f)
        backgroundInfo.unk2C.z -= 65536.0f;
    mathutil_mtxA_rotate_y((s16)backgroundInfo.unk2C.y);
    mathutil_mtxA_rotate_x((s16)backgroundInfo.unk2C.x);
    mathutil_mtxA_rotate_z((s16)backgroundInfo.unk2C.z);
    scale = 1.0 / (3.5 + (1.5 * mathutil_sin(globalAnimTimer << 6)));
    mathutil_mtxA_scale_xyz(scale, scale, scale);
    mathutil_mtxA_to_mtx(backgroundInfo.unk48);
}

void bg_old_extramaster_finish(void) {}

void bg_old_extramaster_draw(void)
{
    mathutil_mtxA_from_mtx(userWork->matrices[1]);
    nl2ngc_draw_model_sort_translucent_alt2(g_bgNlObj->models[40]);
}

void bg_old_extramaster_interact(int arg0)
{
    struct Effect effect;
    Vec spC;
    float temp_f31;

    switch (arg0)
    {
    case 0:
    case 1:
        memset(&effect, 0, sizeof(effect));
        effect.type = ET_BONUS_STG_STAR;
        mathutil_mtxA_from_mtx(userWork->matrices[1]);
        spC.z = -180.0 + (-300.0 * RAND_FLOAT());
        spC.x = -2.6666666666666665 * spC.z * currentCamera->sub28.unk38 * (RAND_FLOAT() - 0.5);
        spC.y = -1.1 * spC.z * currentCamera->sub28.unk38;
        mathutil_mtxA_rigid_inv_tf_point(&spC, &effect.pos);
        temp_f31 = 0.0033333333333333335 * -spC.z;
        effect.vel.x = temp_f31 * (1.0 + RAND_FLOAT());
        effect.vel.y = temp_f31 * (-3.0 + (-1.0 * RAND_FLOAT()));
        effect.vel.z = temp_f31 * (1.0 + RAND_FLOAT());
        spC.x = 0.0f;
        spC.y = 0.0f;
        spC.z = 0.0f;
        mathutil_mtxA_rigid_inv_tf_point(&spC, &spC);
        mathutil_ray_to_euler_xy(&spC, &effect.pos, &effect.rotX, &effect.rotY);
        effect.rotZ = rand() & 0x7FFF;
        spawn_effect(&effect);
        break;
    }
}
