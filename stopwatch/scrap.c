/*
 * scrap.c
 *
 * This file contains all the unorganized methods. These should be eventually
 * moved to source files with descriptive names (just copy paste).
 *
 *  Created on: Jul 24, 2020
 *      Author: Jenny Cho
 */
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "scrap.h"
#include "nokia5110.h"

extern unsigned long int tic;  // data type short can only go up to 65,535 ms which is only ~1m5sec
//extern unsigned short int msec, sec, min; //Example of Global variables.
extern unsigned char butFLG;  // button flag


///*******************************************************************************
// * Initialize Clock to 1MHz
// ******************************************************************************/
//void Clock_Init_1MHz(void)
//{
//	P1DIR |= BIT0;							// P1.0 ACLK set out to pins
//	P1SEL |= BIT0;							//    (no longer LED)
////	P2DIR |= BIT2;							// P2.2 SMCLK set out to pins
////	P2SEL |= BIT2;
//	P7DIR |= BIT7;							// P7.7 MCLK set out to pins
//	P7SEL |= BIT7;
//
//	UCSCTL3 |= SELREF_2;					// Set DCO FLL reference = REFOCLK
//	UCSCTL4 |= SELA_2;						// Set ACLK = REFOCLK
//
//	__bis_SR_register(SCG0);				// Disable the FLL control loop
//	UCSCTL0 = 0x0000;						// Set lowest possible DCOx, MODx
//	UCSCTL1 = DCORSEL_3;					// Select DCO range 1MHz operation
//	UCSCTL2 = FLLD_1 + 30;					// Set DCO Multiplier for 12MHz
//											// (N + 1) * FLLRef = Fdco
//											// (30 + 1) * 32768 = 1MHz
//											// Set FLL Div = fDCOCLK/2
//	__bic_SR_register(SCG0);				// Enable the FLL control loop
//
//	// Worst-case settling time for the DCO when the DCO range bits have been
//	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
//	// UG for optimization.
//	// 32 x 32 x 1 MHz / 32,768 Hz = 31250 = MCLK cycles for DCO to settle
//	__delay_cycles(31250);
//
//	// Loop until XT1,XT2 & DCO fault flag is cleared
//	do
//	{
//	UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
//											// Clear XT2,XT1,DCO fault flags
//	SFRIFG1 &= ~OFIFG;                      // Clear fault flags
//	}while (SFRIFG1&OFIFG);                 // Test oscillator fault flag
//}


/*******************************************************************************
 * Initialize Timer0_A5
 ******************************************************************************/
void Timer0_A5_Init(void)
{
	TA0CCR0 	 =  0; 		//Initially, Stop the Timer
	TA0CTL		 =  TASSEL_2 + ID_0 + MC__UP; //Select SMCLK, SMCLK/1, Up Mode
	TA0CCTL0	|=  CCIE;		//Enable interrupt on TA0.0
//	TA0CCR0		 =  ;		//Period of 50ms or whatever interval you like.
}


/*******************************************************************************
 * Delay msecs milliseconds of time based on 1MHz clock
 ******************************************************************************/
void delayMS(int ms)
{
	tic = 0; //Reset Over-Flow counter
	// in general, Y MHz clock requires Y*1000 ticks for 1ms delay
	TA0CCR0 = 1000 - 1; //Start Timer, Compare value for Up Mode to get 1ms delay per loop
	//Total count = TACCR0 + 1. Hence we need to subtract 1.
	while(tic<=ms);

	TA0CCR0 = 0; //Stop Timer
}

/*******************************************************************************
 * Start Timer0_A5
 ******************************************************************************/
void startTimer0_A5(void)
{
	tic = 0; //Reset Over-Flow counter
	// in general, Y MHz clock requires Y*1000 ticks for 1ms delay
	TA0CCR0 = 1000 - 1;  // start timer; compare value (up mode): 1 ms
	P1OUT |= BIT0;
}


/*******************************************************************************
 * Stop Timer0_A5
 ******************************************************************************/
void stopTimer0_A5(void)
{
	TA0CCR0 = 0; // stop Timer
	P1OUT &= ~BIT0; //Drive P1.0 LOW - LED1 OFF
}

/*******************************************************************************
 * calculate seconds from given tics value
 ******************************************************************************/
void getSec(int tics)
{

}

void int2strXX(int num, char* str)
{
	sprintf(str, "%d", num);
	if (num < 10) {
		char temp[1];
		strcpy(temp, "0");
		strcat(temp, str);
		strcpy(str, temp);
	}
}

// if num is not "long int" the printed result might be negative if num < 65,536
void int2str(int num, char* str)
{
	sprintf(str, "%d", num);
}





void displayFlowRate(float *value, char* buf)
{
	// TODO: display the current value on the display screen

//	gcvt(value, 5, buf);
//	prints(buf);

//	sprintf(buf, ": %d", *value);
//	puts(buf);
//	prints(buf);
//	value = 233.1;
	ftoa(value, buf, 2);

}

// Converts a floating point number to string
// value  --> Input value pointer
// str[]  --> Array for output string
// afterP --> Number of digits to be considered after the point
void ftoa(float *value, char* str, int afterP)
{
	float n = *value;
	// Extract integer part
	int ipart = (int)n;

	// Extract floating part
	float fpart = n - (float)ipart;

	// Convert integer part to string
	int i = intToStr(ipart, str, 0);

	// Check for display option after point
	if (afterP != 0) {
		str[i] = '.';  // add 'point'

		// Get value of fraction part upto given afterP
		fpart = fpart * pow(10, afterP);

		intToStr((int)fpart, str + i + 1, afterP);
	}
}

// Reverses a string 'str' of length 'len'
void reverse(char* str, int len)
{
	int i = 0, j = len - 1, temp;
	while (i < j) {
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}


// YOO THIS MIGHT BE A DUPLICATE OF MY 'int2str()' METHOD ABOVE!!!
// CHANGE LATER WHEN YOU GET EVERYTHING TO WORK
// Converts given x (int) to str[].
// d --> num digits required for output
//       If d is more than the num digits in x, 0s are added at beginning
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while(x) {
		str[i++] = (x % 10) + '0';
		x = x / 10;
	}

	// If num digits required is more, add 0s in beginning
	while (i < d) str[i++] = '0';

	reverse(str, i);
	str[i] = '\0';
	return i;
}










/*******************************************************************************
 * Timer 0 A0 Interrupt Service Routine
 ******************************************************************************/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0_ISR( void )
{
	/* INCREMENT GLOBAL VARIABLES HERE:
	 *
	 * Will jump in here when TA0R reaches the value stored in TA0CCR0 during setup.
	 * Count the number of tics to equal a second, then increment seconds.
	 * Count the number of seconds to increment minutes. You get it.
	 *
	 * Don?t forget to update the LCD Display with the current time here if you
	 * are not doing that in the main loop.
	 */
	tic++;  // increment over-flow counter
}


/*******************************************************************************
 * Timer 0 A0 Interrupt Service Routine
 ******************************************************************************/
#pragma vector=PORT1_VECTOR
__interrupt void Port_1_ISR( void )
{
	if (P1IFG & BIT1) {
		butFLG = 1;
		_delay_cycles(20000);	// debouncing
		P1IFG &= ~BIT1;				// P1.1 interrupt flag cleared
	}
}


