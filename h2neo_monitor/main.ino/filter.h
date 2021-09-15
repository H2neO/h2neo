//#ifdef __cplusplus
// extern "C" {
//#endif

#ifndef filter_H_
#define filter_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

float calcSlope(int initialPt, int finalPt, float timeBase);

void derivativeFilter(int *prevPtr, int *currPtr, bool *peakFlagPtr, bool *dropFlagPtr, int slopeThreshold, float timeBase);

#endif

//#ifdef __cplusplus
//}
//#endif
