#include "updateFlowRate.h"
#define GTT_FACTOR 20
#define MEMSIZE 10
void updateFlowRate(unsigned long *ticMemPtr, int dropIndex, float *flowRatePtr) {
    unsigned long sum = 0, avgTimeMS = 0, totalDrops = 0;
    int i = 0;

    for (i = 0; i < dropIndex; i++) {
//        Serial.print("The value is  "); Serial.println(*(ticMemPtr + i));
//        if (*(ticMemPtr + i) != 0) {
//            
//        }
        sum += *(ticMemPtr + i);
    }

//    Serial.print("The sum is  "); Serial.println(sum);
    avgTimeMS = (float) sum / dropIndex; // calculates the average drop time
    Serial.print("The average time is   "); Serial.println(avgTimeMS);
    *flowRatePtr = 3600000.0 / ((float) GTT_FACTOR * avgTimeMS);

//    Serial.print("The flow rate is    "); Serial.println(*flowRatePtr);
}
