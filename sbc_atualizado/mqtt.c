#include <stdio.h>
#include <mosquitto.h>
#include <string.h>

#define broker_temperatura "monitoramentoAmbiental/temperatura"
#define broker_umidade "monitoramentoAmbiental/umidade"
#define broker_luminosidade "monitoramentoAmbiental/luminosidade"
#define broker_pressao "monitoramentoAmbiental/pressao"
#define broker_tempo "monitoramentoAmbiental/tempo"

#define broker_temperatura_h "monitoramentoAmbiental/historicoTemperatura"
#define broker_umidade_h "monitoramentoAmbiental/historicoUmidade"
#define broker_luminosidade_h "monitoramentoAmbiental/historicoLuminosidade"
#define broker_pressao_h "monitoramentoAmbiental/historicoPressao"

void publicar(char *publisher_nome,char *topico, char *msg){

    int rc;
	struct mosquitto *mosq;
	mosquitto_lib_init();

	char pub[] = "Publisher: "; 
    strcat(pub, publisher_nome);

	mosq = mosquitto_new(pub,true,NULL);
	
	rc = mosquitto_connect(mosq,topico,1883,60);
	if(rc != 0){
			printf("O Cliente nao conseguiu se conectar ao broker! - {{%d}}\n", rc);
			mosquitto_destroy(mosq);
			return -1;
	}
	printf("Estamos conectados ao broker.\n");
	
	mosquitto_publish(mosq,NULL,topico,strlen(msg),msg,1,false);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	
	mosquitto_lib_cleanup();
	
	return 0;
}

void client(char *client_nome,char *topico){

    int rc;
	
	mosquitto_lib_init();
	
	struct mosquitto *mosq;
	
    char client[] = "Inscrito: "; 
    strcat(client, client_nome);

	mosq = mosquitto_new(client,true,NULL);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	rc = mosquitto_connect(mosq,topico,1883,10);
    mosquitto_subscribe(mosq,NULL,topico,1);
	
	if(rc){
		printf("Nao foi possivel conectar ao broker - {{%d}}\n",rc);
		return -1;
	}
	
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