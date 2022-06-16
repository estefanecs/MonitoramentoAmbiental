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

/*Arquivo para funções dos sensores*/

void leitura(int *luminosidade,int *pressao,char *temperatura, char *umidade,Dados *historico_display){
    /*  Realiza a leitura dos sensores*/
        *luminosidade= fmap(readVoltage(0),0,3.28,0,65400);
        *pressao= fmap(readVoltage(3),0,3.28,300,1100);
        leitura_dht11(temperatura, umidade); //Leitura do DHT11

    /*Insere novas leituras no historico*/
        add(*luminosidade,*pressao,temperatura,umidade); // insere no historico
        getOrdenada(historico_display);  //atualiza vetor de exbição do display
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
            //f = dht11_dat[2] * 9. / 5. + 32;
        
            //Armazenando o valor da umidade
            sprintf(umidade, "%d.", dht11_dat[0]);
            sprintf(umidade2, "%d", dht11_dat[1]);
            strcat(umidade, umidade2);
            
            //Armazenando o valor da temperatura
            sprintf(temperatura, "%d.", dht11_dat[2]);
            sprintf(temperatura2, "%d", dht11_dat[3]);
            strcat(temperatura, temperatura2);
            
            printf("\ntemperatura %s | umidade %s", temperatura, umidade);
            
            leu = 1;

        }else  {
            leu = 0;
            delay( 1000 ); 
        }
    }while(leu == 0);
    
}

/* maps recebe um inteiro e mapeia de uma faixa de valores inteiros para outra faixa valores inteiro */
long int maps(long int in, long int in_min,long int in_max, long int out_min, long int out_max){
    return (in- in_min) * (out_max- out_min) / (in_max - in_min) + out_min;
}

/* 
|   fmap recebe um número de ponto flutuante e mapeia de uma faixa de valores de ponto flutuante
|   para outra faixa valores
*/
float fmap(float in, float in_min,float in_max, float out_min, float out_max){
    return (in- in_min) * (out_max- out_min) / (in_max - in_min) + out_min;
}

/*
|   Função getDataTempo faz a leitura da data e hora atual e insere na struct de data recebida
*/
void getDataTempo(struct tm *data){
    time_t segundos;
    time(&segundos); 
    data = localtime(&segundos);
}