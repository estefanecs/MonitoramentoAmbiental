#include <stdio.h>
#include <mosquitto.h>


int main(int argc, char *argv[])
{
	int rc;
	struct mosquitto *mosq;
	mosquitto_lib_init();
	
	mosq = mosquitto_new("Publisher",true,NULL);
	
	rc = mosquitto_connect(mosq,"localhost",1883,60);
	if(rc != 0){
			printf("O Cliente nao conseguiu se conectar ao broker! - {{%d}}\n", rc);
			mosquitto_destroy(mosq);
			return -1;
	}
	printf("Estamos conectados ao broker.\n");
	
	mosquitto_publish(mosq,NULL,"dht11/temperatura",2,"oi",1,false);
	
	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	
	mosquitto_lib_cleanup();
	
	return 0;

}
