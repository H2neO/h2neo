#include "filter.h"

float calcSlope(int initialPt, int finalPt, float timeBase) {
    return (finalPt - initialPt) / timeBase;
}

void derivativeFilter(int *prevPtr, int *currPtr, bool *peakFlagPtr, bool *dropFlagPtr, int slopeThreshold, float timeBase) {
    float slope = 0.0;
    if (*prevPtr != -1) {
        slope = calcSlope(*prevPtr, *currPtr, timeBase);
        if (slope < slopeThreshold) {
            *peakFlagPtr = 1;
        } else if (*peakFlagPtr && (slope > slopeThreshold)) {
            *dropFlagPtr = 1;
            *peakFlagPtr = 0;
        }
    }
    *prevPtr = *currPtr;
}
