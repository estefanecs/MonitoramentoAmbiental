
#ifndef SBC_H
#define SBC_H

#define MAXTIMINGS	85
#define DHTPIN		0 // Usa a GPIO 17 se no WiringPiSetup
#define MAX 10 
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

typedef struct Dados{   // Dados de leituras
    int lumi;
    int press;
    char temp[10];
    char umi[10];
}Dados;

/* prototipos de função*/

	void leitura(int *luminosidade,int *pressao,char *temperatura,char *umidade,Dados *historico_display);
	void leitura_dht11(char *temperatura, char *umidade); //Leitura do DHT11
	long int maps(long int in, long int in_min,long int in_max, long int out_min, long int out_max);
	float fmap(float in, float in_min,float in_max, float out_min, float out_max);
	int getMilisegundos(int digitos[7]);
	void getOrdenada(Dados *v); 
	void add(int lum, int press, char *temp , char *umi);
	void *leituraSensores();
	void *displayLCD();


#endif
