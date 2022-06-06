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
//#include <MQTTClient.h>
#define MAXTIMINGS	85
#define DHTPIN		0 // Usa a GPIO 17 se no WiringPiSetup
/* Define configurações do LCD*/

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

/*
//Caso desejar utilizar um broker MQTT diferente do iot.eclipse.org, substitua o endereco abaixo pelo do broker desejado 
#define MQTT_ADDRESS   "tcp:/iot.eclipse.org"
// Substitua este por um ID unico em sua aplicacao
#define CLIENTID       "MQTTCClientID_FilipeFlop"  
 */

//Variaveis globais
int dht11_dat[5] = { 0, 0, 0, 0, 0 };
MQTTClient client;

// Declaracao das funcoes 
void leitura_dht11(char *temperatura, char *umidade); //Leitura do DHT11
void medidaLuminosidade(float *luminosidade); //Simulacao do sensor de luminosidade
void medidaPressao(float *pressao); //Simulacao do sensor de pressao atmosferica
void *exibirDisplay(char *temperatura, char *umidade,float *luminosidade,float *pressao);
int getMilisegundos(int digitos[7]);
void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void connlost(void *context, char *cause);

int main( void ){
	float pressao;
	float luminosidade;
	char temperatura[10];
	char umidade[10];
	
/*MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
   MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   MQTTClient_setCallbacks(client, NULL,connlost, on_message, NULL);
   int rc = MQTTClient_connect(client, &conn_opts);
  
   if (rc != MQTTCLIENT_SUCCESS)
   {
       printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
       exit(-1);
   }
   //Faz subscribe no topico definido em MQTT_SUBSCRIBE_TOPIC 
   MQTTClient_subscribe(client, "nomedoTopico/#, 0);
   */
	if ( wiringPiSetup() == -1 )
		exit( 1 );
 	if(openI2CBus("/dev/i2c-1") == -1){
		return EXIT_FAILURE;
	}
	setI2CSlave(0x48);
	pthread_t thread1;
	pthread_create(&thread1, NULL,exibirDisplay(&temperatura,&umidade,&luminosidade,&pressao),NULL);
	while ( 1 ){
		leitura_dht11(&temperatura,&umidade); //Ler o sensor DHT11
		printf("temperatura= %s ºC| ",temperatura); //imprime o valor da temperatura
		printf("umidade= %s | ",umidade); //imprime o valor da umidade
		
		medidaLuminosidade(&luminosidade);//Ler o sensor de Luminosidade
		printf("luminosidade= %.2f LUX | ",luminosidade); 
		
		medidaPressao(&pressao);//Ler o sensor de Pressao atmosferica
		printf("pressao = %.2f hPa \n",pressao); 
	}
 
	return(0);
}

// Funcao que realiza a leitura do sensor DHT11
void leitura_dht11(char *temperatura, char *umidade){
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
		f = dht11_dat[2] * 9. / 5. + 32;
	
		//Armazenando o valor da umidade
		sprintf(umidade, "%d.", dht11_dat[0]);
		sprintf(umidade2, "%d", dht11_dat[1]);
		strcat(umidade, umidade2);
		
		//Armazenando o valor da temperatura
		sprintf(temperatura, "%d.", dht11_dat[2]);
		sprintf(temperatura2, "%d", dht11_dat[3]);
		strcat(temperatura, temperatura2);
		
		delay( 1000 ); 

	}else  {
		//printf( "Data not good, skip\n" );
	}
}

// Funcao para simular o sensor de luminosidade BH1750
void medidaLuminosidade(float *luminosidade){
	float volts = readVoltage(0); //Ler a voltagem do potenciometro do canal 0
	*luminosidade = (65535 * volts)/3.3; //65535LUX e o valor maximo lido pelo sensor simulado
}
// Funcao para simular o sensor de pressao atmosferica BMP180
void medidaPressao(float *pressao){
	float volts = readVoltage(3); //Ler a voltagem do potenciometro do canal 3
	*pressao = (1100 * volts)/3.3; //1100pHa e o valor maximo lido pelo sensor simulado
}

//Funcao que exibe os valores lidos no display LCD
void *exibirDisplay(char *temperatura, char *umidade,float *luminosidade,float *pressao){
	int lcd;         // variavel para o display LCD
    wiringPiSetup(); // configuração do wiringPi

    // define os botões como entradas
    pinMode(B0,INPUT);
    pinMode(B1,INPUT);
    pinMode(B2,INPUT);

    //variavel do estado
    int estado=0;

    // chama a função que confira os pinos do display LCD
    lcd = lcdInit(LCD_Rows,LCD_Cols,LCD_bits,LCD_RS,LCD_E,LCD_D0,LCD_D1,LCD_D2,LCD_D3,LCD_D4,LCD_D5,LCD_D6,LCD_D7);
    lcdClear(lcd); // limpa o display

    // variveis do estado do botão
    int b0,b1,b2;
    b0=b1=b2=0; // inicializa os botões com 0

    /* variavel de controle do tempo
       {[dezena minuto]-[unidade minuto] digitos 6-5}
       {[dezena segundo]-[unidade segundo] digitos 4-3}
       {[centena milisegundo]-[dezena milisegundo]-[unidade milisegundo] digitos 2-1-0}  
    
    */
    int digitos_medicao[7] ={0}; 
    int tempo_medicao = 300000;
    int idx =0;

    // variaveis auxiliares;
    int idx_historico =0; // variavel que controla a disposição do menu dos historico
    int H_temperaturas[10]={0};
    int H_umidades[10]={0};
    int H_pressao[10]={0};
    int H_luminosidade[10]={0};

   
    // strings de texto
    char menu[5][16] = {"TEMPERATURA","UMIDADE","PRESSAO","LUMINOSIDADE","TEMPO LEITURA"};
    char subMenuTempo[4][16] = {"SEGUNDOS","MINUTOS","HORAS","VOLTAR"};
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
                case ES_MENU0:  // Estado Menu inicial exibindo opções de temperatura e historico de Temperatura
                    /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPuts(lcd,menu[0]);
                    lcdPosition(lcd,0,1);
                    lcdPuts(lcd,"-->Historico");

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_HTEMP; //  Alterna para Historico de medições
                    }
                    if(!b0){      //Botão de alternar Menu pressionado
                        estado=ES_MENU1; // Alterna para proxima opção do Menu
                    }
                break;

                case ES_MENU1:  // Estado Menu inicial exibindo opções de Umidade e historico de Umidade
                    /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPuts(lcd,menu[1]);
                    lcdPosition(lcd,0,1);
                    lcdPuts(lcd,"-->Historico");

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_HUMID; //  Alterna para Historico de medições
                    }
                    if(!b0){      //Botão de alternar Menu pressionado
                        estado=ES_MENU2; // Alterna para proxima opção do Menu
                    }
                break;

                case ES_MENU2:  // Estado Menu inicial exibindo opções de Pressao e historico de Pressao
                    /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPuts(lcd,menu[2]);
                    lcdPosition(lcd,0,1);
                    lcdPuts(lcd,"-->Historico");

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_HPRES; //  Alterna para Historico de medições
                    }
                    if(!b0){      //Botão de alternar Menu pressionado
                        estado=ES_MENU3; // Alterna para proxima opção do Menu
                    }
                break;

                case ES_MENU3:  // Estado Menu inicial exibindo opções de Luminosidade e historico de Luminosidade
                    /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPuts(lcd,menu[3]);
                    lcdPosition(lcd,0,1);
                    lcdPuts(lcd,"-->Historico");

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_HUMID; //  Alterna para Historico de medições
                    }
                    if(!b0){      //Botão de alternar Menu pressionado
                        estado=ES_MENU4; // Alterna para proxima opção do Menu
                    }
                break;

                case ES_MENU4:  // Estado Menu inicial exibindo opções de Tempo e alterar tempo
                    /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPuts(lcd,menu[4]);
                    lcdPosition(lcd,0,1);
                    lcdPuts(lcd,"->Alterar tempo");

                    /*LOGICA DE MUDANÇA DE ESTADO*/
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_TIME; //  Alterna para Historico de medições
                        idx=0;
                    }
                    if(!b0){      //Botão de alternar Menu pressionado
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
                    if(!b1){      // Botão que seleciona primeira opção apertado
                        estado=ES_MENU0;  // Alterna para Menu anterior
                        idx_historico=0;  // reseta o indice do historico
                    }
                    /*Logica adicional para configuração do menu*/
                    if(!b0){      //Botão de alternar Menu pressionado
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
                        estado= ES_MENU3; // Alterna para Menu anterior
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
                     /*Imprime no Display as informações do menu atual*/
                    lcdPosition(lcd,0,0);
                    lcdPrintf(lcd,"%d%d:",digitos_medicao[6],digitos_medicao[5]);
                    lcdPosition(lcd,3,0);
                    lcdPrintf(lcd,"%d%d:",digitos_medicao[4],digitos_medicao[3]);
                    lcdPosition(lcd,6,0);
                    lcdPrintf(lcd,"%d%d%d",digitos_medicao[2],digitos_medicao[1],digitos_medicao[0]);
                    lcdPosition(lcd,0,1);
                    lcdPrintf(lcd,"Digito:%d",idx+1);

                    if(!b1){    // Incrementa o digito
                        tempo_medicao[idx] == 9 ? idx=0: tempo_medicao[idx]++;  
                    }
                    if(!b2){    // Sai do menu de alterar tempo
                        estado = ES_MENU3;
                    }
                    if(!b0){    // Alterna o digito
                        idx ==5 ? idx=0:idx++;
                    }
                break;
                default: break;
            }
            delay(500);
        }while(TRUE);
}
/*  Função getMilisegundos
|   Recebe um vetor com os digitos referentes as grandezas de tempo e converte para milisegundos
*/
int getMilisegundos(int digitos[7]){
    return digitos[0]+ (digitos[1]*10) + (digitos[2] * 100)+
            (digitos[3]*1000) +  (digitos[4]*10000) +
            ( digitos[5]+digitos[6]*10) *60000;
}

/* Funcao: publicacao de mensagens MQTT
 * Parametros: cleinte MQTT, topico MQTT and payload
 * Retorno: nenhum
*/
/*
void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    MQTTClient_free(topic);
}


int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    // Mostra a mensagem recebida no tópico subescrito
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
//Funcao que e chamada quando a conexao é perdida
void connlost(void *context, char *cause){
    printf("\n\tConexão perdida!");
    printf("Causa: %s\n", cause);
}
*/
