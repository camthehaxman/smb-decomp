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

extern s32 lbl_802F1F4C;
extern s32 lbl_802F1F50;

void bg_old_water_init(void)
{
    float temp_f2;

    backgroundInfo.unk2 = 0;
    backgroundInfo.unk84 = 0.0f;
    backgroundInfo.unk44 = 0.0f;
    backgroundInfo.unk14 = decodedStageLzPtr->startPos->pos;
    backgroundInfo.unk2C.x = 65536.0f * RAND_FLOAT();
    backgroundInfo.unk2C.y = 65536.0f * RAND_FLOAT();
    backgroundInfo.unk2C.z = 65536.0f * RAND_FLOAT();
    backgroundInfo.unk38.x = (128.0f * RAND_FLOAT()) - 64.0f;
    backgroundInfo.unk38.y = (128.0f * RAND_FLOAT()) - 64.0f;
    backgroundInfo.unk38.z = (128.0f * RAND_FLOAT()) - 64.0f;
    if (decodedStageLzPtr->goalsCount == 0)
    {
        backgroundInfo.unk20.x = RAND_FLOAT() - 0.5;
        backgroundInfo.unk20.y = RAND_FLOAT() - 0.5;
        backgroundInfo.unk20.z = RAND_FLOAT() - 0.5;
        mathutil_vec_set_len(&backgroundInfo.unk20, &backgroundInfo.unk20, 0.02 * RAND_FLOAT());
    }
    else
    {
        temp_f2 = (0.5 + RAND_FLOAT()) / 7200.0;
        backgroundInfo.unk20.x = temp_f2 * (decodedStageLzPtr->goals[0].pos.x - backgroundInfo.unk14.x);
        backgroundInfo.unk20.y = temp_f2 * (decodedStageLzPtr->goals[0].pos.y - backgroundInfo.unk14.y);
        backgroundInfo.unk20.z = temp_f2 * (decodedStageLzPtr->goals[0].pos.z - backgroundInfo.unk14.z);
    }
    backgroundInfo.unk88 = 1.8e-06 * (float)MAX(lbl_802F1F50, lbl_802F1F4C);
    func_8003026C(2, "background.lm_stg_cpu_load_f:%7.7f\n", backgroundInfo.unk88);
}

void bg_old_water_main(void)
{
    Vec spC0;
    Vec spB4;
    struct Effect effect;
    float temp_f31;
    float temp_f31_2;
    float temp_f31_3;
    int rotX;
    int rotY;
    int rotZ;

    if (debugFlags & 0xA)
        return;
    if (!(globalAnimTimer & 3))
    {
        mathutil_mtxA_from_mtx(userWork->matrices[1]);
        memset(&effect, 0, sizeof(effect));
        effect.type = ET_WATER_LIGHT;
        spB4.x = 0.0f;
        spB4.y = 0.0f;
        spB4.z = -1.0f;
        mathutil_mtxA_rigid_inv_tf_vec(&spB4, &spB4);
        mathutil_mtxA_rigid_inv_tf_tl(&effect.pos);
        if (spB4.y != 0.0)
        {
            if (spB4.y < 0.0)
                spB4.y = -spB4.y;
            temp_f31 = (70.0 - effect.pos.y) / spB4.y;
            effect.pos.x += 0.25 * (spB4.x * temp_f31);
            effect.pos.y = 70.0f;
            effect.pos.z += 0.25 * (spB4.z * temp_f31);
            spB4.x = RAND_FLOAT() - 0.5;
            spB4.y = 0.0001f;
            spB4.z = RAND_FLOAT() - 0.5;
            temp_f31 *= 0.1 + (0.75 * RAND_FLOAT());
            mathutil_vec_set_len(&spB4, &spB4, temp_f31);
            effect.pos.x = effect.pos.x + spB4.x;
            effect.pos.z += spB4.z;
            effect.model = (void *)g_bgNlObj->models[5];
            spawn_effect(&effect);
        }
        memset(&effect, 0, sizeof(effect));
        effect.type = ET_BUBBLE;
        temp_f31_2 = currentCamera->sub28.unk38;
        effect.pos.x = 4.0 * temp_f31_2 * (RAND_FLOAT() - 0.5);
        effect.pos.y = -1.0 * temp_f31_2 * RAND_FLOAT();
        effect.pos.z = -1.0f;
        mathutil_vec_set_len(&effect.pos, &effect.pos, 30.0 + (20.0 * RAND_FLOAT()));
        mathutil_mtxA_rigid_inv_tf_point(&effect.pos, &effect.pos);
        effect.vel.x = 0.01 * RAND_FLOAT();
        effect.vel.y = 0.01 * (RAND_FLOAT() - 0.5);
        effect.vel.z = 0.01 * (RAND_FLOAT() - 0.5);
        effect.rotX = rand() & 0x7FFF;
        effect.rotY = rand() & 0x7FFF;
        effect.rotZ = rand() & 0x7FFF;
        effect.model = (void *)g_bgNlObj->models[4];
        spawn_effect(&effect);
    }
    switch (backgroundInfo.unk2)
    {
    case 0:
        backgroundInfo.unk2 = 1;
        backgroundInfo.unk10 = 120.0 * RAND_FLOAT();
        /* fallthrough */
    case 1:
        backgroundInfo.unk84 += 0.05 * (1.0 - backgroundInfo.unk84);
        backgroundInfo.unk10--;
        if (backgroundInfo.unk10 < 0)
            backgroundInfo.unk2 = 2;
        break;
    case 2:
        backgroundInfo.unk2 = 3;
        backgroundInfo.unk10 = 90.0 * RAND_FLOAT();
        /* fallthrough */
    case 3:
        backgroundInfo.unk84 += 0.025 * (0.5 - backgroundInfo.unk84);
        backgroundInfo.unk10--;
        if (backgroundInfo.unk10 < 0)
            backgroundInfo.unk2 = 0;
        break;
    }
    if (backgroundInfo.unk44 > 0.0)
    {
        if (backgroundInfo.unk44 < 1.0)
        {
            backgroundInfo.unk44 += 0.016666666666666666;
            if (backgroundInfo.unk44 > 1.0)
                backgroundInfo.unk44 = 1.0f;
        }
    }
    else
        backgroundInfo.unk44 += 0.016666666666666666;

    backgroundInfo.unk20.x *= 0.99999;
    backgroundInfo.unk20.y *= 0.999;
    backgroundInfo.unk20.z *= 0.9999;
    backgroundInfo.unk14.x += backgroundInfo.unk20.x;
    backgroundInfo.unk14.y += backgroundInfo.unk20.y;
    backgroundInfo.unk14.z += backgroundInfo.unk20.z;
    backgroundInfo.unk2C.x += backgroundInfo.unk38.x;
    backgroundInfo.unk2C.y += backgroundInfo.unk38.y;
    backgroundInfo.unk2C.z += backgroundInfo.unk38.z;
    mathutil_mtxA_from_mtxB();
    mathutil_mtxA_rigid_inv_tf_tl(&spB4);
    temp_f31_3 = 1.0 / (1.0 + (0.02 * mathutil_vec_distance(&spB4, &backgroundInfo.unk14)));
    rotX = 8192.0f * temp_f31_3 * mathutil_sin(backgroundInfo.unk2C.x);
    rotY = 8192.0f * temp_f31_3 * mathutil_sin(backgroundInfo.unk2C.y);
    rotZ = 2048.0f * temp_f31_3 * mathutil_sin(backgroundInfo.unk2C.z);
    spC0.x = 1.0 / (4.0 + (2.0 * temp_f31_3 * mathutil_sin((globalAnimTimer << 6) + rotY)));
    spC0.y = 1.0 / (4.0 + (2.0 * temp_f31_3 * mathutil_sin((globalAnimTimer << 4) + rotX)));
    mathutil_mtxA_from_translate(&backgroundInfo.unk14);
    mathutil_mtxA_rotate_x(-0x4000);
    mathutil_mtxA_rotate_y(rotY);
    mathutil_mtxA_rotate_x(rotX);
    mathutil_mtxA_rotate_z(rotZ);
    mathutil_mtxA_scale_xyz(spC0.x, spC0.y, spC0.y);
    mathutil_mtxA_to_mtx(backgroundInfo.unk48);
}

void bg_old_water_finish(void) {}

void func_8005AEA8(struct NlVtxTypeA *);
void func_8005AFB8(struct NlVtxTypeB *);
void func_8005B0C8(struct NlVtxTypeA *);

void bg_old_water_draw(void)
{
    struct NlModel *model;

    mathutil_mtxA_from_mtx(userWork->matrices[1]);
    nl2ngc_draw_model_sort_translucent_alt2(g_bgNlObj->models[0]);

    model = g_bgNlObj->models[1];
    memcpy(lbl_802F1B4C, model, NLMODEL_HEADER(model)->unk4->modelSize);
    u_apply_func_to_nl_model_vertices(lbl_802F1B4C, NULL, func_8005AEA8);
    mathutil_mtxA_from_mtx(userWork->matrices[1]);
    nl2ngc_draw_model_sort_none_alt2(lbl_802F1B4C);

    model = g_bgNlObj->models[3];
    memcpy(lbl_802F1B4C, model, NLMODEL_HEADER(model)->unk4->modelSize);
    u_apply_func_to_nl_model_vertices(lbl_802F1B4C, func_8005AFB8, func_8005B0C8);
    mathutil_mtxA_from_mtx(userWork->matrices[1]);
    nl2ngc_draw_model_sort_none_alt2(lbl_802F1B4C);
}

void bg_old_water_interact(int arg0) {}

int func_8005AD80(struct NlModel *arg0, struct NlModel *arg1)
{
    float sp10;

    if (backgroundInfo.unk44 == 0.0 || backgroundInfo.unk84 == 0.0)
        return 0;
    sp10 = backgroundInfo.unk84 * backgroundInfo.unk44;
    memcpy(arg1, arg0, NLMODEL_HEADER(arg0)->unk4->modelSize);
    mathutil_mtxA_mult_left((f32 (*)[4]) backgroundInfo.unk48[0]);
    func_80048084(arg1, g_bgNlObj->models[2], &sp10);
    return 1;
}

int func_8005AE1C(struct NlModel *arg0, struct NlModel *arg1)
{
    float sp10;

    if (backgroundInfo.unk84 == 0.0)
        return 0;
    sp10 = backgroundInfo.unk84;
    memcpy(arg1, arg0, NLMODEL_HEADER(arg0)->unk4->modelSize);
    mathutil_mtxA_mult_left((f32 (*)[4]) backgroundInfo.unk48[0]);
    func_80048084(arg1, g_bgNlObj->models[2], &sp10);
    return 1;
}

void func_8005AEA8(struct NlVtxTypeA *arg0)
{
    int rotX, rotY, rotZ;

    rotX = 192.0f * mathutil_sin(((int)arg0->z << 10) + (globalAnimTimer << 8));
    rotY = 2048.0f * mathutil_sin(((int)arg0->y << 14) + (globalAnimTimer << 7));
    rotZ = 192.0f * mathutil_sin(((int)arg0->x << 8) + (globalAnimTimer << 9));
    mathutil_mtxA_from_rotate_y(rotY);
    mathutil_mtxA_rotate_x(rotX);
    mathutil_mtxA_rotate_z(rotZ);
    mathutil_mtxA_tf_vec((Vec *)arg0, (Vec *)arg0);
    *(u32 *)&arg0->x |= 1;
}

void func_8005AFB8(struct NlVtxTypeB *arg0)
{
    int rotX, rotY, rotZ;

    rotX = 192.0f * mathutil_sin(((int)arg0->z << 11) + (globalAnimTimer << 5));
    rotY = 512.0f * mathutil_sin(((int)arg0->y << 13) + (globalAnimTimer << 6));
    rotZ = 192.0f * mathutil_sin(((int)arg0->x << 9) + (globalAnimTimer << 7));
    mathutil_mtxA_from_rotate_y((s16) rotY);
    mathutil_mtxA_rotate_x(rotX);
    mathutil_mtxA_rotate_z(rotZ);
    mathutil_mtxA_tf_vec((Vec *)arg0, (Vec *)arg0);
    *(u32 *)&arg0->x |= 1;
}

void func_8005B0C8(struct NlVtxTypeA *arg0)
{
    int rotX, rotY, rotZ;

    rotX = 256.0f * mathutil_sin(((int)arg0->z << 11) + (globalAnimTimer << 8));
    rotY = 1024.0f * mathutil_sin(((int)arg0->y << 13) - (globalAnimTimer << 6));
    rotZ = 256.0f * mathutil_sin(((int)arg0->x << 9) + (globalAnimTimer << 7));
    mathutil_mtxA_from_rotate_y(rotY);
    mathutil_mtxA_rotate_x(rotX);
    mathutil_mtxA_rotate_z(rotZ);
    mathutil_mtxA_tf_vec((Vec *)arg0, (Vec *)arg0);
    *(u32 *)&arg0->x |= 1;
}
