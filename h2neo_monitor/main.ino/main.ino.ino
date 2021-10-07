#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <TimerOne.h>

#include "filter.h"
#include "updateFlowRate.h"

int prevTime = 0;
int currTime = 0;

#define MEMSIZE         10                              // size of memory buffer used for flow rate calculations
#define GTT_FACTOR      20                              // factor specified in tubing packaging (used to calculate # drops/min)
#define GTT_FACTOR_STR  "20"                            // ^ in string format... not sure if it'll work lol
#define SIGNAL_LENGTH   1000                            // 2 * 20ms **Eric: Changed this to 1000ms to limit double drop counting (so max rate we can go to is around 350ml/hr)

#define TRUE 1
#define FALSE 0

// tic - number of times the Timer ISR is entered after x clock cycles
//          tic will be programmed to be 1ms long
// sec - seconds (tic * clock cycles)
// min - minutes (sec / 60)

//unsigned short int dropStopwatch = SIGNAL_LENGTH + 1; // Length of time between each drop used to check if 1 drop has occurred ( > 20ms) value primed to enter if() the first time
unsigned long tic = 0;                                  // (data type short can only go up to 65,535 ms which is only ~1m5sec)
//unsigned short int msec = 0, sec = 0, min = 0;
//unsigned short int oMsec = 0, oSec = 0, oMin = 0;       // old sec; old min
bool dropFlag = 0;                                      // presence of a drop

// save last 5 time interval values and average to find more accurate flow rate
unsigned long ticMem[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};                      // global var auto initialized to 0
unsigned long *ticMemPtr = &(ticMem[0]);
int index = 0;
char str[6];                                            // used to convert each integer to string

float flowRate = 0.0; // mL/hr
float oldFlowRate = 0.0;  // old value used to determine if rate should be printed again

//unsigned char isPrompting = 1;                          // initially set to YES
//unsigned char alarmTriggered = 0;
//unsigned short desiredRate = 0;

// interrupt flags
//char rotKnobIFG = 0;                                    // rotary encoder knob turned
//char rotButIFG = 0;                                     // rotary encoder button pressed
//char s2IFG = 0;                                         // on-board P1.1 (S2) pressed

//short i = 0, yCursor = 1;                               // yCursor = 0 is taken by the stopwatch display

//char refRate[6];                                        // The desired rate but as a string

// Eric's additions
float timeBase = 0.01;
int prevADCValue = 0;
volatile int currADCValue = 0;

//int slopeThreshold = 5000;
// set as volatile because value is being used and updated in different places (one of which is an interrupt)

//float slope = 0;
bool peakFlag = 0;
int ticMemFull = 0;
unsigned long numDrops = 0;

//signal input interrupt using timer1
const uint16_t t1_load = 0; //reset the timer at startup
const uint16_t t1_comp = 10000; //time span to get 2 ms

// EVERYTHING BELOW IS FOR AVERAGING ALGORITHM

float inSignal[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float *inSignalPtr = &(inSignal[0]);

float filteredIn[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float *filteredInPtr = &(filteredIn[0]);

float avgFilter[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float *avgFilterPtr = &(avgFilter[0]);

int timeIndex = 0;
int outSignal[MEMSIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int *outSignalPtr = &(outSignal[0]);
int trigger = 0;
int peaks = 0;
int dropIndex = 0;
float threshold = 13.0;      // update threshold so baseline variability is not inducing error
float influence = 0;
int lag = 2;

int prev = 0;
int curr = 0;

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

    if(dropIndex < MEMSIZE){        // Before the array is filled...
        inSignal[dropIndex] = (float) currADCValue;   // store ADC value into array

        if(dropIndex == lag){             // When lag value is reached, start peak detection
            // Thresholding Init
            memcpy(filteredIn, inSignal, sizeof(float)*MEMSIZE);  // Copy the values of inSignal to filteredIn
            
            avgFilter[lag - 1] = calcMean(inSignal, lag);               // Initial Mean
            thresholding(dropIndex, inSignalPtr, outSignalPtr, filteredInPtr, avgFilterPtr, lag, threshold, influence, &trigger, &peaks, &dropFlag);
        }else if (dropIndex > lag){
            thresholding(dropIndex, inSignalPtr, outSignalPtr, filteredInPtr, avgFilterPtr, lag, threshold, influence, &trigger, &peaks, &dropFlag);
        }

        dropIndex++;
    }else{ // When array is full, the new values are getting added to the end and the array is getting shifted, with the first value getting deleted.
        memmove(&inSignal[0], &inSignal[1], sizeof(inSignal) - sizeof(*inSignal));  //Shift function (WORKS)
        inSignal[dropIndex - 1] = currADCValue;
        memmove(&filteredIn[0], &filteredIn[1], sizeof(filteredIn) - sizeof(*filteredIn));
        memmove(&avgFilter[0], &avgFilter[1], sizeof(avgFilter) - sizeof(*avgFilter));
        thresholding(dropIndex - 1, inSignalPtr, outSignalPtr, filteredInPtr, avgFilterPtr, lag, threshold, influence, &trigger, &peaks, &dropFlag);
    }

    curr = millis();

    if (dropFlag == TRUE && (curr - prev) > 40) {

        if (timeIndex > MEMSIZE-1) {  
            timeIndex = 0;          
        }
        
        ticMem[timeIndex] = curr - prev;
        updateFlowRate(ticMemPtr, dropIndex, &flowRate);
        
        Serial.print(ticMem[timeIndex]); Serial.print(" "); Serial.println(flowRate);

        timeIndex++;
                
        prev = curr;
        dropFlag = FALSE;
    }
}

//    Serial.print("["); 
//        Serial.print(inSignal[0]);
//    Serial.print(", ");
//        Serial.print(inSignal[1]);
//    Serial.print(", ");
//        Serial.print(inSignal[2]);
//    Serial.print(", ");
//        Serial.print(inSignal[3]);
//    Serial.print(", ");
//        Serial.print(inSignal[4]);
//    Serial.print(", ");
//        Serial.print(inSignal[5]);
//    Serial.print(", ");
//        Serial.print(inSignal[6]);
//    Serial.print(", ");
//        Serial.print(inSignal[7]);
//    Serial.print(", ");
//        Serial.print(inSignal[8]);
//    Serial.print(", ");
//        Serial.print(inSignal[9]);
//    Serial.println("]");
    
