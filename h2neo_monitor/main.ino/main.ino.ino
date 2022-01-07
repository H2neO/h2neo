// H2NEO EMAIL REPO 

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

bool dropFlag = 0;                                      // presence of a drop

// save last 5 time interval values and average to find more accurate flow rate
unsigned long ticMem[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                      // global var auto initialized to 0
unsigned long *ticMemPtr = &(ticMem[0]);

float inSignal[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float *inSignalPtr = &(inSignal[0]);

float medFilter[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float *medFilterPtr = &(medFilter[0]);

float flowRate = 0.0; // mL/hr
float timeBase = 0.01;

//signal input interrupt using timer1
const uint16_t t1_load = 0; //reset the timer at startup
const uint16_t t1_comp = 10000; //time span to get 5 ms

int index = 0;
int timeIndex = 0;
int dropIndex = 0;
int trigger = 0;
float threshold = 13.0;      // This is a percentage value (20% decrease from baseline)

volatile int currADCValue = 0;
volatile unsigned long prev = 0;
volatile unsigned long curr = 0;

void setup() {
  pinMode(A0, INPUT);
  
  //interrupt code
  TCCR1A = 0; //reset Timer1 Control Reg A

  // Set CTC mode
  TCCR1B &= ~(1 << WGM13);
  TCCR1B |= (1 << WGM12);

  // Set to prescaler of 1
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

// i created a change!
void loop() {

}

ISR(TIMER1_COMPA_vect){
    currADCValue = analogRead(A0);

    if(index < MEMSIZE){        // Before the array is filled...
        inSignal[index] = (float) currADCValue;   // store ADC value into array
        index++;
    }
    else{ // When array is full, the new values are getting added to the end and the array is getting shifted, with the first value getting deleted.
        memmove(&inSignal[0], &inSignal[1], sizeof(inSignal) - sizeof(*inSignal));  //Shift function (WORKS)
        inSignal[index - 1] = currADCValue;
        memmove(&medFilter[0], &medFilter[1], sizeof(medFilter) - sizeof(*medFilter));
        thresholding(index - 1, inSignalPtr, medFilterPtr, threshold, &trigger, &dropFlag);
    }
    
    curr = millis();

    if (dropFlag == TRUE && (curr - prev) > 40) {

        if (timeIndex > MEMSIZE-1) {  
            timeIndex = 0;          
        }

        if(dropIndex < MEMSIZE+1){
            dropIndex++;
        }

        if(dropIndex > 1){
            ticMem[timeIndex] = curr - prev;
            updateFlowRate(ticMemPtr, dropIndex-1, &flowRate);
            
            Serial.print(ticMem[timeIndex]); Serial.print(" "); Serial.println(flowRate); 
            timeIndex++;
        }else{
            Serial.println("First Drop Detected - Calibrating..."); 
        }
                
        prev = curr;
        dropFlag = FALSE;
    }
}


    
