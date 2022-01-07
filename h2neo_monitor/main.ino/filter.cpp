#include "filter.h"

void swap(float *p, float *q){
  float t;
  
  t = *p;
  *p = *q;
  *q = t;
}

float calcMedian(float *dataPtr, int len){
  float tempData[len];
  float median;
  int i,j,temp;

  memcpy(tempData, dataPtr, sizeof(float)*len);
  
  for(i = 0; i < len-1; i++){
    for(j = 0; j < len-i-1; j++){
      if(tempData[j] > tempData[j+1]){
        swap(&tempData[j], &tempData[j+1]);  
      }  
    }  
  }

  return tempData[len/2 - 1];
}

// The actual drop detection calculations
void thresholding(int index, float *inSignalPtr, float *medFilterPtr, float threshold, int *triggerPtr, bool *dropFlagPtr) {

    if ((fabsf(*(inSignalPtr + index) - *(medFilterPtr + index - 1)) > threshold) && (*dropFlagPtr == 0)) {
        // If the different between input and average is greater than a threshold value, toggle
        if (*(inSignalPtr + index) < *(medFilterPtr + index - 1)) {
           *triggerPtr = 1;
        }

    } else {
        if(*triggerPtr == 1){
            *dropFlagPtr = 1; // dropFLG triggers when incrementing # of peaks
        }
        *triggerPtr = 0;
    }
        
    *(medFilterPtr + index) = calcMedian(inSignalPtr, index+1);
}
