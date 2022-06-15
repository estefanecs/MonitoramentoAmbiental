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
 * Encarrega-se da conex�o com o broker MQTT.
 *
 */
public class ClienteMQTT implements MqttCallbackExtended {

    private final String servidorURI; 
    private MqttClient cliente;
    private final MqttConnectOptions mqttOptions;

    /**
     * M�todo construtor
     *
     * @param servidor URI para o broker MQTT
     * @param usuario Identificador de usu�rio.
     * @param senha Senha de conex�o.
     */
    public ClienteMQTT(String servidor, String usuario, String senha) {
        this.servidorURI = servidor;
        //Configura��es MQTT.
        mqttOptions = new MqttConnectOptions();
        mqttOptions.setMaxInflight(200);
        mqttOptions.setConnectionTimeout(3);
        mqttOptions.setKeepAliveInterval(10);
        mqttOptions.setAutomaticReconnect(true);
        mqttOptions.setCleanSession(false);

        //Configura o usu�rio caso haja.
        if (usuario != null && senha != null) {
            mqttOptions.setUserName(usuario);
            mqttOptions.setPassword(senha.toCharArray());
        }
    }

    /**
     * Configura a inscri��o no t�pico.
     *
     * @param qos n�vel de conex�o.
     * @param gestorMensagemMQTT objeto para receber as mensagens MQTT.
     * @param topicos Lista de t�picos em que o editor ser� inscrito.
     * @return Objeto para acompanhar as mensagens ass�ncronas recebidas.
     */
    public IMqttToken subscribe(int qos, IMqttMessageListener gestorMensagemMQTT, String... topicos) {
        if (cliente == null || topicos.length == 0) {
            return null;
        }
        int tamanho = topicos.length;
        int[] qoss = new int[tamanho];
        //Lista de listeners para os diversos t�picos.
        IMqttMessageListener[] listners = new IMqttMessageListener[tamanho];
        //Configura os qoss para cada t�pico.
        for (int i = 0; i < tamanho; i++) {
            qoss[i] = qos;
            listners[i] = gestorMensagemMQTT;
        }
        try {//Realiza a inscri��o no t�pico.
            return cliente.subscribeWithResponse(topicos, qoss, listners);
        } catch (MqttException ex) {
            System.out.println(String.format("Ouvinte: Erro ao se inscrever nos t�picos %s - %s", Arrays.asList(topicos), ex));
            return null;
        }
    }

    /**
     * M�todo para remover a inscri��o nos t�picos passados.
     *
     * @param topicos T�picos para remover a inscri��o.
     */
    public void unsubscribe(String... topicos) {
        if (cliente == null || !cliente.isConnected() || topicos.length == 0) {
            return;
        }
        try {
            cliente.unsubscribe(topicos);
        } catch (MqttException ex) {
            System.out.println(String.format("Ouvinte: Erro ao se desinscrever no t�pico %s - %s", Arrays.asList(topicos), ex));
        }
    }

    /**
     * Inicializa a conex�o do cliente com o Broker.
     */
    public void iniciar() {
        try {
            System.out.println("Ouvinte: Conectando no broker MQTT em " + servidorURI);
            //Cria o cliente e inicia a conex�o
            cliente = new MqttClient(servidorURI, String.format("cliente_java_%d", System.currentTimeMillis()), new MqttDefaultFilePersistence(System.getProperty("java.io.tmpdir")));
            cliente.setCallback((MqttCallback) this);
            cliente.connect(mqttOptions);
        } catch (MqttException ex) {//Se n�o conectar
            System.out.println("Ouvinte: Erro ao se conectar ao broker mqtt " + servidorURI + " - " + ex);
        }
    }

    /**
     * M�todo que finaliza a conex�o com o broker MQTT apenas se houver cliente
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
     * Metodo que realiza a publica��o de uma informa��o em um t�pico
     *
     * @param topico -topico a ser publicado
     * @param informacao - informa��o
     * @param qos - variavel de controle de qualidade de mensagem
     */
    public void publicar(String topico, byte[] informacao, int qos) {
        publicar(topico, informacao, qos, false);
    }

    /**
     * **
     * Metodo que realiza a publica��o de uma informa��o em um topico
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
                System.out.println(String.format("Informa��o publicada no topico %s com sucesso", topico));
            } else { //Se estiver desconectado
                System.out.println("N�o foi possivel publicar no topico " + topico + ", pois o editor est� desconectado");
            }
        } catch (MqttException ex) { //Se houver erros ao publicar o topico
            System.out.println("Erro ao publicar o topico:" + topico + ": " + ex);
        }
    }

    /**
     * M�todo que sinaliza se a conexao com o broker foi perdida
     *
     * @param thrwbl
     */
    @Override
    public void connectionLost(Throwable thrwbl) {
        System.out.println("Ouvinte: Conex�o com o broker perdida -" + thrwbl);
    }

    /**
     * M�todo que indica se o editor conseguiu conectar-se ou reconectar-se com
     * o broker
     *
     * @param reconectado
     * @param servidor - endere�o do servidor
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
