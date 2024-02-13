#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "effect.h"
#include "mathutil.h"
#include "mt_effect.h"
#include "nl2ngc.h"

void bg_old_ice_init(void)
{
    struct Effect sp8;

    backgroundInfo.unk7C = NULL;
    MTEfcChangeAttributeITI(g_bgNlObj->models[1], 0xFBFFFFFF, 0x04000000);
    MTEfcChangeAttributeITI(g_bgNlObj->models[2], 0xFBFFFFFF, 0x04000000);
    memset(&sp8, 0, 0xACU);
    sp8.type = 5;
    sp8.model = (void *)g_bgNlObj->models[1];
    sp8.unk64.x = 0.0f;
    sp8.unk64.y = -1100.0f;
    sp8.unk64.z = 0.0f;
    sp8.unk70.x = 0.0f;
    sp8.unk70.y = 1.0f;
    sp8.unk70.z = 0.0f;
    spawn_effect(&sp8);
}

void bg_old_ice_main(void) {}

void bg_old_ice_finish(void) {}

void bg_old_ice_draw(void)
{
    mathutil_mtxA_from_mtx(userWork->matrices[1]);
    nl2ngc_draw_model_sort_translucent_alt2(g_bgNlObj->models[0]);
}

void bg_old_ice_interact(int arg0) {}

void effect_icewater_reflect_init(struct Effect *arg0)
{
    arg0->flags |= 1;
    arg0->u_otherTimer = (rand() & 0xF) + 8;
    mathutil_vec_to_euler_xy(&arg0->unk70, &arg0->rotX, &arg0->rotY);
    arg0->rotX += 0x8000;
}

void effect_icewater_reflect_main(struct Effect *arg0)
{
    Vec spB8;
    struct Effect spC;

    if (!(arg0->flags & 1))
    {
        arg0->u_otherTimer--;
        if (arg0->u_otherTimer < 0)
        {
            arg0->u_otherTimer = (rand() & 0xF) + 0x10;
            memset(&spC, 0, sizeof(spC));
            spC.type = 6;
            mathutil_mtxA_from_mtx(userWork->matrices[1]);
            mathutil_mtxA_rigid_inv_tf_point(&arg0->pos, &spC.pos);
            spC.rotX = arg0->rotX;
            spC.rotY = arg0->rotY;
            spC.rotZ = arg0->rotZ;
            spB8.x = RAND_FLOAT() - 0.5;
            spB8.y = RAND_FLOAT() - 0.5;
            spB8.z = 0.0f;
            mathutil_vec_normalize_len((Point3d *) &spB8);
            spB8.x = spB8.x * (0.075 * arg0->scale.x);
            spB8.y = spB8.y * (0.075 * arg0->scale.y);
            spB8.z = 0.01 + (0.01 * RAND_FLOAT());
            mathutil_mtxA_from_rotate_y(spC.rotY);
            mathutil_mtxA_rotate_x(spC.rotX);
            mathutil_mtxA_rotate_z(spC.rotZ);
            mathutil_mtxA_tf_vec(&spB8, &spB8);
            spB8.x = 0.000125 * (arg0->scale.x * (1.0 + RAND_FLOAT()));
            spB8.y = 0.000125 * (arg0->scale.y * (1.0 + RAND_FLOAT()));
            spB8.z = 0.0f;
            mathutil_mtxA_tf_vec(&spB8, &spC.vel);
            spC.scale.x = 0.005 * (arg0->scale.x * (1.0 + RAND_FLOAT()));
            spC.scale.y = 0.005 * (arg0->scale.y * (1.0 + RAND_FLOAT()));
            spC.scale.z = 1.0f;
            spC.model = (void *)g_bgNlObj->models[2];
            spC.timer = (rand() & 0xF) + 0x20;
            spawn_effect(&spC);
        }
    }
    arg0->flags &= 0xFFFFFFFE;
}

void effect_icewater_reflect_draw(struct Effect *arg0)
{
    Vec sp48;                   
    Vec sp3C;
    Vec sp30;
    Vec sp24;
    Vec sp18;
    Vec spC;
    float temp_f1;
    float temp_f1_3;
    float temp_f30;
    float temp_f30_2;
    float temp_f29;
    float temp_f29_2;
    float temp_f2;
    float temp_f31;
    float temp_f31_2;
    float temp_f3;
    float temp_f5;
    float temp_f5_3;
    float temp_f5_4;
    float temp_f7;

    if (arg0->model != NULL)
    {
        arg0->flags |= 1;
        mathutil_mtxA_from_mtx(userWork->matrices[1]);
        sp24 = arg0->unk70;
        mathutil_mtxA_tf_vec(&sp24, &sp24);
        sp18 = arg0->unk64;
        mathutil_mtxA_tf_point(&sp18, &sp18);
        sp48.x = -sp18.x;
        sp48.y = -sp18.y;
        sp48.z = -sp18.z;
        temp_f31 = mathutil_vec_dot_prod(&sp48, &sp24);
        if (temp_f31 <= 0.0)
            return;
        sp3C.x = -0.5f;
        sp3C.y = 0.707107f;
        sp3C.z = 0.5f;
        mathutil_mtxA_tf_vec(&sp3C, &sp3C);
        temp_f2 = mathutil_vec_dot_prod(&sp3C, &sp24);
        if (temp_f2 <= 0.0)
            return;
        temp_f7 = temp_f2;
        temp_f7 *= -2.0;
        sp30.x = sp3C.x + (temp_f7 * sp24.x);
        sp30.y = sp3C.y + (temp_f7 * sp24.y);
        sp30.z = sp3C.z + (temp_f7 * sp24.z);
        spC.x = arg0->pos.x;
        spC.y = arg0->pos.y;
        spC.z = arg0->pos.z;
        temp_f7 = temp_f31 / (-0.5 * temp_f7);
        arg0->pos.x = sp30.x * temp_f7;
        arg0->pos.y = sp30.y * temp_f7;
        arg0->pos.z = sp30.z * temp_f7;
        arg0->vel.x = arg0->pos.x - spC.x;
        arg0->vel.y = arg0->pos.y - spC.y;
        arg0->vel.z = arg0->pos.z - spC.z;
        mathutil_mtxA_set_translate(&arg0->pos);
        mathutil_mtxA_rotate_y(arg0->rotY);
        mathutil_mtxA_rotate_x(arg0->rotX);
        mathutil_mtxA_rigid_inv_tf_tl(&sp48);
        arg0->rotZ = -mathutil_atan2(sp48.y, sp48.x);
        mathutil_mtxA_rotate_z(arg0->rotZ);
        temp_f31_2 = mathutil_vec_len(&arg0->pos);
        sp48.x = 1.0f;
        sp48.y = 0.0f;
        sp48.z = 0.0f;
        mathutil_mtxA_tf_vec(&sp48, &sp48);
        temp_f5 = mathutil_vec_dot_prod(&sp3C, &sp48);
        temp_f30 = temp_f5 - 0.5868240888043891;
        temp_f29 = 0.8263518223912218 * temp_f5;
        temp_f1 = (temp_f29 * temp_f29) - (1.6527036447824437 * temp_f30);
        if (temp_f1 < 0.0)
            return;
        temp_f1_3 = mathutil_sqrt(temp_f1);
        temp_f3 = -temp_f29;
        temp_f29_2 = (temp_f3 + temp_f1_3) / (2.0 * temp_f30);
        temp_f30_2 = (temp_f3 - temp_f1_3) / (2.0 * temp_f30);
        mathutil_mtxA_translate_xyz(0.5 * (temp_f31_2 * (temp_f29_2 + temp_f30_2)), 0.0f, 0.0f);
        arg0->scale.x = (temp_f31_2 * fabs(temp_f29_2 - temp_f30_2));
        mathutil_mtxA_scale_xyz(arg0->scale.x, 1.0f, 1.0f);
        sp48.x = 0.0f;
        sp48.y = 1.0f;
        sp48.z = 0.0f;
        mathutil_mtxA_tf_vec(&sp48, &sp48);
        temp_f5_3 = mathutil_vec_dot_prod(&sp3C, &sp48);
        temp_f30 = (temp_f5_3 - 0.5868240888043891);
        temp_f29 = (0.8263518223912218 * temp_f5_3);
        temp_f1 = (temp_f29 * temp_f29) - (1.6527036447824437 * temp_f30);
        if (temp_f1 < 0.0)
            return;
        temp_f1_3 = mathutil_sqrt(temp_f1);
        temp_f5_4 = -temp_f29;
        temp_f29_2 = (temp_f5_4 + temp_f1_3) / (2.0 * temp_f30);
        temp_f30_2 = (temp_f5_4 - temp_f1_3) / (2.0 * temp_f30);
        mathutil_mtxA_translate_xyz(0.0f, 0.5 * (temp_f31_2 * (temp_f29_2 + temp_f30_2)), 0.0f);
        arg0->scale.y = (temp_f31_2 * fabs(temp_f29_2 - temp_f30_2));
        mathutil_mtxA_scale_xyz(1.0f, arg0->scale.y, 1.0f);
        nlSetScaleFactor(MAX(arg0->scale.x, arg0->scale.y));
        nl2ngc_draw_model_sort_translucent_alt2((struct NlModel *) arg0->model);
        mathutil_mtxA_translate_xyz(0.0f, 0.0f, 0.001f);
        mathutil_mtxA_scale_xyz(0.01f, 0.01f, 1.0f);
        nlSetScaleFactor(0.01 * MAX(arg0->scale.x, arg0->scale.y));
        nl2ngc_draw_model_sort_translucent_alt2(g_bgNlObj->models[2]);
        arg0->flags &= 0xFFFFFFFE;
    }
}

void effect_icewater_reflect_destroy(struct Effect *arg0)
{
    arg0->model = NULL;
}

void effect_icewater_particle_init(struct Effect *arg0)
{
    arg0->unk56 = (rand() & 0x1F) - 0xF;
    arg0->pos.y += 0.1 * (float)(arg0->poolIndex & 0xF);
    arg0->colorFactor = 0.0f;
}

void effect_icewater_particle_main(struct Effect *arg0)
{
    arg0->vel.x *= 0.995;
    arg0->vel.y *= 0.995;
    arg0->vel.z *= 0.995;
    arg0->pos.x += arg0->vel.x;
    arg0->pos.y += arg0->vel.y;
    arg0->pos.z += arg0->vel.z;
    arg0->unkA4 += arg0->unk56;
    if (arg0->timer > 30.0)
        arg0->colorFactor += 0.25 * (0.75 - arg0->colorFactor);
    else
        arg0->colorFactor *= 0.94;
}

void effect_icewater_particle_draw(struct Effect *arg0)
{
    mathutil_mtxA_from_mtx(userWork->matrices[1]);
    mathutil_mtxA_translate(&arg0->pos);
    mathutil_mtxA_rotate_y(arg0->rotY);
    mathutil_mtxA_rotate_x(arg0->rotX);
    mathutil_mtxA_rotate_z(arg0->rotZ);
    mathutil_mtxA_scale_xyz(arg0->scale.x, arg0->scale.y, 1.0f);
    nlSetScaleFactor(MAX(arg0->scale.x, arg0->scale.y));
    mathutil_mtxA_rotate_z(arg0->unkA4);
    nlObjPutSetFadeColorBase(arg0->colorFactor, arg0->colorFactor, arg0->colorFactor);
    nl2ngc_draw_model_sort_translucent_alt2((struct NlModel *)arg0->model);
    fade_color_base_default();
}

void effect_icewater_particle_destroy(struct Effect *arg0)
{
    arg0->model = NULL;
}
