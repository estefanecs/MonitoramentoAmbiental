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

float medidaLuminosidade();
float medidaPressao();

int main(void) {

	if(openI2CBus("/dev/i2c-1") == -1)
	{
		return EXIT_FAILURE;
	}
	setI2CSlave(0x48);
	while(1)
	{
		
		printf("CH_0 = %.2f V | ", medidaLuminosidade());

		printf("CH_3 = %.2f V \n",medidaPressao());
	}

	return EXIT_SUCCESS;
}
float medidaLuminosidade(){
	float volts = readVoltage(0); //Potenciometro do canal 0
	float luminosidade;
	luminosidade = (65535 * volts)/3.3; //65535LUX é o valor máximo lido pelo sensor simulado
	return luminosidade;
}

float medidaPressao(){
	float volts = readVoltage(3); //Potenciometro do canal 3
	float pressao;
	pressao = (1100 * volts)/3.3; //1100pHa é o valor máximo lido pelo sensor simulado
	return pressao;
	
}
