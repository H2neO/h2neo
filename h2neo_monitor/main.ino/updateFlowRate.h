#ifdef __cplusplus
 extern "C" {
#endif

#ifndef updateFlowRate_H_
#define updateFlowRate_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define GTT_FACTOR 20

void updateFlowRate(unsigned long *ticMemPtr, unsigned long numDrops, float *flowRatePtr);

#endif

#ifdef __cplusplus
}
#endif
