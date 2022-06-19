#ifndef SBC_H
#define SBC_H

#define MAXTIMINGS	85
#define DHTPIN		0 // Usa a GPIO 17 no WiringPiSetup
#define MAX 10 



//Struct para dados das leituras realizadas
typedef struct Dados{   
    int lumi; //luminosidade
    int press; //pressao atmosferica
    int temp; //temperatura
    int umi; //umidade
    struct tm * data_hora_atual; //data e hora atual
}Dados;

//Prototipos de funcao
void leitura(int *luminosidade,int *pressao,char *temperatura,char *umidade,Dados *historico_display);
int getMilisegundos(int digitos[7]);
void getOrdenada(Dados *v); 
void add(int lum, int press, char *temp , char *umi);
void *leituraSensores();
void *displayLCD();
void getDataTempo(struct tm *data);

#endif
