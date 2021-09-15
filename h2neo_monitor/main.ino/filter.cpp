#include "filter.h"

float calcSlope(int initialPt, int finalPt, float timeBase) {
    return (finalPt - initialPt) / timeBase;
}

void derivativeFilter(int *prevPtr, int *currPtr, bool *peakFlagPtr, bool *dropFlagPtr, int slopeThreshold, float timeBase) {
    float slope = 0.0;
    if (*prevPtr != *currPtr) {
        slope = calcSlope(*prevPtr, *currPtr, timeBase);
//        Serial.print("The adcValue is    "); Serial.println(*currPtr);
        Serial.print("The slope is   "); Serial.println(slope);
//        Serial.print("The threshold is   "); Serial.println(slopeThreshold);
        if (slope < slopeThreshold) {
            *peakFlagPtr = 1;
        }
        if (*peakFlagPtr && (slope > slopeThreshold)) {
            *dropFlagPtr = 1;
            *peakFlagPtr = 0;
        }
    }
    *prevPtr = *currPtr;
}

//float calcMean(float *dataPtr, 
