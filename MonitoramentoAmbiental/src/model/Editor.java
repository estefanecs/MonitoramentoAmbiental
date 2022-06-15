
package model;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MqttDefaultFilePersistence;


/**
 * Esta classe sao para objetos do tipo Editor, um cliente do servidor MQTT. Contem atributos como o
 * endere�o do servidor, o cliente e as opcoes de conexoes do MQTT.
 * Exemplo de uso:
 *
 * Editor editor= new Editor("servidor");
 *
 */
public class Editor implements MqttCallbackExtended {

    private final String servidor; //URL do servidor
    private MqttClient client; //Cliente MQTT
    private final MqttConnectOptions mqttOptions; //Op��es de conexoes
 
    /**
     * Construtor da classe
     * @param servidor - endereco do servidor
     * @param usuario - Identificador de usu�rio.
     * @param senha - Senha de conex�o
     */
    public Editor(String servidor, String usuario, String senha) {
        this.servidor = servidor;

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
     * Metodo que inicia a execucao do cliente editor
     */
    public void iniciar() {
        try {
            System.out.println("Conectando ao broker MQTT " + servidor);
            //Cria o cliente e inicia a conex�o
            client = new MqttClient(servidor, String.format("cliente_editor_java_%d", System.currentTimeMillis()), new MqttDefaultFilePersistence(System.getProperty("java.io.tmpdir")));
            client.setCallback(this);
            client.connect(mqttOptions); 
        } catch (MqttException ex) { //Se n�o conectar
            System.out.println("Erro ao tentar conectar-se com o broker MQTT " + servidor + " - " + ex);
        }
    }
    
    /**
     * M�todo que finaliza a conex�o com o broker MQTT apenas se houver cliente
     * e estiver conectado com o broker
     */
    public void finalizar() {
        if (client != null && client.isConnected()) { //Se cliente nao for nulo ou estiver conectado
            try {
                client.disconnect(); //Disconecta
                client.close(); 
            } catch (MqttException ex) {
                System.out.println("Erro ao tentar desconectar do broker MQTT - " + ex);
            }
        }
    }

    /**
     * Metodo que realiza a publica��o de uma informa��o em um t�pico
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
     * @param topico -topico a ser publicado 
     * @param mensagem - mensagem
     * @param qos - variavel de controle de qualidade de mensagem
     * @param retained 
     */
    public synchronized void publicar(String topico, byte[] mensagem, int qos, boolean retained) {
        try {
            if (client.isConnected()) { //Se o cliente estiver conectado ao broker MQTT
                client.publish(topico, mensagem, qos, retained); //publica a informacao no topico
                System.out.println(String.format("Informa��o publicada no topico %s com sucesso", topico));
            } else { //Se estiver desconectado
                System.out.println("N�o foi possivel publicar no topico " + topico+", pois o editor est� desconectado");
            }
        } catch (MqttException ex) { //Se houver erros ao publicar o topico
            System.out.println("Erro ao publicar o topico:" + topico + ": " + ex);
        }
    }

     /**
     * M�todo que sinaliza se a conexao com o broker foi perdida
     * @param thrwbl 
     */
    @Override
    public void connectionLost(Throwable thrwbl) {
        System.out.println("Editor: Conexao com o broker perdida -" + thrwbl);
    }
    
    /**
     * M�todo que indica se o editor conseguiu conectar-se ou reconectar-se com o broker
     * @param reconnect
     * @param servidor - endere�o do servidor 
     */
    @Override
    public void connectComplete(boolean reconnect, String servidor) {
        System.out.println("Editor " + (reconnect ? "reconectado" : "conectado") + " com o broker " + servidor);
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken imdt) {
    }

    @Override
    public void messageArrived(String topic, MqttMessage mm) throws Exception {
    }



}
