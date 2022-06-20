#include <stdio.h>
#include <mosquitto.h>
#include <string.h>
#include "mqtt.h"

#define broker_temperatura "monitoramentoAmbiental/temperatura"
#define broker_umidade "monitoramentoAmbiental/umidade"
#define broker_luminosidade "monitoramentoAmbiental/luminosidade"
#define broker_pressao "monitoramentoAmbiental/pressao"
#define broker_tempo "monitoramentoAmbiental/tempo"

#define broker_temperatura_h "monitoramentoAmbiental/historicoTemperatura"
#define broker_umidade_h "monitoramentoAmbiental/historicoUmidade"
#define broker_luminosidade_h "monitoramentoAmbiental/historicoLuminosidade"
#define broker_pressao_h "monitoramentoAmbiental/historicoPressao"

/**
 * @brief 
 * 
 * @param publisher_nome 
 * @param topico 
 * @param msg 
 */
void publicar(Publisher pub){

    int rc;
	struct mosquitto *mosq;
	mosquitto_lib_init();

	mosq = mosquitto_new(pub.Nome,true,NULL);

	mosquitto_username_pw_set(mosq,"aluno","aluno*123"); //Define usuario e senha

	rc = mosquitto_connect(mosq,pub.Host,1883,60);
	if(rc != 0){
			printf("O Publisher nao conseguiu se conectar ao broker! - {{%d}}\n", rc);
			mosquitto_destroy(mosq);
			return -1;
	}
	printf("Estamos conectados ao broker.\n");
	
	mosquitto_publish(mosq,NULL,pub.Topico,strlen(pub.Msg),pub.Msg,1,true);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	
	mosquitto_lib_cleanup();
	
	return 0;
}

/**
 * @brief 
 * 
 * @param client_nome 
 * @param topico 
 */
void create_client(Cliente Observer){


    int rc;
	
	mosquitto_lib_init();
	
	struct mosquitto *mosq;

	mosq = mosquitto_new(Observer.Nome,true,NULL);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	mosquitto_username_pw_set(mosq,"aluno","aluno*123"); //Define usuario e senha

	rc = mosquitto_connect(mosq,Observer.Host,1883,10);
	
	if(rc){
		printf("Nao foi possivel conectar ao broker - {{%d}}\n",rc);
		return -1;
	}

	mosquitto_subscribe(mosq,NULL,Observer.Topico,1);

	mosquitto_loop_start(mosq);
	printf("Aperte Enter para sair...\n");
	getchar();
	mosquitto_loop_stop(mosq,true);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
}

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
		printf("ID: %d\n", * (int *) obj);
		if(rc){
			printf("Erro de codigo - {{%d}}\n",rc);
			exit(-1);
		}
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("Nova mensagem para o topico %s: %s\n", msg->topic, (char *) msg->payload);
}