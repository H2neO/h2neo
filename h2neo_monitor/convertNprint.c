/*
 * convertNprint.c
 *
 *  Created on: Aug 19, 2020
 *      Author: Jenny Cho
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "convertNprint.h"


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
