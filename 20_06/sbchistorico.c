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
#include "mqtt.h"
#include <mosquitto.h>

//Definicao dos topicos e o host do broker MQTT
#define Host_broker "10.0.0.101"
#define broker_temperatura "monitoramentoAmbiental/temperatura"
#define broker_umidade "monitoramentoAmbiental/umidade"
#define broker_luminosidade "monitoramentoAmbiental/luminosidade"
#define broker_pressao "monitoramentoAmbiental/pressao"
#define broker_tempo "monitoramentoAmbiental/tempo"

#define broker_temperatura_h "monitoramentoAmbiental/historicoTemperatura"
#define broker_umidade_h "monitoramentoAmbiental/historicoUmidade"
#define broker_luminosidade_h "monitoramentoAmbiental/historicoLuminosidade"
#define broker_pressao_h "monitoramentoAmbiental/historicoPressao"

/*-------------------Variaveis globais-----------------*/
unsigned int tempo =0;
unsigned int tempoAnterior=0;
char tempoLido[]="";
float intervaloTempo=1.0;
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
        historico[i].lumi = 0;
        historico[i].press= 0;
        strcpy(historico[i].temp,"-");
        strcpy(historico[i].umi,"-");
    
        historico_display[i].lumi= 0;
        historico_display[i].press= 0;
        strcpy(historico_display[i].temp,"-");
        strcpy(historico_display[i].umi,"-");
    }

    pthread_t threadSensores, interface, pubT;

    pthread_create(&threadSensores, NULL,leituraSensores,NULL); //criacao da thread para leitura dos sensores
    pthread_create(&interface,NULL,displayLCD,NULL); //Criacao da thread para exibir dados no display LCD
    pthread_create(&pubT,NULL, PublicarValores, NULL); //Criacao da thread para publicar os dados no broker
    LerIntervaloTempo(); //Realiza a leitura do intervalo de tempo enviado pela interface desktop
        
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
            case ES_MENU0:  // Estado que exibe a medida atual da temperatura e a opcao de historico de temperatura
                 /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%s:%s",menu[0],temperatura);
                lcdPosition(lcd,0,1);
                lcdPuts(lcd,"-->Historico");
                
                /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado=ES_HTEMP; //  Alterna para Historico de medicoes
                }
                if(!b0){      //Botao de alternar Menu pressionado
                    estado=ES_MENU1; // Alterna para proxima opcao do Menu
                }
                break;

            case ES_MENU1:  // Estado que exibe a medida atual de umidade e a opcao de historico de umidade
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%s:%s",menu[1],umidade);
                lcdPosition(lcd,0,1);
                lcdPuts(lcd,"-->Historico");

                 /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){     // Botao que seleciona primeira opcao apertado
                    estado=ES_HUMID; //  Alterna para Historico de medicoes
                }
                if(!b0){     //Botao de alternar Menu pressionado
                    estado=ES_MENU2; // Alterna para proxima opcao do Menu
                }
                break;

            case ES_MENU2:  // Estado que exibe a medida atual de pressao e a opcao de historico de pressao
               /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%s:%d",menu[2],pressao);
                lcdPosition(lcd,0,1);
                lcdPuts(lcd,"-->Historico");

                 /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){     // Botao que seleciona primeira opcao apertado
                    estado=ES_HPRES; //  Alterna para Historico de medicoes
                }
                if(!b0){     //Botao de alternar Menu pressionado
                    estado=ES_MENU3; // Alterna para proxima opcao do Menu
                }
                break;

            case ES_MENU3:  // Estado que exibe a medida atual de luminosidade e a opcao de historico de luminosidade
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%s:%d",menu[3],luminosidade);
                lcdPosition(lcd,0,1);
                lcdPuts(lcd,"-->Historico");

                 /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){     // Botao que seleciona primeira opcao apertado
                    estado=ES_HLUMI;//  Alterna para Historico de medicoes
                }
                if(!b0){     // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU4;// Alterna para proxima opcao do Menu
                }
                break;

            case ES_MENU4:  // Estado que exibe o valor do tempo e a opcao de altera-lo
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%s:%.3f",menu[4],intervaloTempo);
                lcdPosition(lcd,0,1);
                lcdPuts(lcd,"->Alterar tempo");

                 /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){     // Botao que seleciona primeira opcao apertado
                    estado=ES_TIME; //  Alterna para Historico de medicoes
                    idx=0;
                }
                if(!b0){      //Botao de alternar Menu pressionado
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
                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU0;  // Alterna para Menu anterior
                    idx_historico=0;  // reseta o indice do historico
                }
                 /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
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
                    intervaloTempo= getSegundos(digitos_medicao); // chama funcao que converte o tempo definido nos digitos
                }
                if(!b0){    // Alterna o digito
                    idx ==6 ? idx=0:idx++;
                }
                break;
            default: break;
        }

        if(!b0 || !b1 || !b2){  //Limpa o display se algum bot√£o foi pressionado
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
        printf("Tempo de leitura:%.3f segundos\n", intervaloTempo);
        printf("| pressao: %d | luminosidade: %d\n",pressao,luminosidade);
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
//--------------------------------------------------------------------------
// Funcao que recebe um vetor com os digitos referentes as grandezas de tempo e converte para milisegundos.
float getSegundos(int digitos[7]){
    return (digitos[0]+ (digitos[1]*10.0) + (digitos[2] * 100.0) /1000.0)+
            (digitos[3]) +  (digitos[4]*10) +
            ( digitos[5]+digitos[6]*10) *60;
}
//--------------------------------------------------------------------------
//Funcao que realiza a leitura do intervalo de tempo, enviado pela interface Desktop
void LerIntervaloTempo(){
    Cliente clientTempo; 
    clientTempo = (Cliente){.Nome = "Leitor de intevalos - Client",.Host = Host_broker,.Topico = broker_tempo}; //Configura os parametros
    create_client(clientTempo); //Cria o cliente
    
}
//--------------------------------------------------------------------------
//Funcao que realiza a publicacao no broker, dos dados medidos e historico.
void * PublicarValores(){
	//Publicadores para as medicoes e para o intervalo de tempo
    Publisher pubTempo = (Publisher){.Nome = "SBC Tempo - Pub",.Host = Host_broker,.Topico = broker_tempo,.Msg = ""};
   	Publisher  pubPres = (Publisher){.Nome = "ads1115 Pressao - Pub",.Host = Host_broker,.Topico = broker_pressao,.Msg = ""};
    Publisher pubLumi = (Publisher){.Nome = "ads1115 Luminosidade - Pub",.Host = Host_broker,.Topico = broker_luminosidade,.Msg = ""};
    Publisher pubUmid = (Publisher){.Nome = "dht11 Umidade - Pub",.Host = Host_broker,.Topico = broker_umidade,.Msg = ""};
    Publisher pubTemp = (Publisher){.Nome = "dht11 Temperatura - Pub",.Host = Host_broker,.Topico = broker_temperatura,.Msg = ""};
    
    //Publicadores para os historicos das medicoes
    Publisher historicoPres = (Publisher){.Nome = "Historico Pressao - Pub",.Host = Host_broker,.Topico = broker_pressao_h,.Msg = ""};
    Publisher historicoLumi = (Publisher){.Nome = "Historico Luminosidade - Pub",.Host = Host_broker,.Topico = broker_luminosidade_h,.Msg = ""};
    Publisher historicoUmid = (Publisher){.Nome = "Historico Umidade - Pub",.Host = Host_broker,.Topico = broker_umidade_h,.Msg = ""};
    Publisher historicoTemp = (Publisher){.Nome = "Historico Temperatura - Pub",.Host = Host_broker,.Topico = broker_temperatura_h,.Msg = ""};
    
    char tempo[255]="",pressao1[255]="",luminosidade1[255]="",luminosidade_h[300]="",temperatura_h[300]="",umidade_h[300]="",pressao_h[300]="";
    do{
    	//Converte os dados a serem enviados em string
        sprintf(tempo,"%f",intervaloTempo);
        sprintf(pressao1,"%d",pressao);
        sprintf(luminosidade1,"%d",luminosidade);
        stringHistorico(luminosidade_h,temperatura_h,umidade_h,pressao_h);

		//Copia os dados para o escopo de mensagem dos publicadores
        strcpy(pubPres.Msg,pressao1);
        strcpy(pubLumi.Msg,luminosidade1);
        strcpy(pubUmid.Msg,umidade);
        strcpy(pubTemp.Msg,temperatura);
        strcpy(pubTempo.Msg,tempo);
        strcpy(historicoPres.Msg,pressao_h);
        strcpy(historicoLumi.Msg,luminosidade_h);
        strcpy(historicoUmid.Msg,umidade_h);
        strcpy(historicoTemp.Msg,temperatura_h);

		//Realiza a publicacao das mensagens no broker
        publicar(pubTempo);
        publicar(pubPres);
        publicar(pubLumi);
        publicar(pubUmid);
        publicar(pubTemp);
        publicar(historicoPres);
        publicar(historicoLumi);
        publicar(historicoUmid);
        publicar(historicoTemp);
        
        sleep(intervaloTempo);
    }while(1);
    pthread_exit(NULL); //Encerra a thread
}
//--------------------------------------------------------------------------
//Funcao que realiza a criacao da string do historico
void stringHistorico(char *luminosidades, char *temperaturas, char *umidades,char *pressoes){
	int i =0;
	char aux[300]="";
	char dia[20]="", mes[10]="", ano[10]="";
	char hora[20]="", min[10]="", seg[10]="";
	char data[25]="";
	
	for(i=0; i<10;i++){
		
		sprintf(dia," %d/", historico_display[i].data_hora_atual->tm_mday); //dia
	  	sprintf(mes,"%d/",historico_display[i].data_hora_atual->tm_mon+1); //mÍs
	  	sprintf(ano,"%d",historico_display[i].data_hora_atual->tm_year+1900); //ano
	  	strcat(dia,mes);
	  	strcat(dia,ano);
			
		sprintf(hora," %d:",historico_display[i].data_hora_atual->tm_hour);//hora   
	  	sprintf(min, "%d:",historico_display[i].data_hora_atual->tm_min);//minuto
	  	sprintf(seg,"%d-",historico_display[i].data_hora_atual->tm_sec);//segundo  
	  	strcat(hora,min);
	  	strcat(hora,seg);
	  
	  	strcpy(data,dia);
	  	strcat(data,hora);
  		
		//Converte o historico de luminosidade para string e concatena
		sprintf(aux, "%d",historico_display[i].lumi);
		strcat(aux,data);
		strcat(luminosidades, aux);
		
		//Concatena o historico de temperatura
		strcpy(aux,historico_display[i].temp);
		strcat(aux,data);
		strcat(temperaturas, aux);
		
		//Concatena o historico de umidade
		strcpy(aux,historico_display[i].umi);
		strcat(aux,data);
		strcat(umidades, aux);
		
		//Converte o historico de pressao para string e concatena
		sprintf(aux, "%d",historico_display[i].press);
		strcat(aux,data);
		strcat(pressoes, aux);
	}
}
