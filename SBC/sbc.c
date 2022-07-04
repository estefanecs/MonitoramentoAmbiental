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

/*-------------------Variaveis globais-----------------*/
unsigned int tempo =0;
unsigned int tempoAnterior=0;
char tempoLido[]="";
float intervaloTempo=2.0;
char temperatura[10]={"0"};
char dataHora[40]={"0"};
char umidade[10]={"0"}; 
int  pressao=0,luminosidade=0;
//Variaveis de historico das medicoes
Dados historico[MAX]; 
Dados historico_display[MAX];
//Variaveis de controle da lista do historico
int L=-1;
int O=-1;
char luminosidade_h[300]="",temperatura_h[300]="",umidade_h[300]="",pressao_h[300]="";
/*----------------------------------------------------*/


int main(void){
    wiringPiSetup(); //Configuracao do wiringPi
    if(openI2CBus("/dev/i2c-1") == -1){ //Verifica se houve falha ao mapear o I2C
        return EXIT_FAILURE;
    }
    setI2CSlave(0x48); //Ativa a configuracao do I2C
    
    ler_arquivo();
    getOrdenada(historico_display);
    pthread_t threadSensores, interface, pubT;

    pthread_create(&threadSensores, NULL,leituraSensores,NULL); //criacao da thread para leitura dos sensores
    pthread_create(&interface,NULL,displayLCD,NULL); //Criacao da thread para exibir dados no display LCD
    pthread_create(&pubT,NULL, PublicarValores, NULL); //Criacao da thread para realizar a publicacao dos dados no broker
    LerIntervaloTempo();
        
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
    int idx_timeStamp=0;
    int b0=0,b1=0,b2=0;//variaveis do estado do botao
    int idx =0; // variavel para indice do historico
    int idx_historico =0; // variavel que controla a disposicao do menu dos historico
    char menu[5][16] = {"TEMP","UMI","PRES","LUMI","TEMPO"}; //String de texto para exibir no display
    char data[13],hora[13];
    char buff[13];
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
                lcdPrintf(lcd,"%s:%sC",menu[0],temperatura);
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
                lcdPrintf(lcd,"%s:%s%%",menu[1],umidade);
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
                lcdPrintf(lcd,"%s:%dbph",menu[2],pressao);
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
                lcdPrintf(lcd,"%s:%dlux",menu[3],luminosidade);
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
		split(historico_display[idx_historico].timeStamp,data,hora,'H');
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%sC", idx_historico+1,historico_display[idx_historico].temp);  // Imprime dado no display
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%s",buff); // imprime data ou hora
            
                /*LOGICA DE MUDANCA DE ESTADO*/

                if(!b2){
                    if(idx_timeStamp ==0){
                        idx_timeStamp=1;
                        strcpy(buff,data);
                    }
                    else if(idx_timeStamp == 1){
                        idx_timeStamp=0;
                        strcpy(buff,hora);
                    }
                }

                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU0;  // Alterna para Menu anterior
                    idx_historico=0;  // reseta o indice do historico
                    idx_timeStamp=0;
                }
                 /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
                    if(idx_historico ==9)
                        idx_historico=0;
                    else
                        idx_historico++;
                    
                }
                
                break;

            case ES_HUMID:
                /*Imprime no Display as informacoes do menu atual*/
		split(historico_display[idx_historico].timeStamp,data,hora,'H');
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%s%%", idx_historico+1,historico_display[idx_historico].umi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%s",buff); // imprime data ou hora

                /*LOGICA DE MUDANCA DE ESTADO*/

                if(!b2){
                    if(idx_timeStamp ==0){
                        idx_timeStamp=1;
                        strcpy(buff,data);
                    }
                    else if(idx_timeStamp == 1){
                        idx_timeStamp=0;
                        strcpy(buff,hora);
                    }
                }

                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU1; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                    idx_timeStamp=0;
                }
                 /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
                    if(idx_historico==9)
                        idx_historico=0;
                    else
                        idx_historico++;
                   
                }
                break;

            case ES_HPRES:
                /*Imprime no Display as informacoes do menu atual*/
		split(historico_display[idx_historico].timeStamp,data,hora,'H');
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%dbph", idx_historico+1,historico_display[idx_historico].press);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%s",buff); // imprime data ou hora

              /*LOGICA DE MUDANCA DE ESTADO*/

                if(!b2){
                    if(idx_timeStamp ==0){
                        idx_timeStamp=1;
                        strcpy(buff,data);
                    }
                    else if(idx_timeStamp == 1){
                        idx_timeStamp=0;
                        strcpy(buff,hora);
                    }
                }

                if(!b1){     // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU2; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                    idx_timeStamp=0;
                }
                /*Logica adicional para configuracao do menu*/
                if(!b0){       //Botao de alternar Menu pressionado
                    if(idx_historico==9)
                        idx_historico=0;
                    else
                        idx_historico++;
                   
                }
                break;

            case ES_HLUMI:
                /*Imprime no Display as informacoes do menu atual*/
		split(historico_display[idx_historico].timeStamp,data,hora,'H');
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%dlux", idx_historico+1,historico_display[idx_historico].lumi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%s",buff); // imprime data ou hora
                
            /*LOGICA DE MUDANCA DE ESTADO*/
               if(!b2){
                    if(idx_timeStamp ==0){
                        idx_timeStamp=1;
                        strcpy(buff,data);
                    }
                    else if(idx_timeStamp == 1){
                        idx_timeStamp=0;
                        strcpy(buff,hora);
                    }
                }

                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado= ES_MENU3; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                }
                /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
                    if(idx_historico==9)
                        idx_historico=0;
                    else
                        idx_historico++;
                    
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
			/*Limpa digitos*/
			digitos_medicao[0]=0;
			digitos_medicao[1]=0;
			digitos_medicao[2]=0;
			digitos_medicao[3]=0;
			digitos_medicao[4]=0;
			digitos_medicao[5]=0;
			digitos_medicao[6]=0;
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

//---------------------------------------------------------------------------
//Funcao para leitura dos sensores 
void *leituraSensores(){
    while(1){
        leitura(&luminosidade,&pressao,temperatura,umidade,historico_display,dataHora); //Realiza a leitura dos sensores
        printf("Tempo de leitura:%.3f segundos\n", intervaloTempo);
        printf("| pressao: %d | luminosidade: %d\n",pressao,luminosidade);
        escrever_arquivo(historico_display); //Escreve no arquivo o historico atual
        sleep(intervaloTempo);
    }
    pthread_exit(NULL); //Encerra a thread
}

//--------------------------------------------------------------------------
//Adiciona uma nova item(medida) no historico. Quando a lista possuir 10 itens, o mais antigo e sobrescrito.
void add(int lum, int press, char *temp , char *umi,char *timeStamp){
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
    strcpy(historico[L].timeStamp,timeStamp);
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
//Funcao que cria o cliente mqtt para obter o intervalo de tempo de medicao
void LerIntervaloTempo(){
    Cliente clientTempo;
    clientTempo = (Cliente){.Nome = "Leitor de intevalos - Client",.Host = Host_broker,.Topico = broker_tempo};
    create_client(clientTempo);
    
}

//--------------------------------------------------------------------------
// Funcao que ler o historico do arquivo
void ler_arquivo(){
    FILE *arq; // ponteiro para o arquivo
    arq = fopen("historico.txt", "r"); //Cria o arquivo para leitura
    Dados aux;
    int i=0;
    char buff[100];
        
        if(arq == NULL){ //Verifica se nao conseguiu abrir o arquivo
            printf("Erro na abertura");
        exit(1);
        }
        
        //Realiza a leitura no arquivo
        for(i=0;i<MAX;i++){
            fgets(buff,100,arq);
            fscanf(arq,"Temperatura=%s\n",aux.temp);
            fscanf(arq,"Umidade=%s\n",aux.umi);
            fscanf(arq,"Luminosidade=%d\n",&aux.lumi);
            fscanf(arq,"Pressao=%d\n",&aux.press);
            fgets(buff,40,arq);
            strcpy(aux.timeStamp,buff);
            add(aux.press,aux.lumi,aux.temp,aux.umi,aux.timeStamp);
        }
        printf("\nFinalizada leitura do arquivo\n");
    fclose(arq); 
}

//--------------------------------------------------------------------------
// Funcao que escreve o historico no arquivo
void escrever_arquivo(Dados *data){
    FILE *arq; // ponteiro para o arquivo
    arq = fopen("historico.txt","w+"); //Cria arquivo para escrita
    int i=0;
    if(arq == NULL){//Verifica se nao conseguiu abrir o arquivo
        printf("Erro na abertura");
        exit(1);
    }
    printf("\nComecei escrita do arquivo\n");
    
    //Realiza a escrita no arquivo
    for(i=0;i<MAX;i++){
        fputs("---------------\n",arq);
        fprintf(arq,"Temperatura=%s\n",data[i].temp);
        fprintf(arq,"Umidade=%s\n",data[i].umi);
        fprintf(arq,"Luminosidade=%d\n",data[i].lumi);
        fprintf(arq,"Pressao=%d\n",data[i].press);
        fprintf(arq,"%s",data[i].timeStamp);

    }
    printf("Finalizada escrita do arquivo\n");
    fclose(arq);
}

//--------------------------------------------------------------------------
//Funcao que realiza a publicacao no broker, dos dados medidos e historico.
void * PublicarValores(){
    //Publicadores para as medicoes e para o intervalo de tempo
    Publisher pubTempo;
    pubTempo = (Publisher){.Nome = "SBC Tempo - Pub",.Host = Host_broker,.Topico = broker_tempo,.Msg = ""};
    Publisher pubPres;
    pubPres = (Publisher){.Nome = "ads1115 Pressao - Pub",.Host = Host_broker,.Topico = broker_pressao,.Msg = ""};
    Publisher pubLumi;
    pubLumi = (Publisher){.Nome = "ads1115 Luminosidade - Pub",.Host = Host_broker,.Topico = broker_luminosidade,.Msg = ""};
    Publisher pubUmid;
    pubUmid = (Publisher){.Nome = "dht11 Umidade - Pub",.Host = Host_broker,.Topico = broker_umidade,.Msg = ""};
    Publisher pubTemp;
    pubTemp = (Publisher){.Nome = "dht11 Temperatura - Pub",.Host = Host_broker,.Topico = broker_temperatura,.Msg = ""};
    
    char tempo[255]="",pressao1[255]="",luminosidade1[255]="",temperatura1[60]="", umidade1[60]="";
    do{
        //Converte os dados a serem enviados em string
        sprintf(tempo,"%f",intervaloTempo);
        sprintf(pressao1,"%d- ",pressao);
        sprintf(luminosidade1,"%d- ",luminosidade);
        
        //Concatena os dados das medicoes com a data/hora em que foram realizadas
	strcat(pressao1,dataHora);
	strcat(luminosidade1,dataHora);	
	strcpy(temperatura1, temperatura);
	strcpy(umidade1,umidade);
	strcat(temperatura1, "- ");
	strcat(umidade1,"- ");
	strcat(temperatura1,dataHora);
	strcat(umidade1,dataHora);

        //Copia os dados para o escopo de mensagem dos publicadores
        strcpy(pubPres.Msg,pressao1);
        strcpy(pubLumi.Msg,luminosidade1);
        strcpy(pubUmid.Msg,umidade1);
        strcpy(pubTemp.Msg,temperatura1);
        strcpy(pubTempo.Msg,tempo);

        //Realiza a publicacao das mensagens no broker
        publicar(pubTempo);
        publicar(pubPres);
        publicar(pubLumi);
        publicar(pubUmid);
        publicar(pubTemp);

        sleep(1);
    }while(1);
    pthread_exit(NULL); //Encerra a thread
}

//--------------------------------------------------------------------------
//Funcao que realiza a separacao de uma string em vetores, dado uma marcacao de separacao
void split(char *str,char *dest1,char *dest2, char token){
    int aux=0;
    int idx=0;  

        while(str[idx] != token){
            dest1[aux] = str[idx];
            aux++;
            idx++;
        }
        aux=0;
        while(str[idx]!='\0'){
            dest2[aux]=str[idx];
            aux++;
            idx++;
        }
        
}
