/*
* Includes
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>
 
/*
* Defines
*/
/* Caso desejar utilizar um broker MQTT diferente do iot.eclipse.org, substitua o endereco abaixo pelo do broker desejado */
#define MQTT_ADDRESS   "tcp://iot.eclipse.org"
/* Substitua este por um ID unico em sua aplicacao */
#define CLIENTID       "MQTTCClientID_FilipeFlop"  
 
/* Substitua aqui os topicos de publish e subscribe por topicos exclusivos de sua aplicacao */
#define MQTT_PUBLISH_TOPIC     "MQTTCClientPubTopic"
#define MQTT_SUBSCRIBE_TOPIC   "MQTTCClientSubTopic"
 
/*
*  Variaveis globais
*/
MQTTClient client;
 
/*
* Prototipos de funcao
*/
void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
 
/*
* Implementacoes
*/
 
/* Funcao: publicacao de mensagens MQTT
 * Parametros: cleinte MQTT, topico MQTT and payload
 * Retorno: nenhum
*/
void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
 
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}
 
/* Funcao: callback de mensagens MQTT recebidas e echo para o broker
 * Parametros: contexto, ponteiro para nome do topico da mensagem recebida, tamanho do nome do topico e mensagem recebida
 * Retorno : 1: sucesso (fixo / nao ha checagem de erro neste exemplo)
*/
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
 
    /* Mostra a mensagem recebida no t�pico subescrito*/
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);
 
    /* Faz echo da mensagem recebida no t�pico de publish */
    publish(client, MQTT_PUBLISH_TOPIC, payload);
 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
 
int main(int argc, char *argv[])
{
   int rc;
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
 
   /* Inicializacao do MQTT e configu��o de funcao de callback MQTT */
   MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
 
   /* Conecta-se ao broker (e sai do programa caso a conexao venha a falhar) */
   rc = MQTTClient_connect(client, &conn_opts);
  
   if (rc != MQTTCLIENT_SUCCESS)
   {
       printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
       exit(-1);
   }
 
   /* Faz subscribe no topico definido em MQTT_SUBSCRIBE_TOPIC */
   MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC, 0);
 
 
  		
   /* Programa principal */
   while(1)
   {
   	publish(client, MQTT_SUBSCRIBE_TOPIC, "helllo");
       /*
        * este client opera por "interrupcao", ou seja, opera em fun��o do que � recebido no callback de recepcao de 
        * mensagens MQTT. Portanto, neste laco principal nao eh preciso fazer nada.
        */
   }
}
