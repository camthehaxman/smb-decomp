#ifndef _SRC_SPLINE_H_
#define _SRC_SPLINE_H_

struct Spline
{
    long int start_x;
    float start_y;
    float end_tangent;
    float start_tangent;
};

float calc_spline(float x, struct Spline *splineSegments);

#endif
