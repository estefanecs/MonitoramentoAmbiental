#include <wiringPi.h>
#include <lcd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ads1115_rpi.h"
/* Define configurações do LCD*/

#define LCD_Rows 2
#define LCD_Cols  16
#define LCD_bits  4
#define LCD_RS	6
#define LCD_E 	31
#define LCD_D0 26
#define LCD_D1 27
#define LCD_D2 28
#define LCD_D3 29
#define LCD_D4  0
#define LCD_D5  0
#define LCD_D6  0
#define LCD_D7  0

/* Define pinos dos botões */
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

/* prototipos de função*/
void leitura(float *umidade,float *pressao);


int main(void){
    int lcd;
    wiringPiSetup();
    // define os botões como entradas
    pinMode(B0,INPUT);
    pinMode(B1,INPUT);
    pinMode(B2,INPUT);

    //variavel do estado
    int estado=0;
    // chama a função que confira os pinos do display LCD
    lcd = lcdInit(LCD_Rows,LCD_Cols,LCD_bits,LCD_RS,LCD_E,LCD_D0,LCD_D1,LCD_D2,LCD_D3,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
    lcdClear(lcd);
    // variveis do estado do botão
    int b0,b1,b2;
    b0=b1=b2=0; // inicializa os botões com 0
    // leitura
    float pressao,umidade;
    // variaveis auxiliares;
    int idx_historico =0; // variavel que controla a disposição do menu dos historico
    int H_temperaturas[10]={0};
    int H_umidades[10]={0};
    int H_pressao[10]={0};
    int H_luminosidade[10]={0};

    float umidade,pressao;

    // strings de texto
    char menu[5][16] = {"TEMPERATURA","UMIDADE","PRESSAO","LUMINOSIDADE","TEMPO LEITURA"};
    char subMenuTempo[4][16] = {"SEGUNDOS","MINUTOS","HORAS","VOLTAR"};

    //variveis e inicialização para o conversor Analogico-Digital
        if(openI2CBus("/dev/i2c-1") == -1)  //verifica se a conexão foi feita com sucesso
        {
            return EXIT_FAILURE;
        }
        setI2CSlave(0x48);  // configura o endereço do ad1115 como slave

    	do{
            /* Realiza leitura dos botões*/
    		b0 = digitalRead(B0); // botão pressionado tem como leitura um nivél logico baixo
    		b1 = digitalRead(B1); // botão solto tem como leitura um nivél logico alto
    		b2 = digitalRead(B2);

            /*Limpa o display Lcd a cada execução vez*/
             lcdClear(lcd);


            /*  A interface é implementada como um maquina de estados usando Switch-case
            |   cada exxibição no display é considerada como um estado diferente
            |   os botões são responsavéis pela interação do usuario com a interface
            |   b0 é usado para alternar menus
            |   b1 é usado para função_1 de cada menu e submenu
            |   b2 éusado para função_2 de cada menu e submenu
            */
    		switch(estado){
    			case ES_MENU0:	// Estado Menu inicial exibindo opções de temperatura e historico de Temperatura
                    /*Imprime no Display as informações do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPuts(lcd,menu[0]);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÇA DE ESTADO*/
    				if(!b1){	  // Botão que seleciona primeira opção apertado
    					estado=ES_HTEMP; //  Alterna para Historico de medições
    				}
    				if(!b0){	  //Botão de alternar Menu pressionado
    					estado=ES_MENU1; // Alterna para proxima opção do Menu
    				}
    			break;

    			case ES_MENU1:	// Estado Menu inicial exibindo opções de Umidade e historico de Umidade
                    /*Imprime no Display as informações do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPuts(lcd,menu[1]);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÇA DE ESTADO*/
    				if(!b1){	  // Botão que seleciona primeira opção apertado
    					estado=ES_HUMID; //  Alterna para Historico de medições
    				}
    				if(!b0){	  //Botão de alternar Menu pressionado
    					estado=ES_MENU2; // Alterna para proxima opção do Menu
    				}
    			break;

                case ES_MENU2:	// Estado Menu inicial exibindo opções de Pressao e historico de Pressao
                    /*Imprime no Display as informações do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPuts(lcd,menu[2]);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÇA DE ESTADO*/
    				if(!b1){	  // Botão que seleciona primeira opção apertado
    					estado=ES_HPRES; //  Alterna para Historico de medições
    				}
    				if(!b0){	  //Botão de alternar Menu pressionado
    					estado=ES_MENU3; // Alterna para proxima opção do Menu
    				}
    			break;

    			case ES_MENU3:	// Estado Menu inicial exibindo opções de Luminosidade e historico de Luminosidade
                    /*Imprime no Display as informações do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPuts(lcd,menu[3]);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"-->Historico");

    				/*LOGICA DE MUDANÇA DE ESTADO*/
    				if(!b1){	  // Botão que seleciona primeira opção apertado
    					estado=ES_HUMID; //  Alterna para Historico de medições
    				}
    				if(!b0){	  //Botão de alternar Menu pressionado
    					estado=ES_MENU4; // Alterna para proxima opção do Menu
    				}
    			break;

    			case ES_MENU4:	// Estado Menu inicial exibindo opções de Tempo e alterar tempo
                    /*Imprime no Display as informações do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPuts(lcd,menu[4]);
    				lcdPosition(lcd,0,1);
    				lcdPuts(lcd,"->Alterar tempo");

    				/*LOGICA DE MUDANÇA DE ESTADO*/
    				if(!b1){	  // Botão que seleciona primeira opção apertado
    					estado=ES_TIME; //  Alterna para Historico de medições
    				}
    				if(!b0){	  //Botão de alternar Menu pressionado
    					estado=ES_MENU0; // Alterna para proxima opção do Menu
    				}
    			break;

                case ES_HTEMP:
                    /*Imprime no Display as informações do menu atual*/
    				lcdPosition(lcd,0,0);
    				lcdPrintf(lcd,"%d-%d", idx_historico+1,H_temperaturas[idx_historico]);
    				lcdPosition(lcd,0,1);
    				lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_temperaturas[idx_historico+1]);

    				/*LOGICA DE MUDANÇA DE ESTADO*/
    				if(!b1){	  // Botão que seleciona primeira opção apertado
    					estado=ES_MENU0;  // Alterna para Menu anterior
    					idx_historico=0;  // reseta o indice do historico
    				}
    				/*Logica adicional para configuração do menu*/
    				if(!b0){	  //Botão de alternar Menu pressionado
                        		if(idx_historico==8)
                        			idx_historico=0;
                        		else
                        			idx_historico+=2;
    				}
                break;

                case ES_HUMID:
                	/*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d-%d", idx_historico+1,H_umidades[idx_historico]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_umidades[idx_historico+1]);

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_MENU1; // Alterna para Menu anterior
                        idx_historico=0; // reseta o indice do historico
                    }
                    /*Logica adicional para configuração do menu*/
                    if(!b0){      //Botão de alternar Menu pressionado
                                if(idx_historico==8)
                                    idx_historico=0;
                                else
                                    idx_historico+=2;
                    }
                break;

                case ES_HPRES:
                    /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d-%d", idx_historico+1,H_pressao[idx_historico]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_pressao[idx_historico+1]);

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_MENU2; // Alterna para Menu anterior
                        idx_historico=0; // reseta o indice do historico
                    }
                    /*Logica adicional para configuração do menu*/
                    if(!b0){      //Botão de alternar Menu pressionado
                                if(idx_historico==8)
                                    idx_historico=0;
                                else
                                    idx_historico+=2;
                    }
                break;

                case ES_HLUMI:
                    /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d-%d", idx_historico+1,H_luminosidade[idx_historico]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"%d-%d" ,idx_historico+2,H_luminosidade[idx_historico+1]);

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_MENU3; // Alterna para Menu anterior
                        idx_historico=0; // reseta o indice do historico
                    }
                    /*Logica adicional para configuração do menu*/
                    if(!b0){      //Botão de alternar Menu pressionado
                                if(idx_historico==8)
                                    idx_historico=0;
                                else
                                    idx_historico+=2;
                    }
                break;

                case ES_TIME:
                break;
    			default: break;
    		}
    		delay(500);
    	}while(TRUE);
    return 0;
}

void leitura(float *umidade,float *pressao);{
        *umidade= readVoltage(0);
        *pressao= readVoltage(3);
}