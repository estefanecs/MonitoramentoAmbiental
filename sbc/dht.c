#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "ads1115_rpi.h"

#define MAXTIMINGS	85
#define DHTPIN		0 // Usa a GPIO 17 se no WiringPiSetup
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
 
void read_dht11_dat()
{
	uint8_t laststate	= HIGH;
	uint8_t counter		= 0;
	uint8_t j		= 0, i;
	float	f; 
 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	wiringPiSetup(); // Usa WiringPI
	//wiringPiSetupGpio(void); // Usa BCM
	//wiringPiSetupPhys(void); // Usa Physical

	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 20 );
	//printf("\nDelay de 20ms feito.\n");
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 50 );
	//printf("Delay de 50us feito.\n");
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ )
	{
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate )
		{
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 )
			{
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) )
		{
			dht11_dat[j / 8] <<= 1;
			if ( counter > 16 ) // antes era 16
				dht11_dat[j / 8] |= 1;
			j++;
		}
	}
 
	if ( (j >= 40) &&
	     (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
	{
		f = dht11_dat[2] * 9. / 5. + 32;
		printf( "Umidade = %d.%d %% Temperatura = %d.%d° C (%.1f° F)\n",
			dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3], f );
		delay( 1000 ); 

	}else  {
		//printf( "Data not good, skip\n" );
	}
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

int main( void )
{
	if ( wiringPiSetup() == -1 )
		exit( 1 );
 	if(openI2CBus("/dev/i2c-1") == -1){
		return EXIT_FAILURE;
	}
	setI2CSlave(0x48);
	while ( 1 )
	{
		read_dht11_dat(); //Ler o sensor DHT11
		printf("CH_0 = %.2f V | ", medidaLuminosidade()); //Ler o sensor de Luminosidade
		printf("CH_3 = %.2f V \n",medidaPressao()); //Ler o sensor de Pressao atmosferica
	}
 
	return(0);
}
