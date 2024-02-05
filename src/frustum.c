#include <dolphin.h>

#include "global.h"
#include "mathutil.h"

static Vec clip;
static Vec lbl_801EFC24[4];
static Vec lnormal[4];

FORCE_BSS_ORDER(clip)
FORCE_BSS_ORDER(lbl_801EFC24)
FORCE_BSS_ORDER(lnormal)

double force_sdata2_802F2F50() { return 182.04444885253906; }
double force_sdata2_802F2F58() { return 0.5; }
float  force_sdata2_802F2F60() { return 182.04444885253906f; }

void clip_init_detail(Vec *a, S16Vec *b, float c, float d, float e, float f)
{
    u8 dummy[8];
    s16 r30;
    s16 r29;
    s16 r26;
    s16 r25;
    s16 temp;
    float f28;
    float f27;
    float sincos[2];

    clip = *a;
    if (f == 0.0f)
    {
        temp = DEGREES_TO_S16(c);
        r30 = -temp / 2;
        r29 = temp / 2;
    }
    else
    {
        f27 = mathutil_tan(DEGREES_TO_S16(c * 0.5f));
        r30 = -mathutil_atan(f27 * (1.0 - f));
        r29 = mathutil_atan(f27 * (1.0 + f));
    }
    if (e == 0.0f)
    {
        temp = mathutil_atan(mathutil_tan(DEGREES_TO_S16(c * 0.5)) * d) * 2;
        r26 = temp / 2;
        r25 = -temp / 2;
    }
    else
    {
        if (f == 0.0f)
            f28 = d * mathutil_tan(DEGREES_TO_S16(c * 0.5f));
        else
            f28 = f27 * d;
        r26 = mathutil_atan(f28 * (1.0 + e));
        r25 = -mathutil_atan(f28 * (1.0 - e));
    }

    mathutil_mtxA_from_translate(a);
    mathutil_mtxA_rotate_y(b->y);
    mathutil_mtxA_rotate_x(b->x);
    mathutil_mtxA_rotate_z(b->z);

    mathutil_sin_cos_v(r26 - 0x4000, sincos);
    lnormal[0].x = -sincos[0];
    lnormal[0].y = 0.0f;
    lnormal[0].z = -sincos[1];
    mathutil_mtxA_tf_vec(&lnormal[0], &lbl_801EFC24[0]);

    mathutil_sin_cos_v(r25 + 0x4000, sincos);
    lnormal[1].x = -sincos[0];
    lnormal[1].y = 0.0f;
    lnormal[1].z = -sincos[1];
    mathutil_mtxA_tf_vec(&lnormal[1], &lbl_801EFC24[1]);

    mathutil_sin_cos_v(r30 + 0x4000, sincos);
    lnormal[2].x = 0.0f;
    lnormal[2].y = -sincos[0];
    lnormal[2].z = -sincos[1];
    mathutil_mtxA_tf_vec(&lnormal[2], &lbl_801EFC24[2]);

    mathutil_sin_cos_v(r29 - 0x4000, sincos);
    lnormal[3].x = 0.0f;
    lnormal[3].y = -sincos[0];
    lnormal[3].z = -sincos[1];
    mathutil_mtxA_tf_vec(&lnormal[3], &lbl_801EFC24[3]);
}

BOOL clip_z(Vec *a, float b)
{
    Vec v;
    int i;

    v.x = a->x - clip.x;
    v.y = a->y - clip.y;
    v.z = a->z - clip.z;

    for (i = 0; i < 4; i++)
    {
        if (v.x * lbl_801EFC24[i].x + v.y * lbl_801EFC24[i].y + v.z * lbl_801EFC24[i].z < -b)
            return FALSE;
    }
    return TRUE;
}

BOOL test_sphere_in_frustum(Point3d *p, float radius)
{
    Vec sp10;
    float x, y, z;
    int i;

    mathutil_mtxA_tf_point(p, &sp10);
    x = sp10.x;
    y = sp10.y;
    z = sp10.z;
    if (z > radius)
        return FALSE;
    radius = -radius;
    for (i = 0; i < 4; i++)
    {
        if (x * lnormal[i].x + y * lnormal[i].y + z * lnormal[i].z < radius)
            return FALSE;
    }
    return TRUE;
}

BOOL test_scaled_sphere_in_frustum(Point3d *p, float radius, float scale)
{
    Vec sp10;
    float x, y, z;
    int i;

    mathutil_mtxA_tf_point(p, &sp10);
    radius *= scale;
    x = sp10.x;
    y = sp10.y;
    z = sp10.z;
    if (z > radius)
        return FALSE;
    radius = -radius;
    for (i = 0; i < 4; i++)
    {
        if (x * lnormal[i].x + y * lnormal[i].y + z * lnormal[i].z < radius)
            return FALSE;
    }
    return TRUE;
}
