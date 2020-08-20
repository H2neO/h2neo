// Written by Jenny Cho
// Modified from PDC8544.h by Rob

// The functions defined here are written to be used for the Nokia5110 LCD screen,
// interfaced with MSP430F5529 Launchpad.

#include "nokia5110.h"

/*******************************************************************************
 * Initialize Clock to 1MHz
 ******************************************************************************/
// ACLK = REFO = 32kHz, MCLK = SMCLK = 12MHz
void Clock_Init_1MHz()
{
	P1DIR |= BIT0;							// P1.0 ACLK set out to pins
	P1SEL |= BIT0;							//    (no longer LED)
//	P2DIR |= BIT2;							// P2.2 SMCLK set out to pins
//	P2SEL |= BIT2;
	P7DIR |= BIT7;							// P7.7 MCLK set out to pins
	P7SEL |= BIT7;

	UCSCTL3 |= SELREF_2;					// Set DCO FLL reference = REFOCLK
	UCSCTL4 |= SELA_2;						// Set ACLK = REFOCLK

	__bis_SR_register(SCG0);				// Disable the FLL control loop
	UCSCTL0 = 0x0000;						// Set lowest possible DCOx, MODx
	UCSCTL1 = DCORSEL_3;					// Select DCO range 1MHz operation
	UCSCTL2 = FLLD_1 + 30;					// Set DCO Multiplier for 12MHz
											// (N + 1) * FLLRef = Fdco
											// (30 + 1) * 32768 = 1MHz
											// Set FLL Div = fDCOCLK/2
	__bic_SR_register(SCG0);				// Enable the FLL control loop

	// Worst-case settling time for the DCO when the DCO range bits have been
	// changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
	// UG for optimization.
	// 32 x 32 x 1 MHz / 32,768 Hz = 31250 = MCLK cycles for DCO to settle
	__delay_cycles(31250);

	// Loop until XT1,XT2 & DCO fault flag is cleared
	do
	{
	UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
											// Clear XT2,XT1,DCO fault flags
	SFRIFG1 &= ~OFIFG;                      // Clear fault flags
	}while (SFRIFG1&OFIFG);                 // Test oscillator fault flag
}


/*******************************************************************************
 * VCC -- 3.3V power
 * GND -- ground pin
 * SCE -- chip select/enable, active low  (CE)
 * RST -- can reset display, active low
 * D/C -- tells if received data == command or display data
 * DN  -- serial data pin for SPI
 * SCLK - clock pin for SPI
 * LED -- controls backlight LED, PWM can control brightness
 ******************************************************************************/
void SPI_Init()
{
    // SLAU396F p.19
    P2OUT |= LCD_SCE_PIN;   				// high (chip enable)
    P2DIR |= LCD_SCE_PIN;     				// set as output

    P3OUT |= LCD_DC_PIN;
    P3DIR |= LCD_DC_PIN;

    // PxSEL-- 1: peripheral module function selected
    P3SEL |= LCD_SCLK_PIN + LCD_DN_PIN;

    // SLAU411F p.12/21
    // 3-pin, 8-bit SPI master
	UCB0CTL1 |= UCSWRST;   					// USCI sofware reset enable

	UCB0CTL0 |= UCMST;     					// Master mode
	UCB0CTL0 |= UCSYNC;    					// 1: SPI-mode
	UCB0CTL0 |= UCMODE_0;  					// sync. mode USCI mode 0  <-- may need to change
	UCB0CTL0 |= UCMSB;     					// 1: MSB first
	UCB0CTL0 |= UCCKPH;   					// data captured on RISE, changed on FALL
	UCB0CTL1 |= UCSSEL_2;  					// USCI 0 clk source 2 (SMCLK)

	// SLAU411F p.14/22
	// (BR0 + BR1*256) prescaler
	// f_bitClock = f_BRCLK / UCBRx
	// f_bitClock = f_BRCLK?? (bc UCBRx = 1?)
	UCB0BR0 = 0x01;      					// baud rate (lower bits)
	UCB0BR1 = 0x00;		   					// baud rate (higher bits)

    UCB0CTL1 &= ~UCSWRST; 					// USCI module enabled (see SLAU411F p.12)
}


/*******************************************************************************
 * Initialize NOKIA5110 LCD Screen to normal display mode
 ******************************************************************************/
void LCD_Init()
{
	P3DIR |= LCD_RST_PIN;
	P3OUT |= LCD_RST_PIN;
	P8DIR |= LCD_VCC_PIN;
	P8OUT |= LCD_VCC_PIN;

	// reset controller
	P3OUT &= ~LCD_RST_PIN;					// reset -> low
	_delay_cycles(200);						// reset pulse length
	P3OUT |= LCD_RST_PIN;					// reset -> high
	_delay_cycles(20000);					// wait for slave init

	// H = 1
	sendByte(LCD_COMMAND, FUNCTION_SET | EXTENDED_INSTRUCTION_SET);
	sendByte(LCD_COMMAND, SET_BIAS | 0x03);
	sendByte(LCD_COMMAND, SET_VOP | 0x3F);
	sendByte(LCD_COMMAND, SET_TEMP | 0x02);
	// H = 0
	sendByte(LCD_COMMAND, FUNCTION_SET);
	sendByte(LCD_COMMAND, DISP_CONTROL | DISP_NORMAL);
}


/******************************************************************************
 * INPUT
 *   isData - data or command
 *   byte   - data to be sent to LCD
 *
 *   1. select chip (active low)
 *   1.5. set dAta or cOmmand
 *   2. check TXIFG flag (1 if ready to be filled)
 *   3. send byte
 *   4. receive byte
 *   5. unselect chip
 ******************************************************************************/
// "writeToLCD()"
char sendByte(unsigned char isData, unsigned char byte)
{
	LCD_SELECT;  // send stuff when SCE low (see p.13 of Nokia5110 datasheet)
	if(isData) {
		LCD_SET_DATA;
	} else {
		LCD_SET_COMMAND;
	}

	while(!(UCB0IFG & UCTXIFG));		// 1 when buffer empty
	UCB0TXBUF = byte;  					// hold tx data in buffer
	while(!(UCB0IFG & UCRXIFG));
	char received = UCB0RXBUF;
	LCD_DESELECT;  						// done sending stuff
	return received;
}

/*******************************************************************************
 * Positions cursor to given coordinates (x, y)
 * (0,0)-----------------------
 *  |							|
 *  |							|
 * 	L------------------------(83, 5)
 ******************************************************************************/
void setCursor(unsigned char xAddr, unsigned char yAddr)
{
	sendByte(LCD_COMMAND, SET_X_ADDR | xAddr);
	sendByte(LCD_COMMAND, SET_Y_ADDR | yAddr);
}

/*******************************************************************************
 * What is "bank"? like a fat row of pixels?
 ******************************************************************************/
void clearBank(unsigned char bank)
{
	setCursor(0, bank);
	int c = 0;
	while(c < X_PIXELS) {
		sendByte(LCD_DATA, 0);
		c++;
	}
	setCursor(0, bank);
}

/*******************************************************************************
 * clear the whole screen
 ******************************************************************************/
void clearLCD()
{
	setCursor(0, 0);
	int c = 0;
	while(c < MAX_BYTES) {
		sendByte(LCD_DATA, 0);
		c++;
	}
	setCursor(0, 0);
}

/*******************************************************************************
 * Print string *str on LCD
 ******************************************************************************/
void prints(const char *str)
{
	while(*str) {
		printc(*str++);
	}
}

/*******************************************************************************
 * Print char c on LCD
 ******************************************************************************/
void printc(char c)
{
	unsigned char i;
	for(i = 0; i < 5; i++) {
		sendByte(LCD_DATA, symbols_table[c - SPACE_CHAR][i]);
	}
	sendByte(LCD_DATA, 0);
}
