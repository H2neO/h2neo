#ifndef filter_H_
#define filter_H_

#include <stdlib.h>
#include <stdbool.h>
#include <Arduino.h>

double calcSlope(int initialPt, int finalPt, double timeBase);

void derivativeFilter(int *prevPtr, int *currPtr, int slope, int *peakFlag, int *dropFlag);

#endif
