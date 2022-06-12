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
 * Classe respons�vel por receber os dados do sensor, enviados pelo SBC
 */
public class Ouvinte implements IMqttMessageListener {

    private DadoSensores dados; //Atributo para armazenar os valores das medi��es

    /**
     * M�todo construtor da classe
     *
     * @param user Identificador de usu�rio.
     * @param password Senha de conex�o.
     * @param topic T�pico da mensagem.
     * @param qos N�vel de conex�o.
     */
    public Ouvinte(String user, String password, String topic, int qos) {
        dados = DadoSensores.getInstancia();
        OuvinteInterno clienteMQTT = new OuvinteInterno("tcp://broker.mqttdashboard.com:1883", user, password);
        clienteMQTT.iniciar();
        clienteMQTT.subscribe(qos, this, topic);
    }

    /**
     * M�todo executado quando uma mensagem � recebida do Broker.
     *
     * @param topic T�pico em que a mensagem foi enviada.
     * @param mm Mensagem recebida.
     * @throws Exception
     */
    @Override
    public void messageArrived(String topic, MqttMessage mm) throws Exception {
        String mensagem = new String(mm.getPayload());
        System.out.println("Dado lido no t�pico: "+topic+": "+mensagem);
        switch (topic) {
            case "monitoramentoAmbiental/temperatura":
                dados.setTemperatura(mensagem);
                break;
            case "monitoramentoAmbiental/umidade":
                dados.setUmidade(mensagem);
                break;
            case "monitoramentoAmbiental/luminosidade":
                dados.setLuminosidade(mensagem);
                break;
            case "monitoramentoAmbiental/pressao":
                dados.setPressao(mensagem);
                break;
            case "monitoramentoAmbiental/tempo":
                dados.setTempo(mensagem);
                break;
            case "monitoramentoAmbiental/historicoTemperatura":
                String[] temperaturas= mensagem.split(":"); //Separa a string de dados
                dados.getTemperaturas().clear();
                for(int i =0; i<temperaturas.length; i++){
                    dados.getTemperaturas().add(temperaturas[i]);
                }
                break;  
            case "monitoramentoAmbiental/historicoUmidade":
                 String[] umidades= mensagem.split(":"); //Separa a string de dados
                 dados.getUmidades().clear();
                for(int i =0; i<umidades.length; i++){
                    dados.getUmidades().add(umidades[i]);
                }
                break;  
            case "monitoramentoAmbiental/historicoLuminosidade":
                 String[] luminosidades= mensagem.split(":"); //Separa a string de dados
                 dados.getLuminosidades().clear();
                for(int i =0; i<luminosidades.length; i++){
                    dados.getLuminosidades().add(luminosidades[i]);
                }
                break;  
            case "monitoramentoAmbiental/historicoPressao":
                 String[] pressoes= mensagem.split(":"); //Separa a string de dados
                 dados.getPressoes().clear();
                 for(int i=0; i<pressoes.length;i++){
                     dados.getPressoes().add(pressoes[i]);
                 }
                break;  
            default:
                System.out.println("T�pico n�o encontrado");
                break;
        }
    }

}

/**
 * Encarrega-se da conex�o com o broker MQTT.
 *
 */
class OuvinteInterno implements MqttCallbackExtended {

    private final String serverURI;
    private MqttClient client;
    private final MqttConnectOptions mqttOptions;

    /**
     * M�todo construtor
     *
     * @param serverURI URI para o broker MQTT
     * @param usuario Identificador de usu�rio.
     * @param senha Senha de conex�o.
     */
    public OuvinteInterno(String serverURI, String usuario, String senha) {
        this.serverURI = serverURI;
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
        if (client == null || topicos.length == 0) {
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
            return client.subscribeWithResponse(topicos, qoss, listners);
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
        if (client == null || !client.isConnected() || topicos.length == 0) {
            return;
        }
        try {
            client.unsubscribe(topicos);
        } catch (MqttException ex) {
            System.out.println(String.format("Ouvinte: Erro ao se desinscrever no t�pico %s - %s", Arrays.asList(topicos), ex));
        }
    }

    /**
     * Inicializa a conex�o do cliente com o Broker.
     */
    public void iniciar() {
        try {
            System.out.println("Ouvinte: Conectando no broker MQTT em " + serverURI);
            client = new MqttClient(serverURI, String.format("cliente_java_%d", System.currentTimeMillis()), new MqttDefaultFilePersistence(System.getProperty("java.io.tmpdir")));
            client.setCallback((MqttCallback) this);
            client.connect(mqttOptions);
        } catch (MqttException ex) {
            System.out.println("Ouvinte: Erro ao se conectar ao broker mqtt " + serverURI + " - " + ex);
        }
    }

    /**
     * Finaliza a conex�o do cliente com o broker.
     */
    public void finalizar() {
        if (client == null || !client.isConnected()) {
            return;
        }
        try {
            client.disconnect();
            client.close();
        } catch (MqttException ex) {
            System.out.println("Ouvinte: Erro ao desconectar do broker mqtt - " + ex);
        }
    }

    @Override
    public void connectionLost(Throwable thrwbl) {
        System.out.println("Ouvinte: Conex�o com o broker perdida -" + thrwbl);
    }

    @Override
    public void connectComplete(boolean reconnect, String serverURI) {
        System.out.println("Cliente ouvinte " + (reconnect ? "reconectado" : "conectado") + " com o broker " + serverURI);
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken imdt) {
    }

    @Override
    public void messageArrived(String topic, MqttMessage mm) throws Exception {
    }

}
