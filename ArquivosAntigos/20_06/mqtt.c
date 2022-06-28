#include <stdio.h>
#include <mosquitto.h>
#include <string.h>
#include <stdlib.h>
#include "mqtt.h"


// Topicos utilizados pelo nosso programa
#define broker_temperatura "monitoramentoAmbiental/temperatura"
#define broker_umidade "monitoramentoAmbiental/umidade"
#define broker_luminosidade "monitoramentoAmbiental/luminosidade"
#define broker_pressao "monitoramentoAmbiental/pressao"
#define broker_tempo "monitoramentoAmbiental/tempo"

#define broker_temperatura_h "monitoramentoAmbiental/historicoTemperatura"
#define broker_umidade_h "monitoramentoAmbiental/historicoUmidade"
#define broker_luminosidade_h "monitoramentoAmbiental/historicoLuminosidade"
#define broker_pressao_h "monitoramentoAmbiental/historicoPressao"

struct mosquitto_message *destino;

extern float intervaloTempo; 

/**
 * @brief Recebe uma struct do tipo Publisher e utiliza as informações contidas na struct para publicar uma mensagem.
 * 
 * @param pub 
 */
void publicar(Publisher pub){

    int rc;
	struct mosquitto *mosq;
	mosquitto_lib_init();

	mosq = mosquitto_new(pub.Nome,true,NULL);

	mosquitto_username_pw_set(mosq,"aluno","aluno*123"); //Define usuario e senha

	rc = mosquitto_connect(mosq,pub.Host,1883,60);
	if(rc != 0){
			printf("O %s nao conseguiu se conectar ao broker! - {{%d}}\n", pub.Nome, rc);
			mosquitto_destroy(mosq);
			return ;
	}
	printf("%s conectou-se ao broker.\n",pub.Nome);
	
	mosquitto_publish(mosq,NULL,pub.Topico,strlen(pub.Msg),pub.Msg,1,true);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	
	mosquitto_lib_cleanup();
	
	return ;
}

/**
 * @brief Cria um client, utilizando os valores previamente colocados numa struct do tipo Cliente.
 * Utiliza essas dados para se conectar a um tópico e receber suas mensagens.
 * 
 * @param Observer 
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
		printf("O %s nao conseguiu conectar ao broker - {{%d}}\n", Observer, rc);
		return ;
	}

	mosquitto_subscribe(mosq,NULL,Observer.Topico,1);

	mosquitto_loop_start(mosq);
	getchar();
	mosquitto_loop_stop(mosq,true);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return;
}
/**
 * @brief Callback para caso a nossa sessão mosq se conecte em algum broker. Caso sim, essa função é chamada.
 * Deve ser definida anteriormente utilizando o mosquitto_connect_callback_set(sessao_do_Mosquito, Nome_da_função_Callback);
 * 
 * @param mosq 
 * @param obj 
 * @param rc 
 */
void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
		printf("ID: %d\n", * (int *) obj);
		if(rc){
			printf("Erro de codigo - {{%d}}\n",rc);
			exit(-1);
		}

	return;
}

/**
 * @brief Callback para caso a nossa sessão mosq esteja inscrita em algum tópico, e receba alguma mensagem, essa função é chamada.
 *  Deve ser definida anteriormente utilizando o mosquitto_message_callback_set(sessao_do_Mosquito, Nome_da_função_Callback);
 * @param mosq 
 * @param obj 
 * @param msg 
 */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	intervaloTempo = atof((char *) msg->payload);
	return;
}
