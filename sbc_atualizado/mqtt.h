#ifndef _mqtt_h
#define _mqtt_h

void publicar(char *publisher_nome,char *topico, char *msg);
void client(char *client_nome,char *topico);
void on_connect(struct mosquitto *mosq, void *obj, int rc);
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

#endif