#ifndef _mqtt_h
#define _mqtt_h

#include <mosquitto.h>

//Struct para cliente mqtt
typedef struct Cliente{   
    char Nome[255];
    char Host[255];
    char Topico[255];
}Cliente;

//Struct para publisher mqtt
typedef struct Publisher{   
    char Nome[255];
    char Host[255];
    char Topico[255];
    char Msg[255];
}Publisher;

void publicar(Publisher pub);
void create_client(Cliente client);
void on_connect(struct mosquitto *mosq, void *obj, int rc);
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

#endif