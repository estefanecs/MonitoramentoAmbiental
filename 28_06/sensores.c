#include "SBC.h"
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
#include <lcd.h>
#include <time.h>

/*Variavel global*/
int dht11_dat[5] = { 0, 0, 0, 0, 0 };

//Arquivo para funcoes dos sensores

void leitura(int *luminosidade,int *pressao,char *temperatura, char *umidade,Dados *historico_display, char *dataHora){
    /*  Realiza a leitura dos sensores*/
        char buff[40];
        *luminosidade= fmap(readVoltage(0),0,3.28,1,65535);
        *pressao= fmap(readVoltage(3),0,3.28,300,1100);
        getDataTempo(buff); // obtem data e hora da medicao
        leitura_dht11(temperatura, umidade); //Leitura do DHT11
	strcpy(dataHora,buff); //copia a data atual para a variavel
    /*Insere novas leituras no historico*/
        add(*luminosidade,*pressao,temperatura,umidade,buff); // insere no historico
        getOrdenada(historico_display);  //atualiza vetor de exibicao do display
        printf("LEITURA REALIZADA \n");
}

// Funcao que realiza a leitura do sensor DHT11
void leitura_dht11(char *temperatura, char *umidade){

    int leu = 0;
    
    do{
        char umidade2[10]; //armazena a segunda parte do valor de umidade
        char temperatura2[10]; //armazena a segunda parte do valor de temperatura
        
        uint8_t laststate = HIGH;
        uint8_t counter = 0;
        uint8_t j = 0, i;
    
        dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

        wiringPiSetup(); // Inicializa o WiringPi
        pinMode( DHTPIN, OUTPUT ); // Coloca o pino selecionado como Saida
        digitalWrite( DHTPIN, LOW ); // Altera o valor de Saida para baixo
        delay( 20 ); // Espera por 20 ms
        digitalWrite( DHTPIN, HIGH ); // Altera o valor de Saida para alto
        delayMicroseconds( 50 ); // Espera por 50 us
        pinMode( DHTPIN, INPUT ); // Altera o pino selecionado para Entrada


    
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
                if ( counter > 16 )
                    dht11_dat[j / 8] |= 1;
                j++;
            }
        }
    
        if ( (j >= 40) &&
            (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
        {        
            //Armazenando o valor da umidade em uma string
            sprintf(umidade, "%d.", dht11_dat[0]);
            sprintf(umidade2, "%d", dht11_dat[1]);
            strcat(umidade, umidade2);
            
            //Armazenando o valor da temperatura em uma string
            sprintf(temperatura, "%d.", dht11_dat[2]);
            sprintf(temperatura2, "%d", dht11_dat[3]);
            strcat(temperatura, temperatura2);
            
            printf("\ntemperatura %s | umidade %s\n", temperatura, umidade);
            
            leu = 1;

        }else  {
            leu = 0;
            delay(500); 
        }
    }while(leu == 0);
    
}

/* 
|   fmap recebe um numero de ponto flutuante e mapeia de uma faixa de valores de ponto flutuante
|   para outra faixa valores
*/
float fmap(float valorLido, float minPotenciometro,float maxPotenciometro, float minSensor, float maxSensor){
    return (valorLido- minPotenciometro) * (maxSensor- minSensor) / (maxPotenciometro - minPotenciometro) + minSensor;
}

//Funcao getDataTempo faz a leitura da data e hora atual e insere na struct de data recebida
void getDataTempo(char *data){
    time_t segundos;    //variavel para obter tempo atual
    char aux[20];       // variavel auxiliar para obter string
    struct tm *DH;      //struct da biblioteca time.h com os campos de data e hora
    
    time(&segundos);            // obtém o tempo
    DH = localtime(&segundos);  // obtém as informações de ta e hora
    
   aux[0]='\0'; 
   sprintf(aux,"Data %d/%d/%d ",DH->tm_mday,DH->tm_mon+1,DH->tm_year+1900); // escreve os dados para data e converte em string
   strcat(data,aux); // concatena na variavel

   aux[0]='\0';
   sprintf(aux,"Hora %d:%d:%d\n",DH->tm_hour,DH->tm_min,DH->tm_sec);    // escreve os dados para hora e converte em string
   strcat(data,aux); // concatena na variavel 
}
