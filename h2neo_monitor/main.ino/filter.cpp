#include "filter.h"

//float calcSlope(int initialPt, int finalPt, float timeBase) {
//    return (finalPt - initialPt) / timeBase;
//}
//
//void derivativeFilter(int *prevPtr, int *currPtr, bool *peakFlagPtr, bool *dropFlagPtr, int slopeThreshold, float timeBase) {
//    float slope = 0.0;
//    if (*prevPtr != *currPtr) {
//        slope = calcSlope(*prevPtr, *currPtr, timeBase);
////        Serial.print("The adcValue is    "); Serial.println(*currPtr);
//        Serial.print("The slope is   "); Serial.println(slope);
////        Serial.print("The threshold is   "); Serial.println(slopeThreshold);
//        if (slope < slopeThreshold) {
//            *peakFlagPtr = 1;
//        }
//        if (*peakFlagPtr && (slope > slopeThreshold)) {
//            *dropFlagPtr = 1;
//            *peakFlagPtr = 0;
//        }
//    }
//    *prevPtr = *currPtr;
//}

void swap(float *p, float *q){
  float t;
  
  t = *p;
  *p = *q;
  *q = t;
}

float calcMedian(float *dataPtr, int len){
  float tempDataPtr[len];
  float median;
  int i,j,temp;

  memcpy(tempDataPtr, dataPtr, sizeof(float)*len);
  
  for(i = 0; i < len-1; i++){
    for(j = 0; j < len-i-1; j++){
      if(tempDataPtr[j] > tempDataPtr[j+1]){
        swap(&tempDataPtr[j], &tempDataPtr[j+1]);  
      }  
    }  
  }

  return tempDataPtr[len/2 - 1];
}

// The actual drop detection calculations
void thresholding(int index, float *inSignalPtr, float *avgFilterPtr, float threshold, int *triggerPtr, bool *dropFlagPtr) {

    if ((fabsf(*(inSignalPtr + index) - *(avgFilterPtr + index - 1)) > threshold) && (*dropFlagPtr == 0)) {
        // If the different between input and average is greater than a threshold value, toggle
        if (*(inSignalPtr + index) < *(avgFilterPtr + index - 1)) {
           *triggerPtr = 1;
        }

    } else {
        if(*triggerPtr == 1){
            *dropFlagPtr = 1; // dropFLG triggers when incrementing # of peaks
        }
        *triggerPtr = 0;
    }
        
    *(avgFilterPtr + index) = calcMedian(inSignalPtr, index+1);
     Serial.print(*(inSignalPtr + index)); Serial.print(" "); /*Serial.print(*(filteredInPtr + index)); Serial.print(" ");*/ Serial.println(*(avgFilterPtr + index));
}
