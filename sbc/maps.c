#include <stdio.h>

/*
|    maps.c
|    Possui duas funções declaradas que mapeam um intervalo numerico em outro
*/

/*maps recebe um inteiro e mapeia de uma faixa de valores inteiros para outra faixa valores inteiro*/
long int maps(long int in, long int in_min,long int in_max, long int out_min, long int out_max);
/*fmp recebe um número de ponto flutuante e mapeia de uma faixa de valores de ponto flutuante
    para outra faixa valores
*/
float fmap(float in, float in_min,float in_max, float out_min, float out_max);

/* exemplo de utilziação*/
void main(){
    float d;
    int i;

    char caso = 'e'; // escolha caso como i- inteiro ou f- decimal

        if( caso =='i')
            for(i=0;i<=10;i++)
                printf("in:%d  out:%d\n",i,maps(i,0,10,0,100));
        else if (caso =='f')
            for(d=0;d<=15.78;d += 0.75)
                printf("in:%.2f  out:%.2f\n",d,fmap(d,0,10.0,0,24.25));
        else
            printf("ERRO!!\n -- Defina caso como i ou f\n --   ENCERRADO   --");
}

long int maps(long int in, long int in_min,long int in_max, long int out_min, long int out_max){
    return (in- in_min) * (out_max- out_min) / (in_max - in_min) + out_min;
}

float fmap(float in, float in_min,float in_max, float out_min, float out_max){
    return (in- in_min) * (out_max- out_min) / (in_max - in_min) + out_min;
}
