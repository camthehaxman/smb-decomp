#include <dolphin.h>

#include "global.h"
#include "spline.h"

float calc_spline(float t, struct Spline *arg1)
{
    float temp_f10;
    float temp_f11;
    float temp_f12;
    float temp_f13;
    float ret;
    struct Spline *phi_r6;

    if (t < arg1->unk0)
        return arg1->unk4;

    phi_r6 = arg1 + 1;
    while (t > phi_r6->unk0)
    {
		if (phi_r6->unk0 == 0xFFFF)
		{
			phi_r6--;
			break;
		}
		phi_r6++;
	}

    temp_f10 = phi_r6->unk0 - phi_r6[-1].unk0;
    temp_f11 = (t - phi_r6[-1].unk0) / temp_f10;
    temp_f12 = temp_f11 * temp_f11;
    temp_f13 = temp_f12 * temp_f11;
    ret = phi_r6[-1].unk4 * (2.0 * temp_f13 - 3.0 * temp_f12 + 1.0)
        + phi_r6->unk4 * (-2.0 * temp_f13 + 3.0 * temp_f12);
    ret += temp_f10 * (phi_r6[-1].unkC * (temp_f13 - 2.0 * temp_f12 + temp_f11) + phi_r6->unk8 * (temp_f13 - temp_f12));
    return ret;
}
