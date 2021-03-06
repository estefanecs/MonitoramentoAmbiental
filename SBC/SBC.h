#ifndef SBC_H
#define SBC_H

#define MAXTIMINGS	85
#define DHTPIN		0 // Usa a GPIO 17 no WiringPiSetup
#define MAX 10 

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

/*Define estados da maquina de estados*/
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

//Struct para dados das leituras realizadas
typedef struct Dados{   
    int lumi; //luminosidade
    int press; //pressao atmosferica
    char temp[10]; //temperatura
    char umi[10]; //umidade
    char timeStamp[40];; //data e hora atual
}Dados;


//Prototipos de funcao
void leitura(int *luminosidade,int *pressao,char *temperatura,char *umidade,Dados *historico_display, char *dataHora);
void leitura_dht11(char *temperatura, char *umidade);
float fmap(float valorLido, float minPotenciometro,float maxPotenciometro, float minSensor, float maxSensor);
int getMilisegundos(int digitos[7]);
void getOrdenada(Dados *v); 
float getSegundos(int digitos[7]);
void add(int lum, int press, char *temp , char *umi,char *timeStamp);
void *leituraSensores();
void *displayLCD();
void getDataTempo(char *data);
void * PublicarValores();
void LerIntervaloTempo();
void escrever_arquivo(Dados *data);
void ler_arquivo();
void split(char *str,char *dest1,char *dest2, char token);
#endif
