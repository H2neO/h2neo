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
unsigned long ticMem[MEMSIZE] = {0, 0, 0, 0, 0};                      // global var auto initialized to 0
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
int prevADCValue = -1;
volatile int currADCValue = -1;
                             
int slopeThreshold = 5000;
// set as volatile because value is being used and updated in different places (one of which is an interrupt)

//float slope = 0;
bool peakFlag = 0;
int ticMemFull = 0;
unsigned long numDrops = 0;

//signal input interrupt using timer1 
const uint16_t t1_load = 0; //reset the timer at startup
const uint16_t t1_comp = 12500; //time span to get 50ms
void setup() {
  /****************
    * Function name: setup
    * Function inputs: None
    * Function outputs: None
    * Function description: N/A
    * Author(s): 
    *****************/
    pinMode(A0,INPUT);
    //interrupt code
    TCCR1A = 0; //reset Timer1 Control Reg A

    // Set CTC mode
    TCCR1B &= ~(1 << WGM13);
    TCCR1B |= (1 << WGM12);

    // Set to prescaler of 64
    TCCR1B &= ~(1 << CS12);
    TCCR1B |= (1 << CS11);
    TCCR1B %= ~(1 << CS10);

    //Reset Timer1 and set compare value
    TCNT1 = t1_load;
    OCR1A = t1_comp;

    //Enable Timer1 compare interrupt
    TIMSK1 = (1 << OCIE1A);

    //Enable global interrupts
    sei();
    
    Serial.begin(9600);                   // Sets serial data transmission rate at 9600 bits/s (baud)
//    Serial1.begin(9600);
//    Serial1.setTimeout(1000);
    
//    Timer1.initialize(100E+3);          // Set the timer period to 100E+3 uS (100mS)
//    Timer1.attachInterrupt(timerISR);   // Attach the interrupt service routine (ISR)
//    Timer1.start();                     // Start the timer
    
}

// i created a change!
void loop() {
    // put your main code here, to run repeatedly:
//    currTime = millis();
    delay(1000);
//    
//    prevTime = currTime;
    
    // detect drop every time

    // calculate the slope and the void updateFlowRate(unsigned long *ticMemPtr, int numDrops, float *flowRatePtr)

    derivativeFilter(&prevADCValue, &currADCValue, &peakFlag, &dropFlag, slopeThreshold, timeBase);
    if (numDrops != 0) {
        updateFlowRate(ticMemPtr, numDrops, &flowRate);
//    flowRate = 0;
    }
    Serial.println(flowRate);
    // if flow rate has not changed, then do NOT update LCD and call function

    oldFlowRate = flowRate;
}

ISR(TIMER1_COMPA_vect){
  Serial.println(analogRead(A0));
}
