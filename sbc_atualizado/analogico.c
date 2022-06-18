#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "ads1115_rpi.h"
#include "analogico.h"
//Arquivo para funcoes dos sensores

/**
 * @brief Ler os dois sensores analogicos e retorna seus resultados normalizados
 * 
 * @return int* vetor de tamanho 2, com o resultado dos dois sensores analogicos
 */
int * leitura_analogica(){

        int sensor[2] = {0};
    /*  Realiza a leitura dos sensores*/
        sensor[0] = fmap(readVoltage(0),0,3.28,1,65535);
        sensor[1] = fmap(readVoltage(3),0,3.28,300,1100);

        return sensor;
}


//A funcao maps recebe um inteiro e mapeia de uma faixa de valores inteiros para outra faixa valores inteiro
long int maps(long int valorLido, long int minPotenciometro, float maxPotenciometro, long int minSensor, long int maxSensor){
    return (valorLido- minPotenciometro) * (maxSensor- minSensor) / (maxPotenciometro - minPotenciometro) + minSensor;
}

/* 
|   fmap recebe um numero de ponto flutuante e mapeia de uma faixa de valores de ponto flutuante
|   para outra faixa valores
*/
float fmap(float valorLido, float minPotenciometro,float maxPotenciometro, float minSensor, float maxSensor){
    return (valorLido- minPotenciometro) * (maxSensor- minSensor) / (maxPotenciometro - minPotenciometro) + minSensor;
}
