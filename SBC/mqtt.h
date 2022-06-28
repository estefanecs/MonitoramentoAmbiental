#ifndef _mqtt_h
#define _mqtt_h
#include <mosquitto.h>

//Definicoes de topicos e host do broker utilizado
#define Host_broker "10.0.0.101"
#define broker_temperatura "monitoramentoAmbiental/temperatura"
#define broker_umidade "monitoramentoAmbiental/umidade"
#define broker_luminosidade "monitoramentoAmbiental/luminosidade"
#define broker_pressao "monitoramentoAmbiental/pressao"
#define broker_tempo "monitoramentoAmbiental/tempo"

//Struct para cliente ouvinte mqtt
typedef struct Cliente{   
    char Nome[255];
    char Host[255];
    char Topico[255];
}Cliente;

//Struct para o cliente editor mqtt
typedef struct Publisher{   
    char Nome[255];
    char Host[255];
    char Topico[255];
    char Msg[300];
}Publisher;

//Prototipos de funcao
void publicar(Publisher pub);
void create_client(Cliente client);
void on_connect(struct mosquitto *mosq, void *obj, int rc);
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

#endif
