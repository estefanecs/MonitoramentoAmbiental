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
#include <pthread.h>

#define MAXTIMINGS	85
#define DHTPIN		0 // Usa a GPIO 17 se no WiringPiSetup
/* Define configuraÃ§Ãµes do LCD*/

#define LCD_Rows 2
#define LCD_Cols  16
#define LCD_bits  4
#define LCD_RS  6
#define LCD_E   31
#define LCD_D0 26
#define LCD_D1 27
#define LCD_D2 28
#define LCD_D3 29
#define LCD_D4  0
#define LCD_D5  0
#define LCD_D6  0
#define LCD_D7  0

/* Define pinos dos botÃµes */
#define B0 21
#define B1 24
#define B2 25
// Led da placa
#define LED 22

/* Define estados da maquina de estados*/
#define ES_MENU0  0
#define ES_MENU1  1
#define ES_MENU2  2
#define ES_MENU3  3
#define ES_MENU4  4
#define ES_HTEMP  5
#define ES_HUMID  6
#define ES_HPRES  7
#define ES_HLUMI  8
#define ES_TIME   9

//Variaveis globais
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
unsigned int tempo =0;
unsigned int tempoAnterior=0;
int tempo_medicao = 10000;

/* prototipos de funÃ§Ã£o*/

void leitura(int *luminosidade,int *pressao);
void leitura_dht11(char *temperatura, char *umidade); //Leitura do DHT11
long int maps(long int in, long int in_min,long int in_max, long int out_min, long int out_max);
float fmap(float in, float in_min,float in_max, float out_min, float out_max);
int getMilisegundos(int digitos[7]);
void *leituraSensores(char *temperatura, char *umidade,float *luminosidade,float *pressao);


int main(void){

    int lcd;         // variavel para o display LCD
    wiringPiSetup(); // configuraÃ§Ã£o do wiringPi

    /* define os botÃµes como entradas */
    pinMode(B0,INPUT);
    pinMode(B1,INPUT);
    pinMode(B2,INPUT);

    /*variavel do estado*/
    int estado=0;

    /* chama a funÃ§Ã£o que configura os pinos do display LCD*/
    lcd = lcdInit(LCD_Rows,LCD_Cols,LCD_bits,LCD_RS,LCD_E,LCD_D0,LCD_D1,LCD_D2,LCD_D3,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
    

    /* variveis do estado do botÃ£o  */
    int b0,b1,b2;
    b0=b1=b2=0; // inicializa os botÃµes com 0

    /* variavÃ©is para leitura dos sensores */
    char temperatura[10]={"0"};
	char umidade[10]={"0"};	
    int  pressao,luminosidade;

    /* variavel de controle do tempo
       {[dezena minuto]-[unidade minuto] digitos 6-5}
       {[dezena segundo]-[unidade segundo] digitos 4-3}
       {[centena milisegundo]-[dezena milisegundo]-[unidade milisegundo] digitos 2-1-0}  
    */
    int digitos_medicao[7] ={0}; 
    int idx =0;

    /* VariavÃ©is para o controle de tempo
    |  tempo recebe o tempo de execuÃ§Ã£o do programa
    |  tempoAnterior recebe o tempo de execuÃ§Ã£o em um intervalo anterior 
    */

    /* variaveis auxiliares */

    int idx_historico =0; // variavel que controla a disposiÃ§Ã£o do menu dos historico

    /* variaveis de historico das mediÃ§Ãµes */

    int H_temperaturas[10]={0};
    int H_umidades[10]={0};      
    int H_pressao[10]={0};      
    int H_luminosidade[10]={0}; 

   
    // strings de texto
    char menu[5][16] = {"TEMP","UMI","PRES","LUMI","TIME"};
    
        
        // limpa o display
        lcdClear(lcd); 

        /*-----------------Ativa comunicaÃ§Ã£o I2c ---------------------------*/
        if(openI2CBus("/dev/i2c-1") == -1){
            return EXIT_FAILURE;
        }
        setI2CSlave(0x48);

        /*-------------------------------------------------------------------*/

        /* REALIZA LEITURA ANTES DA PRIMEIRA EXECUÃ‡ÃƒO*/
    /*    leitura(&luminosidade,&pressao); //faz nova leitura
        leitura_dht11(temperatura, umidade); //Leitura do DHT11*/
        pthread_t threadSensores;
		pthread_create(&threadSensores, NULL,leituraSensores(temperatura,umidade,&luminosidade,&pressao),NULL);
        
        
        
    	do{
            /* Realiza leitura dos botÃµes*/
    		b0 = digitalRead(B0);   // botÃ£o pressionado tem como leitura um nivÃ©l logico baixo
            b1= digitalRead(B1);    // botÃ£o naÃµ pressionado tem como leitura um nivÃ©l logico alto
    		b2 = digitalRead(B2);

            /*Limpa o display Lcd a cada execuÃ§Ã£o por vez*/
             //lcdClear(lcd);

            /*Controle de tempo da mediÃ§Ã£o */
            //tempo = millis();
          /*  if(tempo - tempoAnterior >= tempo_medicao){   // se passou do intervalo de tempo definido
                tempoAnterior= tempo; //atualiza o tempo passado
                tempo=0;          // zera o acumulador
                leitura(&luminosidade,&pressao); //faz nova leituraleitura(&luminosidade,&pressao); //faz nova leitura
        		leitura_dht11(temperatura, umidade); //Leitura do DHT11printf("Nova leitura realizada\n");
                printf("Tempo de leitura Ã©:%d milisegundos\n", tempo_medicao);
                printf("press:%d --- lumi:%d\n",pressao,luminosidade);
            }*/

            /*  A interface Ã© implementada como um maquina de estados usando Switch-case
            |   cada exibiÃ§Ã£o no display Ã© considerada como um estado diferente
            |   os botÃµes sÃ£o responsavÃ©is pela interaÃ§Ã£o do usuario com a interface
            |   b0 Ã© usado para alternar menus
            |   b1 Ã© usado para funÃ§Ã£o_1 de cada menu e submenu
            |   b2 Ã© usado para funÃ§Ã£o_2 de cada menu e submenu
            */
    		switch(estado){
    			case ES_MENU0:	// Estado Menu inicial exibindo opÃ§Ãµes de temperatura e historico de Temperatura
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPrintf(lcd,"%s:%s",menu[0],temperatura);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÃ‡A DE ESTADO*/
    				if(!b1){	  // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
    					estado=ES_HTEMP; //  Alterna para Historico de mediÃ§Ãµes
    				}
    				if(!b0){	  //BotÃ£o de alternar Menu pressionado
    					estado=ES_MENU1; // Alterna para proxima opÃ§Ã£o do Menu
    				}
    			break;

    			case ES_MENU1:	// Estado Menu inicial exibindo opÃ§Ãµes de Umidade e historico de Umidade
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPrintf(lcd,"%s:%s",menu[1],umidade);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÃ‡A DE ESTADO*/
    				if(!b1){	  // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
    					estado=ES_HUMID; //  Alterna para Historico de mediÃ§Ãµes
    				}
    				if(!b0){	  //BotÃ£o de alternar Menu pressionado
    					estado=ES_MENU2; // Alterna para proxima opÃ§Ã£o do Menu
    				}
    			break;

                case ES_MENU2:	// Estado Menu inicial exibindo opÃ§Ãµes de Pressao e historico de Pressao
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPrintf(lcd,"%s:%d",menu[2],pressao);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÃ‡A DE ESTADO*/
    				if(!b1){	  // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
    					estado=ES_HPRES; //  Alterna para Historico de mediÃ§Ãµes
    				}
    				if(!b0){	  //BotÃ£o de alternar Menu pressionado
    					estado=ES_MENU3; // Alterna para proxima opÃ§Ã£o do Menu
    				}
    			break;

    			case ES_MENU3:	// Estado Menu inicial exibindo opÃ§Ãµes de Luminosidade e historico de Luminosidade
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPrintf(lcd,"%s:%d",menu[3],luminosidade);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÃ‡A DE ESTADO*/
    				if(!b1){	  // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
    					estado=ES_HLUMI; //  Alterna para Historico de mediÃ§Ãµes
    				}
    				if(!b0){	  //BotÃ£o de alternar Menu pressionado
    					estado=ES_MENU4; // Alterna para proxima opÃ§Ã£o do Menu
    				}
    			break;

    			case ES_MENU4:	// Estado Menu inicial exibindo opÃ§Ãµes de Tempo e alterar tempo
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPuts(lcd,menu[4]);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"->Alterar tempo");

    				/*LOGICA DE MUDANÃ‡A DE ESTADO*/
    				if(!b1){	  // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
    					estado=ES_TIME; //  Alterna para Historico de mediÃ§Ãµes
                        idx=0;
    				}
    				if(!b0){	  //BotÃ£o de alternar Menu pressionado
    					estado=ES_MENU0; // Alterna para proxima opÃ§Ã£o do Menu
    				}
    			break;

                case ES_HTEMP:
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPrintf(lcd,"%d-%d", idx_historico+1,H_temperaturas[idx_historico]);
    				lcdPosition(lcd,0,1);
    				lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_temperaturas[idx_historico+1]);

    				/*LOGICA DE MUDANÃ‡A DE ESTADO*/
    				if(!b1){	  // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
    					estado=ES_MENU0;  // Alterna para Menu anterior
    					idx_historico=0;  // reseta o indice do historico
    				}
    				/*Logica adicional para configuraÃ§Ã£o do menu*/
    				if(!b0){	  //BotÃ£o de alternar Menu pressionado
                        		if(idx_historico==8)
                        			idx_historico=0;
                        		else
                        			idx_historico+=2;
    				}
                break;

                case ES_HUMID:
                	/*Imprime no Display as informaÃ§Ãµes do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d-%d", idx_historico+1,H_umidades[idx_historico]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_umidades[idx_historico+1]);

                    /*LOGICA DE MUDANÃ‡A DE ESTADO*/
                    if(!b1){      // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
                        estado=ES_MENU1; // Alterna para Menu anterior
                        idx_historico=0; // reseta o indice do historico
                    }
                    /*Logica adicional para configuraÃ§Ã£o do menu*/
                    if(!b0){      //BotÃ£o de alternar Menu pressionado
                                if(idx_historico==8)
                                    idx_historico=0;
                                else
                                    idx_historico+=2;
                    }
                break;

                case ES_HPRES:
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d-%d", idx_historico+1,H_pressao[idx_historico]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_pressao[idx_historico+1]);

                    /*LOGICA DE MUDANÃ‡A DE ESTADO*/
                    if(!b1){      // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
                        estado=ES_MENU2; // Alterna para Menu anterior
                        idx_historico=0; // reseta o indice do historico
                    }
                    /*Logica adicional para configuraÃ§Ã£o do menu*/
                    if(!b0){      //BotÃ£o de alternar Menu pressionado
                                if(idx_historico==8)
                                    idx_historico=0;
                                else
                                    idx_historico+=2;
                    }
                break;

                case ES_HLUMI:
                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d-%d", idx_historico+1,H_luminosidade[idx_historico]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_luminosidade[idx_historico+1]);

                    /*LOGICA DE MUDANÃ‡A DE ESTADO*/
                    if(!b1){      // BotÃ£o que seleciona primeira opÃ§Ã£o apertado
                        estado= ES_MENU3; // Alterna para Menu anterior
                        idx_historico=0; // reseta o indice do historico
                    }
                    /*Logica adicional para configuraÃ§Ã£o do menu*/
                    if(!b0){      //BotÃ£o de alternar Menu pressionado
                                if(idx_historico==8)
                                    idx_historico=0;
                                else
                                    idx_historico+=2;
                    }
                break;

                case ES_TIME:

                    /*Imprime no Display as informaÃ§Ãµes do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d%d:",digitos_medicao[6],digitos_medicao[5]);
                    lcdPosition(lcd,3,0);
                    lcdPrintf(lcd,"%d%d:",digitos_medicao[4],digitos_medicao[3]);
                    lcdPosition(lcd,6,0);
                    lcdPrintf(lcd,"%d%d%d",digitos_medicao[2],digitos_medicao[1],digitos_medicao[0]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"Digito:%d",idx+1);

                    if(!b1){    // Incrementa o digito
                        digitos_medicao[idx] == 9 ? digitos_medicao[idx]=0: digitos_medicao[idx]++;  
                    }
                    if(!b2){    // Sai do menu de alterar tempo

                        estado = ES_MENU4;  // altera para o estado do menu de configurar tempo

                        /* atualiza o tempo de mediÃ§Ã£o*/
                        tempo_medicao= getMilisegundos(digitos_medicao); // chama funÃ§Ã£o que converte o tempo definido nos digitos
                    }
                    if(!b0){    // Alterna o digito
                        idx ==6 ? idx=0:idx++;
                    }
                break;
    			default: break;
    		}
            if(!b0 || !b1 || !b2){
                lcdClear(lcd);
            }
    		delay(100);
    	}while(TRUE);
    return 0;
}


void leitura(int *luminosidade,int *pressao){
        *luminosidade= fmap(readVoltage(0),0,3.25,0,65400);
        *pressao= fmap(readVoltage(3),0,3.25,0,1100);
        printf("LEITURA REALIZADA \n");
}

// Funcao que realiza a leitura do sensor DHT11
void leitura_dht11(char *temperatura, char *umidade){

    int leu = 0;
    
    do{
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

/*  FunÃ§Ã£o getMilisegundos
|   Recebe um vetor com os digitos referentes as grandezas de tempo e converte para milisegundos
*/
int getMilisegundos(int digitos[7]){
    return digitos[0]+ (digitos[1]*10) + (digitos[2] * 100)+
            (digitos[3]*1000) +  (digitos[4]*10000) +
            ( digitos[5]+digitos[6]*10) *60000;
}

/* maps recebe um inteiro e mapeia de uma faixa de valores inteiros para outra faixa valores inteiro */
long int maps(long int in, long int in_min,long int in_max, long int out_min, long int out_max){
    return (in- in_min) * (out_max- out_min) / (in_max - in_min) + out_min;
}

/* 
|   fmap recebe um nÃºmero de ponto flutuante e mapeia de uma faixa de valores de ponto flutuante
|   para outra faixa valores
*/
float fmap(float in, float in_min,float in_max, float out_min, float out_max){
    return (in- in_min) * (out_max- out_min) / (in_max - in_min) + out_min;
}

void *leituraSensores(char *temperatura, char *umidade,float *luminosidade,float *pressao){
	while(1){
		tempo = millis();
		if(tempo - tempoAnterior >= tempo_medicao){   // se passou do intervalo de tempo definido
            tempoAnterior= tempo; //atualiza o tempo passado
            tempo=0;          // zera o acumulador
           // leitura(&luminosidade,&pressao); //faz nova leituraleitura(&luminosidade,&pressao); //faz nova leitura
            *luminosidade= fmap(readVoltage(0),0,3.25,0,65400);
        	*pressao= fmap(readVoltage(3),0,3.25,0,1100);
        	printf("LEITURA REALIZADA de luminosidade e pressao \n");
        	leitura_dht11(temperatura, umidade); //Leitura do DHT11printf("Nova leitura realizada\n");
            printf("Tempo de leitura Ã©:%d milisegundos\n", tempo_medicao);
            printf("| press:%d --- lumi:%d\n",pressao,luminosidade);
        }
	}
}
