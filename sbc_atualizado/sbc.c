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
#include <time.h>
#include <mosquitto.h>

/*-------------------Variaveis globais-----------------*/
unsigned int tempo =0;
unsigned int tempoAnterior=0;
char tempoLido[]="";
unsigned int intervaloTempo=1;
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
    int rc, id=0;
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
void *displayLCD(){
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
    			lcdPrintf(lcd,"%s:%s",menu[0],temperatura);
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
    			lcdPrintf(lcd,"%s:%s",menu[1],umidade);
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
                lcdPrintf(lcd,"%d-%s", idx_historico+1,historico_display[idx_historico].temp);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%s" ,idx_historico+2,historico_display[idx_historico+1].temp);

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
                lcdPrintf(lcd,"%d-%s", idx_historico+1,historico_display[idx_historico].umi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%s" ,idx_historico+2,historico_display[idx_historico+1].umi);

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

        if(!b0 || !b1 || !b2){  //Limpa o display se algum botão foi pressionado
            lcdClear(lcd);      
        }

    	delay(100);    // delay para a thread do display
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
        leitura(&luminosidade,&pressao,temperatura,umidade,historico_display); //Realiza a leitura dos sensores
        printf("Tempo de leitura:%d milisegundos\n", tempo_medicao);
        printf("| pressao:%d | luminosidade:%d\n",pressao,luminosidade);
		sleep(intervaloTempo);
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
    getDataTempo(historico[L].data_hora_atual); // obtem data e hora da medicao
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
//---------------------------------------------------------------------------
//Funcao que verifica se foi possivel realizar a conexao com o broker e realiza a inscricao no topico de tempo caso haja.
void on_connect(struct mosquitto *mosq, void *obj, int rc){
		printf("ID: %d\n", * (int *) obj);
		if(rc){
			printf("Erro de codigo: %d\n",rc);
			exit(-1);
		}
		mosquitto_subscribe(mosq,NULL,"monitoramentoAmbiental/tempo",1);
}
//---------------------------------------------------------------------------
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg){
	printf("Nova mensagem para o topico %s: %s\n", msg->topic, (char *) msg->payload);
	//strcpy(tempoLido,(char *) msg->payload); //Copia a mensagem recebida para o intervalo de tempo
	//intervaloTempo = atoi(tempoLido);
	intervaloTempo = atoi((char *) msg->payload); //A mensagem lida é convertida em inteiro e salvo no intervalo de tempo
	
}

