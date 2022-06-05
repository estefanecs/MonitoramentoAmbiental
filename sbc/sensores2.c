#include <wiringPi.h>
#include <string.h>
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

// Declaracao das funcoes 
void leitura_dht11(char *temperatura, char *umidade); //Leitura do DHT11
void medidaLuminosidade(float *luminosidade); //Simulacao do sensor de luminosidade
void medidaPressao(float *pressao); //Simulacao do sensor de pressao atmosferica

int main( void ){
	float pressao;
	float luminosidade;
	char temperatura[10];
	char umidade[10];
	
	if ( wiringPiSetup() == -1 )
		exit( 1 );
 	if(openI2CBus("/dev/i2c-1") == -1){
		return EXIT_FAILURE;
	}
	setI2CSlave(0x48);
	while ( 1 ){
		leitura_dht11(&temperatura,&umidade); //Ler o sensor DHT11
		printf("temperatura= %s ºC| ",temperatura); //imprime o valor da temperatura
		printf("umidade= %s | ",umidade); //imprime o valor da umidade
		
		medidaLuminosidade(&luminosidade);//Ler o sensor de Luminosidade
		printf("luminosidade= %.2f LUX | ",luminosidade); 
		
		medidaPressao(&pressao);//Ler o sensor de Pressao atmosferica
		printf("pressao = %.2f hPa \n",pressao); 
	}
 
	return(0);
}

// Funcao que realiza a leitura do sensor DHT11
void leitura_dht11(char *temperatura, char *umidade){
	char umidade2[10]; //armazena a segunda parte do valor de umidade
	char temperatura2[10]; //armazena a segunda parte do valor de temperatura
	
	uint8_t laststate = HIGH;
	uint8_t counter	= 0;
	uint8_t j = 0, i;
	float f; 
 
	dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

	wiringPiSetup(); 
	pinMode( DHTPIN, OUTPUT );
	digitalWrite( DHTPIN, LOW );
	delay( 20 );
	digitalWrite( DHTPIN, HIGH );
	delayMicroseconds( 50 );
	pinMode( DHTPIN, INPUT );
 
	for ( i = 0; i < MAXTIMINGS; i++ ){
		counter = 0;
		while ( digitalRead( DHTPIN ) == laststate ){
			counter++;
			delayMicroseconds( 1 );
			if ( counter == 255 ){
				break;
			}
		}
		laststate = digitalRead( DHTPIN );
 
		if ( counter == 255 )
			break;
 
		if ( (i >= 4) && (i % 2 == 0) ){
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
	
		//Armazenando o valor da umidade
		sprintf(umidade, "%d.", dht11_dat[0]);
		sprintf(umidade2, "%d", dht11_dat[1]);
		strcat(umidade, umidade2);
		
		//Armazenando o valor da temperatura
		sprintf(temperatura, "%d.", dht11_dat[2]);
		sprintf(temperatura2, "%d", dht11_dat[3]);
		strcat(temperatura, temperatura2);
		
		delay( 1000 ); 

	}else  {
		//printf( "Data not good, skip\n" );
	}
}

// Funcao para simular o sensor de luminosidade BH1750
void medidaLuminosidade(float *luminosidade){
	float volts = readVoltage(0); //Ler a voltagem do potenciometro do canal 0
	*luminosidade = (65535 * volts)/3.3; //65535LUX e o valor maximo lido pelo sensor simulado
}
// Funcao para simular o sensor de pressao atmosferica BMP180
void medidaPressao(float *pressao){
	float volts = readVoltage(3); //Ler a voltagem do potenciometro do canal 3
	*pressao = (1100 * volts)/3.3; //1100pHa e o valor maximo lido pelo sensor simulado
}

