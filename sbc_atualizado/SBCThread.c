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
#include "dht11.h"

#include <lcd.h>
#include <pthread.h>
#include "SBC.h"
#include <time.h>

/*-------------------Variaveis globais-----------------*/
unsigned int tempo =0;
unsigned int tempoAnterior=0;
char tempoLido[]="";
float intervaloTempo =3.0;
int tempo_medicao = 10000;
char temperatura[10]={"0"};
char umidade[10]={"0"};	
int  pressao=0,luminosidade=0;
//Variaveis de historico das medicoes
Dados historico[MAX]; 
Dados historico_display[MAX];
//Variaveis de controle da lista do historico
int L=-1;
int O=-1;
/*----------------------------------------------------*/


int main(void){
	wiringPiSetup(); //Configuracao do wiringPi
	mosquitto_lib_init(); //Inicializa a biblioteca do MQTT, mosquitto
    int i; //variavel para loop
    if(openI2CBus("/dev/i2c-1") == -1){ //Verifica se houve falha ao mapear o I2C
        return EXIT_FAILURE;
    }
    setI2CSlave(0x48); //Ativa a configuracao do I2C
    
    //Configuracao do cliente leitor do MQTT
    int rc;
	struct mosquitto *leitor;
	leitor = mosquitto_new("inscrito",true,&id);
	mosquitto_connect_callback_set(leitor, on_connect);
	mosquitto_message_callback_set(leitor, on_message);
	mosquitto_username_pw_set(leitor,"aluno","aluno*123"); //Define usuario e senha
	rc = mosquitto_connect(leitor,"10.0.0.101",1883,10); //Inicializa a conexao com o broker
	if(rc){ //Verifica se a conexao foi bem sucedida
		printf("O cliente nao conseguiu conecta-se ao broker\n");
		return -1;
	}

    //Limpa o historico
    for(i=0;i<MAX;i++){
        historico[i].lumi = 0;
        historico[i].press= 0;
        strcpy(historico[i].temp,"-");
        strcpy(historico[i].umi,"-");
    
        historico_display[i].lumi= 0;
        historico_display[i].press= 0;
        strcpy(historico_display[i].temp,"-");
        strcpy(historico_display[i].umi,"-");
    }

    pthread_t threadSensores, interface;
	pthread_create(&threadSensores, NULL,leituraSensores,NULL); //criacao da thread para leitura dos sensores
	pthread_create(&interface,NULL,displayLCD,NULL); //Criacao da thread para exibir dados no display LCD
        
	while(1){
		mosquitto_loop_start(leitor);
		mosquitto_loop_stop(leitor,true);
	}
	mosquitto_disconnect(leitor);
	mosquitto_destroy(leitor);
}

//-------------------- Funcao para a exibicao de dados no display LCD --------------------


//--------------------------------------------------------------------------
// Funcao que recebe um vetor com os digitos referentes as grandezas de tempo e converte para milisegundos.
int getMilisegundos(int digitos[7]){
    return digitos[0]+ (digitos[1]*10) + (digitos[2] * 100)+
            (digitos[3]*1000) +  (digitos[4]*10000) +
            ( digitos[5]+digitos[6]*10) *60000;
}

// Funcao que recebe um vetor com os digitos referentes as grandezas de tempo e converte para milisegundos.
float getSegundos(int digitos[7]){
    return (digitos[0]+ (digitos[1]*10.0) + (digitos[2] * 100.0) /1000.0)+
            (digitos[3]) +  (digitos[4]*10) +
            ( digitos[5]+digitos[6]*10) *60;
}

//-------------------- Funcao para leitura dos sensores  --------------------
void *leituraSensores(){
	while(1){
        leitura(&luminosidade,&pressao,temperatura,umidade,historico_display); //Realiza a leitura dos sensores
        printf("Tempo de leitura:%d milisegundos\n", tempo_medicao);
        printf("| pressao:%d | luminosidade:%d\n",pressao,luminosidade);
		sleep(intervaloTempo);
    }
	pthread_exit(NULL); //Encerra a thread
}

//--------------------------------------------------------------------------

