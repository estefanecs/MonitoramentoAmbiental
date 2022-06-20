#include <wiringPi.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <lcd.h>

#include "displayLCD.h"
#include "sbc.h"

//Define as confirguacoes do LCD
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

//Define pinos dos botoes
#define B0 21
#define B1 24
#define B2 25

/Define estados da maquina de estados*/
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

/*----------------------------------------------------*/

void *displayLCD(int pressao,int luminosidade,int temperatura,int humidade,Dados * historico_display){
    wiringPiSetup(); // configuracao do wiringPi
    //define os botoes como entrada
    pinMode(B0,INPUT);
    pinMode(B1,INPUT);
    pinMode(B2,INPUT);

    int estado=0; //variavel do estado
    
    int b0=0,b1=0,b2=0;//variaveis do estado do botao
    int idx =0; // variavel para indice do historico
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
        //Botao quando precionado tem leitura em nivel logico baixo. Nao precisonado tem como leitura nivel logico alto
    	b0 = digitalRead(B0);  
        b1 = digitalRead(B1); 
    	b2 = digitalRead(B2);

        /*  A interface eh implementada como um maquina de estados usando Switch-case
            |   cada exibicao no display eh considerada como um estado diferente
            |   os botoes sao responsaveis pela interacao do usuario com a interface
            |   b0 eh usado para alternar menus
            |   b1 eh usado para funcao_1 de cada menu e submenu
            |   b2 eh usado para funcao_2 de cada menu e submenu
            */
    	switch(estado){
    		case ES_MENU0:	// Estado que exibe a medida atual da temperatura e a opcao de historico de temperatura
                 /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[0],temperatura);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");
    			
    			/*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	  // Botao que seleciona primeira opcao apertado
    				estado=ES_HTEMP; //  Alterna para Historico de medicoes
    			}
    			if(!b0){	  //Botao de alternar Menu pressionado
    				estado=ES_MENU1; // Alterna para proxima opcao do Menu
    			}
    			break;

    		case ES_MENU1:	// Estado que exibe a medida atual de umidade e a opcao de historico de umidade
                /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[1],umidade);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_HUMID; //  Alterna para Historico de medicoes
    			}
    			if(!b0){	 //Botao de alternar Menu pressionado
    				estado=ES_MENU2; // Alterna para proxima opcao do Menu
    			}
    			break;

            case ES_MENU2:	// Estado que exibe a medida atual de pressao e a opcao de historico de pressao
               /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[2],pressao);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_HPRES; //  Alterna para Historico de medicoes
    			}
    			if(!b0){	 //Botao de alternar Menu pressionado
    				estado=ES_MENU3; // Alterna para proxima opcao do Menu
    			}
    			break;

    		case ES_MENU3:	// Estado que exibe a medida atual de luminosidade e a opcao de historico de luminosidade
                /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[3],luminosidade);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_HLUMI;//  Alterna para Historico de medicoes
    			}
    			if(!b0){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_MENU4;// Alterna para proxima opcao do Menu
    			}
    			break;

    		case ES_MENU4:	// Estado que exibe o valor do tempo e a opcao de altera-lo
                /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPuts(lcd,menu[4]);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"->Alterar tempo");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_TIME; //  Alterna para Historico de medicoes
                    idx=0;
    			}
    			if(!b0){	  //Botao de alternar Menu pressionado
    				estado=ES_MENU0; // Alterna para proxima opcao do Menu
    			}
    			break;

            case ES_HTEMP:
               /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].temp);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].temp);

    			/*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	  // Botao que seleciona primeira opcao apertado
    				estado=ES_MENU0;  // Alterna para Menu anterior
    				idx_historico=0;  // reseta o indice do historico
    			}
    			 /*Logica adicional para configuracao do menu*/
    			if(!b0){	  //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
    			}
                break;

            case ES_HUMID:
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].umi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].umi);

                /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU1; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
            	}
                 /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_HPRES:
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].press);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].press);

              /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){     // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU2; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                }
                /*Logica adicional para configuracao do menu*/
                if(!b0){       //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_HLUMI:
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].lumi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].lumi);
                
                 /*Imprime no Display as informacoes do menu atual*/
                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado= ES_MENU3; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                }
                /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_TIME:
                 /*Imprime no Display as informacoes do menu atual*/
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
                    /* atualiza o tempo de medicao*/
                    tempo_medicao= getMilisegundos(digitos_medicao); // chama funcao que converte o tempo definido nos digitos
                }
                if(!b0){    // Alterna o digito
                    idx ==6 ? idx=0:idx++;
                }
                break;
    		default: break;
    	}

        if(!b0 || !b1 || !b2){  //Limpa o display se algum bot�o foi pressionado
            lcdClear(lcd);      
        }

    	delay(100);    // delay para a thread do display
	}
	//pthread_exit(NULL);
}