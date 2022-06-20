#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "dht11.h"


/**
 * @brief A leitura do DHT11 eh feita aqui
 * 
 * @param value Int de tamanho [4]
 * @return void* 
 */
void * leitura_dht11(void * arg){

    float *fptr = (float *)arg;

    int dht11_dat[5] = {0};

    do{  
        uint8_t laststate = HIGH;
        uint8_t counter = 0;
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
                if ( counter > 16 )
                    dht11_dat[j / 8] |= 1;
                j++;
            }
        }
    
        if ( (j >= 40) &&
            (dht11_dat[4] == ( (dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF) ) )
        {

            *fptr = dht11_dat[0];
            *(fptr+1) = dht11_dat[1];
            *(fptr+2) = dht11_dat[2];
            *(fptr+3) = dht11_dat[3];
        }
        delay(500);
    }while(1);

    return dht11_dat;
}
