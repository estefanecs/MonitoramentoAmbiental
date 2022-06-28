#include <stdio.h>
#include <mosquitto.h>
#include <string.h>
#include <stdlib.h>
#include "mqtt.h"


struct mosquitto_message *destino;

extern float intervaloTempo;

//Recebe uma struct do tipo Publisher e utiliza as informações contidas na struct para publicar uma mensagem.
void publicar(Publisher pub){

    int rc;
	struct mosquitto *mosq;
	mosquitto_lib_init();//inicializa a biblioteca 

	mosq = mosquitto_new(pub.Nome,true,NULL);//cria o cliente mqtt

	mosquitto_username_pw_set(mosq,"aluno","aluno*123"); //Define usuario e senha

	rc = mosquitto_connect(mosq,pub.Host,1883,60);
	if(rc != 0){
			printf("O Publisher %s nao conseguiu se conectar ao broker! - {{%d}}\n",pub.Nome,rc);
			mosquitto_destroy(mosq);
			return ;
	}
	
	mosquitto_publish(mosq,NULL,pub.Topico,strlen(pub.Msg),pub.Msg,1,true);//publica a mensagem no broker
	
	//Desconecta e destroi o cliente mqtt
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	
	mosquitto_lib_cleanup();
	
	return ;
}

/* Funcao que cria um cliente, utilizando os valores previamente colocados numa struct do tipo Cliente.
Utiliza esses dados para se inscrever em um tópico e receber as mensagens.*/
void create_client(Cliente Observer){


    int rc;
	mosquitto_lib_init(); //inicializa a biblioteca
	struct mosquitto *mosq;

	mosq = mosquitto_new(Observer.Nome,true,NULL); //cria o cliente
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_message_callback_set(mosq, on_message);
	
	mosquitto_username_pw_set(mosq,"aluno","aluno*123"); //Define usuario e senha

	rc = mosquitto_connect(mosq,Observer.Host,1883,10);
	
	if(rc){
		printf("Nao foi possivel %s conectar ao broker - {{%d}}\n",Observer.Nome,rc);
		return ;
	}

	mosquitto_subscribe(mosq,NULL,Observer.Topico,1); //Faz a inscricao em um topico

	mosquitto_loop_start(mosq);
	getchar();
	mosquitto_loop_stop(mosq,true);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return;
}

/*Callback para caso o caso em que o cliente mqtt se conecta em algum broker. Caso sim, essa função é chamada.
 Deve ser definida anteriormente utilizando o mosquitto_connect_callback_set(sessao_do_Mosquito, Nome_da_função_Callback); *
void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
		printf("ID: %d\n", * (int *) obj);
		if(rc){
			printf("Erro de codigo - {{%d}}\n",rc);
			exit(-1);
		}

	return;
}

/* Funcao responsavel por receber as mensagens publicadas ao broker. 
Deve ser definida anteriormente utilizando o mosquitto_message_callback_set(sessao_do_Mosquito, Nome_da_funcao_Callback);*/
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
	intervaloTempo = atof((char *) msg->payload); //Transforma o dado recebido em um float
	return;
}

