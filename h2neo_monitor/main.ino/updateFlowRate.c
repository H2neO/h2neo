#define GTT_FACTOR 20

void updateFlowRate(unsigned long *ticMemPtr, unsigned long numDrops, float *flowRatePtr) {
    unsigned long sum, avgTimeMS = 0;
    int i;

    for (i = 0; i <= numDrops; i++) {
        sum += *(ticMemPtr + i);
    }
    avgTimeMS = (float) sum / numDrops; // calculates the average drop time
    *flowRatePtr = 3600000.0 / ((float) GTT_FACTOR * avgTimeMS);
}
