#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <mosquitto.h>
#include <string.h>

#define TOPIC_TEMP "dht11/temperatura"
#define HOST "localhost"
#define USER "aluno"
#define PSW "aluno*123"
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
        enviar_msg(dht11_dat[2],dht11_dat[3]); // envia msg para o broker sempre que faz uma leitura bem sucedida
		delay( 200 ); 

	}else  {
		//printf( "Data not good, skip\n" );
	}
}

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
		printf("ID: %d\n", * (int *) obj);
		if(rc){
			printf("Erro de codigo: %d\n",rc);
			exit(-1);
		}
		mosquitto_subscribe(mosq,NULL,"dht11/temperatura",1);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("Nova mensagem para o topico %s: %s\n", msg->topic, (char *) msg->payload);
}

void enviar_msg(int Temp_1,int Temp_2){

// converte temperatura em string
    char S_temp_1[100],S_temp_2[20];
    sprintf(S_temp_1,"%d",Temp_1);
    sprintf(S_temp_2,"%d",Temp_2);
    strcat(S_temp_1,".");
    strcat(S_temp_1,S_temp_2);

    int rc;
	struct mosquitto *mosq;
	mosquitto_lib_init();
	
	mosq = mosquitto_new("Publisher_de_test",true,NULL);
	
	rc = mosquitto_connect(mosq,HOST,1883,60);
	if(rc != 0){
			printf("O Cliente nao conseguiu se conectar ao broker! - {{%d}}\n", rc);
			mosquitto_destroy(mosq);
			return -1;
	}
	printf("Estamos conectados ao broker.\n");
	
	mosquitto_publish(mosq,NULL,TOPIC_TEMP,strlen(S_temp_1),S_temp_1,1,false);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	
	mosquitto_lib_cleanup();
	
}

int main(){
    printf( "Sensor DHT11 - Raspberry Pi Zero\n" );
 
	if ( wiringPiSetup() == -1 )
		exit( 1 );
 
	while ( 1 )
	{
		read_dht11_dat();
	}
 
	return(0);
}