#include "updateFlowRate.h"
#define GTT_FACTOR 20
#define MEMSIZE 10

void updateFlowRate(unsigned long *ticMemPtr, int dropIndex, float *flowRatePtr) {
    
    // Initialize temp variables
    unsigned long sum = 0;
    double avgTimeMS = 0;
    int i = 0;

    // Sum all values inside ticMem
    for (i = 0; i < dropIndex; i++) {

        sum += *(ticMemPtr + i);
    }

    // Calculate average and determine flow rate given preset constants 
    avgTimeMS =  (double) sum / dropIndex; // calculates the average drop time
    *flowRatePtr = 3600000.0 / ((double) GTT_FACTOR * avgTimeMS);
}
