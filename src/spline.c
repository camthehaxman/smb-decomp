#include "spline.h"

// Calculates the cubic spline interpolation at a given point.
//
// This function computes the interpolated value of a cubic spline at a specific
// point t. It assumes the spline is defined by a series of segments, each represented
// by a struct Spline. The spline is smooth and continuous, with each segment contributing
// to the overall shape based on its control parameters.
//
// Parameters:
//   - t: The x-coordinate at which to evaluate the spline.
//   - splineSegments: Pointer to the first segment in the array of spline segments.
//
// Returns:
//   The interpolated y-value of the spline at x-coordinate t.
//
// Notes:
//   Each Spline struct is expected to have the following fields:
//   - start_x: The x-coordinate at the start of the segment.
//   - start_y: The y-value or control parameter associated with start_x.
//   - end_tangent: Tangent or derivative information at the end of the segment.
//   - start_tangent: Tangent or derivative information at the start of the segment.
//   - A segment where start_x is 0xFFFF marks the end of the spline data.
//
// The function handles edge cases where t is outside the range of the defined spline
// and ensures smooth transitions between segments using cubic interpolation.
float calc_spline(float x, struct Spline *splineSegments)
{
    float deltaX;
    float normalizedX;
    float squaredNormalizedX;
    float cubedNormalizedX;
    float interpolatedY;
    struct Spline *currentSegment;

    // Edge case: x before the start of the spline
    if (x < splineSegments->start_x)
        return splineSegments->start_y;

    currentSegment = splineSegments + 1;
    while (x > currentSegment->start_x)
    {
        if (currentSegment->start_x == 0xFFFF)
        {
            currentSegment--;
            break;
        }
        currentSegment++;
    }

    deltaX = currentSegment->start_x - (currentSegment - 1)->start_x;
    normalizedX = (x - (currentSegment - 1)->start_x) / deltaX;
    squaredNormalizedX = normalizedX * normalizedX;
    cubedNormalizedX = squaredNormalizedX * normalizedX;
    interpolatedY = (currentSegment - 1)->start_y * (2.0 * cubedNormalizedX - 3.0 * squaredNormalizedX + 1.0)
        + currentSegment->start_y * (-2.0 * cubedNormalizedX + 3.0 * squaredNormalizedX);
    interpolatedY += deltaX * ((currentSegment - 1)->start_tangent * (cubedNormalizedX - 2.0 * squaredNormalizedX + normalizedX) 
        + currentSegment->end_tangent * (cubedNormalizedX - squaredNormalizedX));
    return interpolatedY;
}
