#include <stdlib.h>
#include <string.h>
#include <dolphin.h>

#include "global.h"
#include "background.h"
#include "ball.h"
#include "camera.h"
#include "effect.h"
#include "mathutil.h"
#include "mode.h"
#include "nl2ngc.h"
#include "stage.h"

#define SOME_MACRO(param) ((gameSubmode == SMD_ADV_DEMO_INIT || gameSubmode == SMD_ADV_DEMO_MAIN) ? (param) : (rand() & 0x7FFF))

void lbl_80057940(struct NlVtxTypeA *);

void bg_old_bluesky_init(void)
{
    struct Effect effect;
    Point3d sp8;
    u32 var_r31;
    u32 var_r30;
    int i = 0;
    float temp_f28;

    backgroundInfo.unk7C = NULL;
    mathutil_mtxA_from_identity();
    mathutil_mtxA_scale_xyz(0.33333334f, 0.33333334f, 0.33333334f);
    mathutil_mtxA_to_mtx(backgroundInfo.unk48);
    if (gameMode == 1)
		return;
	temp_f28 = 0.5 * g_bgNlObj->models[1]->boundSphereRadius;
	sp8.x = 0.0f;
	sp8.y = 0.0f;
	sp8.z = 0.75 * g_bgNlObj->models[1]->boundSphereRadius;
	mathutil_mtxA_from_identity();
	mathutil_mtxA_rotate_y(SOME_MACRO(-0x6000));
	mathutil_mtxA_tf_vec(&sp8, &sp8);
	memset(&effect, 0, sizeof(effect));
	effect.type = 3;
	var_r31 = 0x15000;
	effect.unk88.y = 12.5f;
	effect.pos.y = decodedStageLzPtr->startPos->pos.y + effect.unk88.y;
	for (i = 16, var_r30 = 0x150; i > 0; i--)
	{
		if (gameSubmode == SMD_ADV_DEMO_INIT || gameSubmode == SMD_ADV_DEMO_MAIN)
		{
			effect.rotY = (u16)var_r31;
			effect.unk54 = (u8)var_r30 + 0x30;
			if (i & 1)
				effect.unk54 = -effect.unk54;
			effect.pos.x = sp8.x + (temp_f28 * ((i / 16) - 0.5));
			effect.pos.z = sp8.z + (temp_f28 * ((i / 16) - 0.5));
		}
		else
		{
			effect.rotY = rand() & 0x7FFF;
			effect.unk54 = (rand() & 0xFF) + 0x30;
			if (rand() & 1)
				effect.unk54 = -effect.unk54;
			effect.pos.x = sp8.x + (temp_f28 * (RAND_FLOAT() - 0.5));
			effect.pos.z = sp8.z + (temp_f28 * (RAND_FLOAT() - 0.5));
		}
		spawn_effect(&effect);
		var_r31 -= 0x1500;
		var_r30 -= 0x15;
		effect.pos.y += 5.0;
		effect.unk88.y += 5.0;
	}
	memset(&effect, 0, sizeof(effect));
	effect.type = 4;
	effect.unkA0 = (SOME_MACRO(0) & 0xFFF) - 0x7FF;
	effect.unkA2 = (SOME_MACRO(0xABC) & 0xFFF) - 0x7FF;
	effect.unkA4 = (SOME_MACRO(0x1578) & 0xFFF) - 0x7FF;
	effect.unk54 = SOME_MACRO(0x2034) & 0xF;
	effect.model = (void *)g_bgNlObj->models[1];
	spawn_effect(&effect);
	effect.unkA0 = (SOME_MACRO(0x2AF0) & 0xFFF) - 0x7FF;
	effect.unkA2 = (SOME_MACRO(0x35AC) & 0xFFF) - 0x7FF;
	effect.unkA4 = (SOME_MACRO(0x4068) & 0xFFF) - 0x7FF;
	effect.unk54 = SOME_MACRO(0x4B24) & 0xF;
	effect.model = (void *)g_bgNlObj->models[2];
	spawn_effect(&effect);
	effect.unkA0 = (SOME_MACRO(0x55E0) & 0xFFF) - 0x7FF;
	effect.unkA2 = (SOME_MACRO(0x609C) & 0xFFF) - 0x7FF;
	effect.unkA4 = (SOME_MACRO(0x6B58) & 0xFFF) - 0x7FF;
	effect.unk54 = SOME_MACRO(0x7614) & 0xF;
	effect.model = (void *)g_bgNlObj->models[3];
	spawn_effect(&effect);
	effect.unkA0 = (SOME_MACRO(0x80D0) & 0x7FF) - 0x7FF;
	effect.unkA2 = (SOME_MACRO(0x8B8C) & 0x7FF) - 0x7FF;
	effect.unkA4 = (SOME_MACRO(0x9648) & 0x7FF) - 0x7FF;
	effect.unk54 = SOME_MACRO(0xA104) & 0xF;
	effect.model = (void *)g_bgNlObj->models[5];
	spawn_effect(&effect);
	effect.unkA0 = (SOME_MACRO(0xABC0) & 0xFFF) - 0x7FF;
	effect.unkA2 = (SOME_MACRO(0xB67C) & 0xFFF) - 0x7FF;
	effect.unkA4 = (SOME_MACRO(0xC138) & 0xFFF) - 0x7FF;
	effect.unk54 = SOME_MACRO(0xCBF4) & 0xF;
	effect.model = (void *)g_bgNlObj->models[6];
	spawn_effect(&effect);
	effect.unkA0 = (SOME_MACRO(0xD6B0) & 0xFFF) - 0x7FF;
	effect.unkA2 = (SOME_MACRO(0xE16C) & 0xFFF) - 0x7FF;
	effect.unkA4 = (SOME_MACRO(0xEC28) & 0xFFF) - 0x7FF;
	effect.unk54 = SOME_MACRO(0xF6E4) & 0xF;
	effect.model = (void *)g_bgNlObj->models[7];
	spawn_effect(&effect);
	u_apply_func_to_nl_model_vertices(g_bgNlObj->models[11], NULL, lbl_80057940);
}

void bg_old_bluesky_main(void) {}

void bg_old_bluesky_finish(void) {}

void bg_old_bluesky_draw(void)
{
    mathutil_mtxA_from_mtx(userWork->matrices[1]);
    nl2ngc_draw_model_sort_translucent_alt2(g_bgNlObj->models[4]);
    nl2ngc_draw_model_sort_translucent_alt(g_bgNlObj->models[11]);
}

void bg_old_bluesky_interact(int arg0) {}

void effect_bird_kite_init(struct Effect *arg0)
{
    arg0->timer = -1;
    arg0->state = 0;
    arg0->u_otherTimer = 30.0 + ((gameSubmode == SMD_ADV_DEMO_INIT || gameSubmode == SMD_ADV_DEMO_MAIN) ? 300.0 / (arg0->poolIndex + 1) : 300.0 * RAND_FLOAT());
    arg0->model = (void *)g_bgNlObj->models[0];
    arg0->unkA2 = arg0->unk54;
}

void effect_bird_kite_main(struct Effect *arg0)
{
	Mtx sp18;
    Vec spC;
    struct Ball *ball = currentBall;
    float temp_f5;
    float var_f31;
    int var_r30;

    switch (arg0->state)
    {
    case 0:
        var_f31 = -0.05f;
        spC.x = currentCamera->eye.x - arg0->pos.x;
        spC.y = 0.0f;
        spC.z = currentCamera->eye.z - arg0->pos.z;
        var_r30 = arg0->unkA2;
        if (mathutil_vec_sq_len(&spC) < 22500.0)
        {
            mathutil_vec_normalize_len(&spC);
            temp_f5 = mathutil_vec_dot_prod(&arg0->vel, &spC);
            if (temp_f5 > 0.0)
                var_r30 = 4.0 * (arg0->unkA2 * temp_f5);
        }
        arg0->unk54 += (var_r30 - arg0->unk54) >> 3;
        arg0->unk56 += ((arg0->unk54 * 16) - arg0->rotZ - arg0->unk56) >> 4;
        arg0->vel.y += -0.008;
        arg0->u_otherTimer -= 1;
        if (arg0->u_otherTimer < 0.0)
        {
            arg0->state = 1;
            arg0->unk88.y += ((gameSubmode == SMD_ADV_DEMO_INIT || gameSubmode == SMD_ADV_DEMO_MAIN) ? 1 / (arg0->poolIndex + 1) : RAND_FLOAT()) - 0.5;
        }
        break;
    default:
    case 1:
        var_f31 = -0.02f;
        arg0->unk54 -= arg0->unk54 >> 3;
        arg0->unk56 += -(arg0->rotZ + arg0->unk56) >> 4;
        arg0->vel.y += 0.016;
        if (arg0->pos.y > (ball->pos.y + arg0->unk88.y))
        {
            arg0->state = 0;
            arg0->u_otherTimer = 30.0 + 300.0 * ((gameSubmode == SMD_ADV_DEMO_INIT || gameSubmode == SMD_ADV_DEMO_MAIN) ? 1 / (arg0->poolIndex + 1) : RAND_FLOAT());
        }
        break;
    }
    arg0->rotX += arg0->unk52;
    arg0->rotY += arg0->unk54;
    arg0->rotZ += arg0->unk56;
    mathutil_mtxA_from_rotate_y(arg0->rotY);
    mathutil_mtxA_rotate_x(arg0->rotX);
    mathutil_mtxA_to_mtx(sp18);
    spC.x = sp18[0][2];
    spC.y = sp18[1][2];
    spC.z = sp18[2][2];
    arg0->vel.x += spC.x * var_f31;
    arg0->vel.y += spC.y * var_f31;
    arg0->vel.z += spC.z * var_f31;
    arg0->vel.x *= 0.95;
    arg0->vel.y *= 0.95;
    arg0->vel.z *= 0.95;
    arg0->pos.x += arg0->vel.x;
    arg0->pos.y += arg0->vel.y;
    arg0->pos.z += arg0->vel.z;
}

void effect_bird_kite_draw(struct Effect *arg0)
{
    if (arg0->model != NULL)
    {
        mathutil_mtxA_from_mtx(userWork->matrices[0]);
        mathutil_mtxA_translate(&arg0->pos);
        mathutil_mtxA_rotate_y(arg0->rotY);
        mathutil_mtxA_rotate_x(arg0->rotX);
        mathutil_mtxA_rotate_z(arg0->rotZ);
        nl2ngc_draw_model_sort_translucent_alt2((struct NlModel *)arg0->model);
    }
}

void effect_bird_kite_destroy(struct Effect *arg0)
{
    arg0->model = NULL;
}

void lbl_80057940(struct NlVtxTypeA *arg0)
{
    float temp_f1;
    int var_r0;
    int var_r4;
    int var_r5;

    temp_f1 = mathutil_sqrt(mathutil_sum_of_sq_2(arg0->x, arg0->z));
    temp_f1 = ((temp_f1 - 750.0) / 3250.0) * 0.5;

    var_r0 = 256.0 * temp_f1;
    if (var_r0 < 0)
        var_r0 = 0;
    else if (var_r0 > 255)
        var_r0 = 255;

    var_r4 = 272.0 * temp_f1;
    if (var_r4 < 0)
        var_r4 = 0;
    else if (var_r4 > 255)
        var_r4 = 255;

    var_r5 = 288.0 * temp_f1;
    if (var_r5 < 0)
        var_r5 = 0;
    else if (var_r5 > 255)
        var_r5 = 255;

    arg0->unk14 = (var_r0 << 16) | (var_r4 << 8) | var_r5;
}
