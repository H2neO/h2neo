#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <TimerOne.h>

#include <filter.h>
void setup() {
  /****************
    * Function name: setup
    * Function inputs: None
    * Function outputs: None
    * Function description: N/A
    * Author(s): 
    *****************/

    Serial.begin(9600);                   // Sets serial data transmission rate at 9600 bits/s (baud)
    Serial1.begin(9600);
    Serial1.setTimeout(1000);
    
    Timer1.initialize(100E+3);          // Set the timer period to 100E+3 uS (100mS)
//    Timer1.attachInterrupt(timerISR);   // Attach the interrupt service routine (ISR)
    Timer1.start();                     // Start the timer

}

void loop() {
    // put your main code here, to run repeatedly:
    Serial.println("hi");
    delay(1000);
    

}
