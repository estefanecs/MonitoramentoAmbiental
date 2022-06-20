#ifndef _analogico_h
#define _analogico_h

void * leitura_analogica(void * arg);

long int maps(long int valorLido, long int minPotenciometro, float maxPotenciometro, long int minSensor, long int maxSensor);

float fmap(float valorLido, float minPotenciometro,float maxPotenciometro, float minSensor, float maxSensor);

#endif