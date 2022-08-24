#include "filter.h"

// Swaps the values of p and q
void swap(float *p, float *q){
  float t;
  
  t = *p;
  *p = *q;
  *q = t;
}

// Calculate and returns median of float array of given len
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

    // Initialize trigger flag if pass filter
    // Supposedly accounts for changing baseline as output voltage levels decrease with battery, but not tested **TODO**
    if ((*(inSignalPtr + index) < *(medFilterPtr)*(1-threshold)) && (*dropFlagPtr == 0)) {
           *triggerPtr = 1;
    } else {

        // If the trigger flag is tripped, then set drop flag
        // Trigger flag exists separately from drop flag to avoid multiple drops being detected within the same drop
        if(*triggerPtr == 1){
            *dropFlagPtr = 1; // dropFLG triggers when incrementing # of peaks
        }
        *triggerPtr = 0;
    }

    // Update median filter with the calculated median from last index + 1 ADC values
    *(medFilterPtr + index) = calcMedian(inSignalPtr, index+1);
}
