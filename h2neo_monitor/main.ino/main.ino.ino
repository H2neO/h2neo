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

unsigned long tic = 0;                                  // (data type short can only go up to 65,535 ms which is only ~1m5sec)
bool dropFlag = 0;                                      // presence of a drop

// save last 5 time interval values and average to find more accurate flow rate
unsigned long ticMem[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                      // global var auto initialized to 0
unsigned long *ticMemPtr = &(ticMem[0]);

float flowRate = 0.0; // mL/hr
float oldFlowRate = 0.0;  // old value used to determine if rate should be printed again

float timeBase = 0.01;
int prevADCValue = 0;
volatile int currADCValue = 0;

bool peakFlag = 0;
int ticMemFull = 0;
unsigned long numDrops = 0;

//signal input interrupt using timer1
const uint16_t t1_load = 0; //reset the timer at startup
const uint16_t t1_comp = 10000; //time span to get 2 ms

// EVERYTHING BELOW IS FOR AVERAGING ALGORITHM

float inSignal[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float *inSignalPtr = &(inSignal[0]);

float medFilter[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float *medFilterPtr = &(medFilter[0]);

int index = 0;
int timeIndex = 0;
int trigger = 0;
int dropIndex = 0;
float threshold = 13.0;      // update threshold so baseline variability is not inducing error

unsigned long prev = 0;
unsigned long curr = 0;

void setup() {
  /****************
      Function name: setup
      Function inputs: None
      Function outputs: None
      Function description: N/A
      Author(s):
    *****************/
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
    //curr = millis();
////    delay(1000);
////  
//   Serial.print("The time that the stuff took  ->  "); Serial.println(curr - prev);  
//    prevTime = currTime;

    // detect drop every time
    //noInterrupts();

   
    //interrupts();

//   curr = millis();
////    delay(1000);
////  
//   Serial.print("The time that the stuff took  ->  "); Serial.println(curr - prev);     
}

ISR(TIMER1_COMPA_vect){
  currADCValue = analogRead(A0);
//  int time1 = millis();
//  Serial.print("The time is   ");
//  Serial.println(time1);
//  Serial.println(currADCValue);

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


    
