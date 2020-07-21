// Written by Jenny Cho
// Modified in May 11th, 2020

// Project h2neo

// The following project is for an electric flow rate monitor for gravity-assisted
// IV therapy equipment. A LCD screen is integrated into a MSP430F5529 Launchpad,
// interfaced via SPI communication. Other user interface features a rotary encoder
// that is used to adjust and input setting by the user.
// The flow rate sensing is done using an optical system consisting of an infrared
// LED and a photodiode.

#include <msp430f5529.h>
#include "nokia5110.h"
#include "rotary_encoder.h"
#include "test.h"

#define   Num_of_Results   8
volatile unsigned int results[Num_of_Results];


unsigned short ps;		// Grab BIT4 and BIT5
unsigned short ns;
float value = 0;

// interrupt flags
char rotKnobIFG = 0;	// rotary encoder knob turned
char rotButIFG = 0;		// rotary encoder button pressed
char s2IFG = 0;			// on-board P1.1 (S2) pressed

/********************************************************************************
 * main.c
 ********************************************************************************/
int main(void) {
	WDTCTL = WDTPW + WDTHOLD;				// stop watchdog timer
	Clock_Init_1MHz();
	SPI_Init();
	_delay_cycles(500000);

	LCD_Init();
	RotEnc_Init();
	clearLCD();

	setCursor(0, 0);

//	test_adc0_init();

//	ADC12CTL0 |= ADC12SC;                     // Start conversion

//	static unsigned char in = 0;



//	__bis_SR_register(GIE);					// enable interrupts
	__bis_SR_register(LPM4_bits + GIE);       // Enter LPM4, Enable interrupts (ADC example)

	__no_operation();						// For debugger

//	char str[5];
	while (1) {
		if (s2IFG) {
			prints("hello! ");
			s2IFG = 0;
		}
		if (rotKnobIFG) {
			prints("turned ");
			get_direction();
			rotKnobIFG = 0;
		}
		if (rotButIFG) {
			prints("button. ");
			rotButIFG = 0;
		}
//		test_adc0();
		// display value

//		intToStr(results[in++], str, 0);
//		prints(str);
//		if (in == 8)
//		{
//		  in = 0;
//		}


//		ps = (P1IN>>4) & 0x3;

//		display_value(value);
//		prints("hello");
//		_delay_cycles(200000);
	}
}
