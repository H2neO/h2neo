//#ifdef __cplusplus
// extern "C" {
//#endif

#ifndef filter_H_
#define filter_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>


float calcMedian(float *dataPtr, int len);

void thresholding(int index, float *inSignalPtr, float *medFilterPtr, float threshold, int *triggerPtr, bool *dropFlagPtr);

              
#endif

//#ifdef __cplusplus
//}
//#endif
