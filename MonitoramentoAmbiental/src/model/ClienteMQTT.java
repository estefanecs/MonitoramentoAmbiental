package model;

import java.util.Arrays;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MqttDefaultFilePersistence;

/**
 * Encarrega-se da conexão com o broker MQTT.
 *
 */
public class ClienteMQTT implements MqttCallbackExtended {

    private final String servidorURI; 
    private MqttClient cliente;
    private final MqttConnectOptions mqttOptions;

    /**
     * Método construtor
     *
     * @param servidor URI para o broker MQTT
     * @param usuario Identificador de usuário.
     * @param senha Senha de conexão.
     */
    public ClienteMQTT(String servidor, String usuario, String senha) {
        this.servidorURI = servidor;
        //Configurações MQTT.
        mqttOptions = new MqttConnectOptions();
        mqttOptions.setMaxInflight(200);
        mqttOptions.setConnectionTimeout(3);
        mqttOptions.setKeepAliveInterval(10);
        mqttOptions.setAutomaticReconnect(true);
        mqttOptions.setCleanSession(false);

        //Configura o usuário caso haja.
        if (usuario != null && senha != null) {
            mqttOptions.setUserName(usuario);
            mqttOptions.setPassword(senha.toCharArray());
        }
    }

    /**
     * Configura a inscrição no tópico.
     *
     * @param qos nível de conexão.
     * @param gestorMensagemMQTT objeto para receber as mensagens MQTT.
     * @param topicos Lista de tópicos em que o editor será inscrito.
     * @return Objeto para acompanhar as mensagens assíncronas recebidas.
     */
    public IMqttToken subscribe(int qos, IMqttMessageListener gestorMensagemMQTT, String... topicos) {
        if (cliente == null || topicos.length == 0) {
            return null;
        }
        int tamanho = topicos.length;
        int[] qoss = new int[tamanho];
        //Lista de listeners para os diversos tópicos.
        IMqttMessageListener[] listners = new IMqttMessageListener[tamanho];
        //Configura os qoss para cada tópico.
        for (int i = 0; i < tamanho; i++) {
            qoss[i] = qos;
            listners[i] = gestorMensagemMQTT;
        }
        try {//Realiza a inscrição no tópico.
            return cliente.subscribeWithResponse(topicos, qoss, listners);
        } catch (MqttException ex) {
            System.out.println(String.format("Ouvinte: Erro ao se inscrever nos tópicos %s - %s", Arrays.asList(topicos), ex));
            return null;
        }
    }

    /**
     * Método para remover a inscrição nos tópicos passados.
     *
     * @param topicos Tópicos para remover a inscrição.
     */
    public void unsubscribe(String... topicos) {
        if (cliente == null || !cliente.isConnected() || topicos.length == 0) {
            return;
        }
        try {
            cliente.unsubscribe(topicos);
        } catch (MqttException ex) {
            System.out.println(String.format("Ouvinte: Erro ao se desinscrever no tópico %s - %s", Arrays.asList(topicos), ex));
        }
    }

    /**
     * Inicializa a conexão do cliente com o Broker.
     */
    public void iniciar() {
        try {
            System.out.println("Ouvinte: Conectando no broker MQTT em " + servidorURI);
            //Cria o cliente e inicia a conexão
            cliente = new MqttClient(servidorURI, String.format("cliente_java_%d", System.currentTimeMillis()), new MqttDefaultFilePersistence(System.getProperty("java.io.tmpdir")));
            cliente.setCallback((MqttCallback) this);
            cliente.connect(mqttOptions);
        } catch (MqttException ex) {//Se não conectar
            System.out.println("Ouvinte: Erro ao se conectar ao broker mqtt " + servidorURI + " - " + ex);
        }
    }

    /**
     * Método que finaliza a conexão com o broker MQTT apenas se houver cliente
     * e estiver conectado com o broker
     */
    public void finalizar() {
        if (cliente != null && cliente.isConnected()) { //Se cliente nao for nulo ou estiver conectado
            try {
                cliente.disconnect(); //Disconecta
                cliente.close();
            } catch (MqttException ex) {
                System.out.println("Erro ao tentar desconectar do broker MQTT - " + ex);
            }
        }
    }

    /**
     * Metodo que realiza a publicação de uma informação em um tópico
     *
     * @param topico -topico a ser publicado
     * @param informacao - informação
     * @param qos - variavel de controle de qualidade de mensagem
     */
    public void publicar(String topico, byte[] informacao, int qos) {
        publicar(topico, informacao, qos, false);
    }

    /**
     * **
     * Metodo que realiza a publicação de uma informação em um topico
     *
     * @param topico -topico a ser publicado
     * @param mensagem - mensagem
     * @param qos - variavel de controle de qualidade de mensagem
     * @param retained
     */
    public synchronized void publicar(String topico, byte[] mensagem, int qos, boolean retained) {
        try {
            if (cliente.isConnected()) { //Se o cliente estiver conectado ao broker MQTT
                cliente.publish(topico, mensagem, qos, retained); //publica a informacao no topico
                System.out.println(String.format("Informação publicada no topico %s com sucesso", topico));
            } else { //Se estiver desconectado
                System.out.println("Não foi possivel publicar no topico " + topico + ", pois o editor está desconectado");
            }
        } catch (MqttException ex) { //Se houver erros ao publicar o topico
            System.out.println("Erro ao publicar o topico:" + topico + ": " + ex);
        }
    }

    /**
     * Método que sinaliza se a conexao com o broker foi perdida
     *
     * @param thrwbl
     */
    @Override
    public void connectionLost(Throwable thrwbl) {
        System.out.println("Ouvinte: Conexão com o broker perdida -" + thrwbl);
    }

    /**
     * Método que indica se o editor conseguiu conectar-se ou reconectar-se com
     * o broker
     *
     * @param reconectado
     * @param servidor - endereço do servidor
     */
    @Override
    public void connectComplete(boolean reconectado, String servidor) {
        System.out.println("Cliente ouvinte " + (reconectado ? "reconectado" : "conectado") + " com o broker " + servidorURI);
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken imdt) {
    }

    @Override
    public void messageArrived(String topic, MqttMessage mm) throws Exception {
    }
}
