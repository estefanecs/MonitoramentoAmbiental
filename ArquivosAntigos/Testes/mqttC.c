#include <stdlib.h>
#include <stdio.h>
#include <mosquitto.h>
#include <string.h>

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
		printf("ID: %d\n", * (int *) obj);
		if(rc){
			printf("Erro de codigo: %d\n",rc);
			exit(-1);
		}
		mosquitto_subscribe(mosq,NULL,"monitoramentoAmbiental/tempo",1);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	printf("Nova mensagem para o topico %s: %s\n", msg->topic, (char *) msg->payload);
}

void isConnect(int rc, struct mosquitto *cliente){
	if(rc != 0){
		printf("O Cliente nao conseguiu se conectar ao broker!\n");
		mosquitto_destroy(cliente);
	}	
}
int main(int argc, char *argv[])
{
	int rc,rc2, id = 12;
	struct mosquitto *mosq;
	
	mosquitto_lib_init();
	
	mosq = mosquitto_new("Publisher",true,NULL);
	
	mosquitto_username_pw_set(mosq,"aluno","aluno*123");
	
	rc = mosquitto_connect(mosq,"10.0.0.101",1883,60);
	if(rc != 0){
			printf("O Cliente nao conseguiu se conectar ao broker! - {{%d}}\n", rc);
			mosquitto_destroy(mosq);
			return -1;
	}
	printf("Estamos conectados ao broker.\n");


	int i = 2;
	char msg[3];
	
	while(i < 500){
	
	sprinf(msg,"%d",i);
	mosquitto_publish(mosq,NULL,"monitoramentoAmbiental/temperatura",2,msg,1,false);
	mosquitto_publish(mosq,NULL,"monitoramentoAmbiental/umidade",2,msg,1,false);
	mosquitto_publish(mosq,NULL,"monitoramentoAmbiental/luminosidade",2,msg,1,false);
	mosquitto_publish(mosq,NULL,"monitoramentoAmbiental/pressao",2,msg,1,false);
	mosquitto_publish(mosq,NULL,"monitoramentoAmbiental/tempo",2,msg,1,false);
	i++;
	}

	//Inscrito
	struct mosquitto *mosqInscrito;
	mosqInscrito = mosquitto_new("inscrito",true,&id);
	mosquitto_connect_callback_set(mosqInscrito, on_connect);
	mosquitto_message_callback_set(mosqInscrito, on_message);
	
	mosquitto_username_pw_set(mosqInscrito,"aluno","aluno*123");
	
	rc2 = mosquitto_connect(mosqInscrito,"10.0.0.101",1883,10);
	
	if(rc2){
		printf("Nao foi possivel conectar ao broker - {{%d}}\n",rc2);
		return -1;
	}
	mosquitto_loop_start(mosqInscrito);
	printf("Aperte Enter para sair...\n");
	getchar();
	mosquitto_loop_stop(mosqInscrito,true);
	

	//Desconecta os clientes
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_disconnect(mosqInscrito);
	mosquitto_destroy(mosqInscrito);
	
	mosquitto_lib_cleanup();
	
	return 0;
	
}

