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
#include "analogico.h"
#include "dht11.h"
#include "displayLCD.h"
//#include "historico.h"
#include "mqtt.h"
#include "sbc.h"

#include <time.h>
#include <pthread.h>

#define Host_broker "10.0.0.101"
#define broker_temperatura "monitoramentoAmbiental/temperatura"
#define broker_umidade "monitoramentoAmbiental/umidade"
#define broker_luminosidade "monitoramentoAmbiental/luminosidade"
#define broker_pressao "monitoramentoAmbiental/pressao"
#define broker_tempo "monitoramentoAmbiental/tempo"

#define broker_temperatura_h "monitoramentoAmbiental/historicoTemperatura"
#define broker_umidade_h "monitoramentoAmbiental/historicoUmidade"
#define broker_luminosidade_h "monitoramentoAmbiental/historicoLuminosidade"
#define broker_pressao_h "monitoramentoAmbiental/historicoPressao


/*-------------------Variaveis globais-----------------*/
//unsigned int tempo =0;
//unsigned int tempoAnterior=0;
//char tempoLido[]="";
//int tempo_medicao = 10000;
int intervaloTempo = 1;
int analogico_s[2] = {0}; // Sensor Analogico de Pressao e Luminosidade
int dht_s[4] = {0} // Sensor Digital Temperatura Umidade
//char umidade[10]={"0"};	
//int  pressao=0,luminosidade=0;
//Variaveis de historico das medicoes
Dados historico[MAX]; 
Dados historico_display[MAX];
//Variaveis de controle da lista do historico
//int L=-1;
//int O=-1;
/*----------------------------------------------------*/


int main(void){
	wiringPiSetup(); //Configuracao do wiringPi
	mosquitto_lib_init(); //Inicializa a biblioteca do MQTT, mosquitto
    if(openI2CBus("/dev/i2c-1") == -1){ //Verifica se houve falha ao mapear o I2C
        return EXIT_FAILURE;
    }
    setI2CSlave(0x48); //Ativa a configuracao do I2C
    
	CreateThreads();

}

void CreateThreads(){

	pthread_t analogicoT,dht11T,pubT,clientT, interfaceT;

	pthread_create(&analogicoT, NULL, leitura_analogica, analogico_s);
	pthread_create(&dht11T, NULL, leitura_dht11, dht_s);
	pthread_create(&pubT,NULL, PublicarValores, &intervaloTempo);
	pthread_create(&clientT,NULL, LerIntervaloTempo, &intervaloTempo);
	pthread_create(&interface,NULL,displayLCD,NULL); //Criacao da thread para exibir dados no display LCD
}  

void * LerIntervaloTempo(void * arg){

	Cliente clientTempo;
	clientTempo = (Cliente){.Nome = "Leitor de intevalos - Client",.Host = Host_broker,.Topico = broker_tempo};
	create_client(clientTempo);
	// como ler o tempo e salvar em uma variavel? // msg->payload ?
}

void * PublicarValores(void * arg){

	int *iptr = (int *)arg;

	Publisher pubPres;
	pubPres = (Publisher){.Nome = "ads1115 Pressao - Pub",.Host = Host_broker,.Topico = broker_pressao,.Msg = ""};

	Publisher pubLumi;
	pubLumi = (Publisher){.Nome = "ads1115 Luminosidade - Pub",.Host = Host_broker,.Topico = broker_luminosidade,.Msg = ""};

	Publisher pubUmid;
	pubUmid = (Publisher){.Nome = "dht11 Umidade - Pub",.Host = Host_broker,.Topico = broker_umidade,.Msg = ""};

	Publisher pubTemp;
	pubTemp = (Publisher){.Nome = "dht11 Temperatura - Pub",.Host = Host_broker,.Topico = broker_temperatura,.Msg = ""};

	int sensores_value[6];
	char temp[255],tempU[255],umidade[255],umidadeU[255],pressao[255],luminosidade[255];

	do{
		
		sensores_value[0] = analogico_s[0];
		sensores_value[1] = analogico_s[1];
		sensores_value[2] = dht_s[0];
		sensores_value[3] = dht_s[1];
		sensores_value[4] = dht_s[2];
		sensores_value[5] = dht_s[3];

		// Salvar sensores_values no historico


		sprintf(pressao,"%d",sensores_value[0])
		sprintf(luminosidade,"%d",sensores_value[1])
		sprintf(umidade,"%d",sensores_value[2])
		sprintf(umidadeU,"%d",sensores_value[3])
		sprintf(temp,"%d",sensores_value[4])
		sprintf(tempU,"%d",sensores_value[5])

		strcat(umidade,".");
		strcat(umidade,umidadeU);
		strcat(temp,".");
		strcat(temp,tempU);

		pubPres.Msg = pressao;
		pubLumi.Msg = luminosidade;
		pubUmid.Msg = umidade;
		pubTemp.Msg = temp;
		
		publicar(pubPres);
		publicar(pubLumi);
		publicar(pubUmid);
		publicar(pubTemp);

		sleep(*iptr);

	}while(1);
	
	
}
//--------------------------------------------------------------------------

