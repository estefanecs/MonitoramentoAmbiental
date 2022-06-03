/*
 ============================================================================
 Name        : ads1115_example.c
 Author      : Giovanni Bauermeister
 Description : Read analog values from potentiometer using ADS1115 and prints to terminal
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ads1115_rpi.h"

int main(void) {

	if(openI2CBus("/dev/i2c-1") == -1)
	{
		return EXIT_FAILURE;
	}
	setI2CSlave(0x48);
	while(1)
	{
		printf("CH_0 = %.2f V | ", readVoltage(0));	// primeiro potenciometro
		printf("CH_3 = %.2f V \n", readVoltage(3));	// segundo potenciometro
	}

	return EXIT_SUCCESS;
}
