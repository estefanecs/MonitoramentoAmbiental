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
#include "SBC.h"

/*-------------------Variaveis globais-----------------*/
unsigned int tempo =0;
unsigned int tempoAnterior=0;
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
    int i; //variavel para loop
    if(openI2CBus("/dev/i2c-1") == -1){ //Verifica se houve falha ao mapear o I2C
        return EXIT_FAILURE;
    }
    setI2CSlave(0x48); //Ativa a configuracao do I2C
    

    //Limpa o historico
    for(i=0;i<MAX;i++){
        historico[i].lumi= 0;
        historico[i].press= 0;
        strcpy(historico[i].temp,"0.0");
        strcpy(historico[i].umi,"0.0");
    
        historico_display[i].lumi= 0;
        historico_display[i].press= 0;
        strcpy(historico_display[i].temp,"0.0");
        strcpy(historico_display[i].umi,"0.0");
    }

    pthread_t threadSensores, interface;
	pthread_create(&threadSensores, NULL,leituraSensores,NULL); //criacao da thread para leitura dos sensores
	pthread_create(&interface,NULL,displayLCD,NULL); //Criacao da thread para exibir dados no display LCD
        
	while(1){
	}
}

//-------------------- Funcao para a exibicao de dados no display LCD --------------------
void *displayLCD(){
    wiringPiSetup(); // configuracao do wiringPi
    //define os botoes como entrada
    pinMode(B0,INPUT);
    pinMode(B1,INPUT);
    pinMode(B2,INPUT);

    int estado=0; //variavel do estado
    
    int b0=0,b1=0,b2=0;//variaveis do estado do botao
    int idx =0;
    int idx_historico =0; // variavel que controla a disposicao do menu dos historico
    char menu[5][16] = {"TEMP","UMI","PRES","LUMI","TIME"}; //String de texto para exibir no display
     
    //Variavel com a configuracao dos pinos do display LCD
    int lcd = lcdInit(LCD_Rows,LCD_Cols,LCD_bits,LCD_RS,LCD_E,LCD_D0,LCD_D1,LCD_D2,LCD_D3,LCD_D4,LCD_D5,LCD_D6,LCD_D7);

    int digitos_medicao[7] ={0};  //Variavel de controle do tempo
    /* {[dezena minuto]-[unidade minuto] digitos 6-5}
       {[dezena segundo]-[unidade segundo] digitos 4-3}
       {[centena milisegundo]-[dezena milisegundo]-[unidade milisegundo] digitos 2-1-0} */
   
        
    lcdClear(lcd); //limpa o display
    
    while(1){
        //Realiza a leitura dos botoes
        //Botão quando precionado tem leitura em nivel logico baixo. Nao precisonado tem como leitura nivel logico alto
    	b0 = digitalRead(B0);  
        b1 = digitalRead(B1); 
    	b2 = digitalRead(B2);

        /*  A interface ÃÂ© implementada como um maquina de estados usando Switch-case
        |   cada exibiÃÂ§ÃÂ£o no display ÃÂ© considerada como um estado diferente
        |   os botÃÂṁes sÃÂ£o responsavÃÂ©is pela interaÃÂ§ÃÂ£o do usuario com a interface
        |   b0 ÃÂ© usado para alternar menus
        |   b1 ÃÂ© usado para funÃÂ§ÃÂ£o_1 de cada menu e submenu
        |   b2 ÃÂ© usado para funÃÂ§ÃÂ£o_2 de cada menu e submenu
        */
    	switch(estado){
    		case ES_MENU0:	// Estado que exibe a medida atual da temperatura e a opcao de historico de temperatura
                /*Imprime no Display as informaÃÂ§ÃÂṁes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%s",menu[0],temperatura);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");
    			
    			/*LOGICA DE MUDANÃâḂA DE ESTADO*/
    			if(!b1){	  // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
    				estado=ES_HTEMP; //  Alterna para Historico de mediÃÂ§ÃÂṁes
    			}
    			if(!b0){	  //BotÃÂ£o de alternar Menu pressionado
    				estado=ES_MENU1; // Alterna para proxima opÃÂ§ÃÂ£o do Menu
    			}
    			break;

    		case ES_MENU1:	// Estado que exibe a medida atual de umidade e a opcao de historico de umidade
                /*Imprime no Display as informaÃÂ§ÃÂṁes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%s",menu[1],umidade);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			/*LOGICA DE MUDANÃâḂA DE ESTADO*/
    			if(!b1){	  // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
    				estado=ES_HUMID; //  Alterna para Historico de mediÃÂ§ÃÂṁes
    			}
    			if(!b0){	  //BotÃÂ£o de alternar Menu pressionado
    				estado=ES_MENU2; // Alterna para proxima opÃÂ§ÃÂ£o do Menu
    			}
    			break;

            case ES_MENU2:	// Estado que exibe a medida atual de pressao e a opcao de historico de pressao
                /*Imprime no Display as informaÃÂ§ÃÂṁes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[2],pressao);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			/*LOGICA DE MUDANÃâḂA DE ESTADO*/
    			if(!b1){	  // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
    				estado=ES_HPRES; //  Alterna para Historico de mediÃÂ§ÃÂṁes
    			}
    			if(!b0){	  //BotÃÂ£o de alternar Menu pressionado
    				estado=ES_MENU3; // Alterna para proxima opÃÂ§ÃÂ£o do Menu
    			}
    			break;

    		case ES_MENU3:	// Estado que exibe a medida atual de luminosidade e a opcao de historico de luminosidade
                /*Imprime no Display as informaÃÂ§ÃÂṁes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[3],luminosidade);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			/*LOGICA DE MUDANÃâḂA DE ESTADO*/
    			if(!b1){	  // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
    				estado=ES_HLUMI; //  Alterna para Historico de mediÃÂ§ÃÂṁes
    			}
    			if(!b0){	  //BotÃÂ£o de alternar Menu pressionado
    				estado=ES_MENU4; // Alterna para proxima opÃÂ§ÃÂ£o do Menu
    			}
    			break;

    		case ES_MENU4:	// Estado que exibe o valor do tempo e a opcao de altera-lo
                /*Imprime no Display as informaÃÂ§ÃÂṁes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPuts(lcd,menu[4]);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"->Alterar tempo");

    			/*LOGICA DE MUDANÃâḂA DE ESTADO*/
    			if(!b1){	  // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
    				estado=ES_TIME; //  Alterna para Historico de mediÃÂ§ÃÂṁes
                    idx=0;
    			}
    			if(!b0){	  //BotÃÂ£o de alternar Menu pressionado
    				estado=ES_MENU0; // Alterna para proxima opÃÂ§ÃÂ£o do Menu
    			}
    			break;

            case ES_HTEMP:
                /*Imprime no Display as informaÃ§Ãṁes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%s", idx_historico+1,historico_display[idx_historico].temp);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%s" ,idx_historico+2,historico_display[idx_historico+1].temp);

    			/*LOGICA DE MUDANÃâḂA DE ESTADO*/
    			if(!b1){	  // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
    				estado=ES_MENU0;  // Alterna para Menu anterior
    				idx_historico=0;  // reseta o indice do historico
    			}
    			/*Logica adicional para configuraÃÂ§ÃÂ£o do menu*/
    			if(!b0){	  //BotÃÂ£o de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
    			}
                break;

            case ES_HUMID:
                /*Imprime no Display as informaÃ§Ãṁes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%s", idx_historico+1,historico_display[idx_historico].umi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%s" ,idx_historico+2,historico_display[idx_historico+1].umi);

                /*LOGICA DE MUDANÃâḂA DE ESTADO*/
                if(!b1){      // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
                    estado=ES_MENU1; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
            	}
                /*Logica adicional para configuraÃÂ§ÃÂ£o do menu*/
                if(!b0){      //BotÃÂ£o de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_HPRES:
                /*Imprime no Display as informaÃ§Ãṁes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].press);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].press);

                /*LOGICA DE MUDANÃâḂA DE ESTADO*/
                if(!b1){      // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
                    estado=ES_MENU2; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                }
                /*Logica adicional para configuraÃÂ§ÃÂ£o do menu*/
                if(!b0){      //BotÃÂ£o de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_HLUMI:
                /*Imprime no Display as informaÃ§Ãṁes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].lumi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].lumi);
                
                /*LOGICA DE MUDANÃâḂA DE ESTADO*/
                if(!b1){      // BotÃÂ£o que seleciona primeira opÃÂ§ÃÂ£o apertado
                    estado= ES_MENU3; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                }
                /*Logica adicional para configuraÃÂ§ÃÂ£o do menu*/
                if(!b0){      //BotÃÂ£o de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_TIME:
                /*Imprime no Display as informaÃÂ§ÃÂṁes do menu atual*/
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
                    /* atualiza o tempo de mediÃÂ§ÃÂ£o*/
                    tempo_medicao= getMilisegundos(digitos_medicao); // chama funÃÂ§ÃÂ£o que converte o tempo definido nos digitos
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
	}
	pthread_exit(NULL);
}

//--------------------------------------------------------------------------
// Funcao que recebe um vetor com os digitos referentes as grandezas de tempo e converte para milisegundos.
int getMilisegundos(int digitos[7]){
    return digitos[0]+ (digitos[1]*10) + (digitos[2] * 100)+
            (digitos[3]*1000) +  (digitos[4]*10000) +
            ( digitos[5]+digitos[6]*10) *60000;
}

//-------------------- Funcao para leitura dos sensores  --------------------
void *leituraSensores(){
	while(1){
		tempo = millis();
		if(tempo - tempoAnterior >= tempo_medicao){   // se passou do intervalo de tempo definido
            tempoAnterior= tempo; //atualiza o tempo passado
            tempo=0;          // zera o acumulador
            leitura(&luminosidade,&pressao,temperatura,umidade,historico_display); //Realiza a leitura dos sensores
            printf("Tempo de leitura:%d milisegundos\n", tempo_medicao);
            printf("| pressao:%d | luminosidade:%d\n",pressao,luminosidade);
		}
		sleep(5);
    }
	pthread_exit(NULL); //Encerra a thread
}

//--------------------------------------------------------------------------
//Adiciona uma nova item(medida) no historico. Quando a lista possuir 10 itens, o mais antigo e sobrescrito.
void add(int lum, int press, char *temp , char *umi){
    if(L== -1){     //Define a posicao de insercao do elemento se a lista estiver vazia
        L=0;    //A medicao mais recente eh a primeira posicao
        O=0;    //A medicao mais antiga eh a primeira posicao
    }
    else{  //Define a posicao de insercao do elemento se a lista nao estiver vazia
        if(L == MAX-1){
            L=0;
            O++;
        }
        else if(L<O){
            L++;
            O++;
        }
        else
            L++;
    }
    //Insere os novos itens na posicao definida acima
    historico[L].lumi= lum;
    historico[L].press= press;
    strcpy(historico[L].temp,temp);
    strcpy(historico[L].umi,umi);
}

//--------------------------------------------------------------------------
//Cria um vetor ordenado da medicao mais recente para a mais antiga
void getOrdenada(Dados *v){
    int i,idx;
    idx= L;
    for(i=0;i<MAX;i++){
        v[i]=historico[idx];
        if(idx !=O && idx ==0)
            idx= MAX-1;
        else
            idx--;
    }
}
