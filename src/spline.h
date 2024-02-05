#ifndef _SRC_SPLINE_H_
#define _SRC_SPLINE_H_

struct Spline
{
    s32 unk0;
    float unk4;
    float unk8;
    float unkC;
};

float calc_spline(float t, struct Spline *arg1);

#endif
