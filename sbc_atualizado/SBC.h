#ifndef sbc_H
#define sbc_H

//Struct para dados das leituras realizadas
typedef struct Dados{   
    int lumi; //luminosidade
    int press; //pressao atmosferica
    int temp; //temperatura
    int umi; //umidade
    struct tm * data_hora_atual; //data e hora atual
}Dados;

//Prototipos de funcao
int getMilisegundos(int digitos[7]);
//void getOrdenada(Dados *v); 
//void add(int lum, int press, char *temp , char *umi);
//void getDataTempo(struct tm *data);

#endif
