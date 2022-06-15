#include <stdlib.h>
#include <stdio.h>
#include <mosquitto.h>


void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
		printf("ID: %d\n", * (int *) obj);
		if(rc){
			printf("Erro de codigo: %d\n",rc);
			exit(-1);
		}
		mosquitto_subscribe(mosq,NULL,"dht11/temperatura",1);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("Nova mensagem para o topico %s: %s\n", msg->topic, (char *) msg->payload);
}

int main()
{
	int rc, id = 12;
	
	mosquitto_lib_init();
	
	struct mosquitto *mosq;
	
	mosq = mosquitto_new("inscrito",true,&id);
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	rc = mosquitto_connect(mosq,"localhost",1883,10);
	
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
	return 0;
}
