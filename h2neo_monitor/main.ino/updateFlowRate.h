
#ifndef updateFlowRate_H_
#define updateFlowRate_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

#define GTT_FACTOR 20

void updateFlowRate(unsigned long *ticMemPtr, int flowrateIndex, float *flowRatePtr);

#endif
