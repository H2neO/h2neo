#include "filter.h"

double calcSlope(int initialPt, int finalPt, double timeBase) {
    return (finalPtr - initialPt) / timeBase;
}

void derivativeFilter(int *prevPtr, int *currPtr, int *slopePtr, int *peakFlagPtr, int *dropFlagPtr, int slopeThreshold) {
    if (*prevPtr != -1) {
        *slopePtr = calcSlope(*prevPtr, *currPtr);
        if (*slopePtr < slopeThreshold) {
            *peakFlagPtr = 1;
        } else if (*peakFlagPtr && (*slope > slopeThreshold) {
            *dropFlagPtr = 1;
            *peakFlagPtr = 0;
        }
}
