#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <TimerOne.h>

#include "filter.h"
#include "updateFlowRate.h"

#define MEMSIZE         10                              // size of memory buffer used for flow rate calculations
#define GTT_FACTOR      20                              // factor specified in tubing packaging (used to calculate # drops/min)
#define GTT_FACTOR_STR  "20"                            // ^ in string format... not sure if it'll work lol
#define SIGNAL_LENGTH   1000                            // 2 * 20ms **Eric: Changed this to 1000ms to limit double drop counting (so max rate we can go to is around 350ml/hr)

#define TRUE 1
#define FALSE 0


// save last 10 time interval values and average to find more accurate flow rate
unsigned long ticMem[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                       // store last 10 timestamps between drops
unsigned long *ticMemPtr = &(ticMem[0]);

float inSignal[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                             // past 10 ADC values
float *inSignalPtr = &(inSignal[0]);

float medFilter[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                            // median filter sotrage
float *medFilterPtr = &(medFilter[0]);

float flowRate = 0.0; // mL/hr
float timeBase = 0.01;
volatile int currADCValue = 0;

// signal input interrupt using timer1
const uint16_t t1_load = 0; // reset the timer at startup
const uint16_t t1_comp = 10000; // time span to get 5 ms

// Variables for the different data arrays
int adcIndex = 0;
int timeIndex = 0;
int dropIndex = 0;

// Flags for whether a drop has occured (trigger says current drop is being detected and not to accidently register more)
int triggerFlag = 0;
bool dropFlag = 0;

// This is a percentage value (20% decrease from baseline)
float threshold = 0.2; 

// Variables for the ms timestamp for current and previous drop
unsigned long prev = 0;
unsigned long curr = 0;

void setup() {
  pinMode(A0, INPUT);

  // Arduino specific setup to write interrupt code
  TCCR1A = 0; //reset Timer1 Control Reg A

  // Set CTC mode
  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);

  // Set to prescaler of 8
  TCCR1B &= ~(1 << CS12);
  TCCR1B |= (1 << CS11);
  TCCR1B &= ~(1 << CS10);

  //Reset Timer1 and set compare value
  TCNT1 = t1_load;
  OCR1A = t1_comp;

  //Enable Timer1 compare interrupt
  TIMSK1 = (1 << OCIE1A);

  //Enable global interrupts
  sei();

  Serial.begin(9600);                   // Sets serial data transmission rate at 9600 bits/s (baud)
}

void loop() {
    // Main code that is not an interrupt should go here!
}


// Typically do not want this much code in an ISR, fix this in future
ISR(TIMER1_COMPA_vect) {

  // Reads in current value and initializes the timer
  currADCValue = analogRead(A0);
  curr = millis();


  if (!((curr - prev) < 40)) { // Prevents a drop from being recorded if less than 40 ms has passed between
    if (adcIndex < MEMSIZE) {      // Before the array is filled...
      inSignal[adcIndex] = (float) currADCValue;   // store ADC value into array
      adcIndex++;
    } else { // When array is full....
      
      // Array is getting shifted so rightmost 9 elements are moved over one, with the first value getting overwritten
      memmove(&inSignal[0], &inSignal[1], sizeof(inSignal) - sizeof(*inSignal));

      // New ADC value gets read into the last index of the ADC array
      inSignal[adcIndex - 1] = currADCValue;
      
      // Array is getting shifted so rightmost 9 elements are moved over one, with the first value getting overwritten
      memmove(&medFilter[0], &medFilter[1], sizeof(medFilter) - sizeof(*medFilter));

      // Determines whether or not the ADC value should register as a drop and sets appropriate flags
      thresholding(adcIndex - 1, inSignalPtr, medFilterPtr, threshold, &triggerFlag, &dropFlag);
    }
  }

  // If a drop has occured,
  if (dropFlag == TRUE) {

    // If the end of the ticMem array is reached, then start begin at the beginning
    if (timeIndex > MEMSIZE - 1) {
      timeIndex = 0;
    }

    // Increment drop index until it is equivalent to MEMSIZE, then doesn't change
    if (dropIndex < MEMSIZE + 1) {
      dropIndex++;
    }

    // If more than one drop has been detected
    if (dropIndex > 1) {

      // Record time between current and previous drop
      ticMem[timeIndex] = curr - prev;
      updateFlowRate(ticMemPtr, dropIndex - 1, &flowRate);

      // Print out drop time and current flow rate, then increment timeIndex
      Serial.print(ticMem[timeIndex]); Serial.print(" "); Serial.println(flowRate);
      timeIndex++;
    } else {
      Serial.println("First Drop Detected - Calibrating..."); // Indicate to user than drop has been detected, but no flow rate calculated yet
    }

    // Reset flags for the next drop
    prev = curr;
    dropFlag = FALSE;
  }
}
