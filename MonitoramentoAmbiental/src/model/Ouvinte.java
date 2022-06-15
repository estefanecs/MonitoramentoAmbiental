package model;

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;

/**
 * Classe respons�vel por receber os dados do sensor, enviados pelo SBC
 */
public class Ouvinte implements IMqttMessageListener {

    private DadoSensores dados; //Atributo para armazenar os valores das medi��es

    /**
     * M�todo construtor da classe
     *
     * @param clienteMQTT cliente
     * @param topic T�pico da mensagem.
     * @param qos N�vel de conex�o.
     */
    public Ouvinte(ClienteMQTT clienteMQTT, String topic, int qos) {
        dados = DadoSensores.getInstancia();
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
        System.out.println("Dado lido no t�pico: " + topic + ": " + mensagem);
        switch (topic) {
            //Altera o valor da temperatura se o t�pico for referente a ela
            case "monitoramentoAmbiental/temperatura":
                dados.setTemperatura(mensagem);
                break;

            //Altera o valor da umidade se o t�pico for referente a ela
            case "monitoramentoAmbiental/umidade":
                dados.setUmidade(mensagem);
                break;

            //Altera o valor da luminosidade se o t�pico for referente a ela
            case "monitoramentoAmbiental/luminosidade":
                dados.setLuminosidade(mensagem);
                break;

            //Altera o valor da pressao atmosferica se o t�pico for referente a ela
            case "monitoramentoAmbiental/pressao":
                dados.setPressao(mensagem);
                break;

            //Altera o tempo se o t�pico for referente a ele
            case "monitoramentoAmbiental/tempo":
                dados.setTempo(mensagem);
                break;

            //Se o t�pico for de hist�rico de temperatura
            case "monitoramentoAmbiental/historicoTemperatura":
                String[] temperaturas = mensagem.split(":"); //Separa a string de dados recedida
                dados.getTemperaturas().clear(); //limpa o array atual
                for (int i = 0; i < temperaturas.length; i++) { //armazena as medicoes recebidas no array
                    dados.getTemperaturas().add(temperaturas[i]);
                }
                break;

            //Se o t�pico for de hist�rico de umidade
            case "monitoramentoAmbiental/historicoUmidade":
                String[] umidades = mensagem.split(":"); //Separa a string de dados recebida
                dados.getUmidades().clear();//limpa o array atual
                for (int i = 0; i < umidades.length; i++) {//armazena as medicoes recebidas no array
                    dados.getUmidades().add(umidades[i]);
                }
                break;

            //Se o t�pico for de hist�rico de luminosidade
            case "monitoramentoAmbiental/historicoLuminosidade":
                String[] luminosidades = mensagem.split(":"); //Separa a string de dados recebida
                dados.getLuminosidades().clear();//limpa o array atual
                for (int i = 0; i < luminosidades.length; i++) {//armazena as medicoes recebidas no array
                    dados.getLuminosidades().add(luminosidades[i]);
                }
                break;
            //Se o t�pico for de hist�rico de pressao atmosferica   
            case "monitoramentoAmbiental/historicoPressao":
                String[] pressoes = mensagem.split(":"); //Separa a string de dados recebida
                dados.getPressoes().clear();//limpa o array atual
                for (int i = 0; i < pressoes.length; i++) {//armazena as medicoes recebidas no array
                    dados.getPressoes().add(pressoes[i]);
                }
                break;
            default:
                System.out.println("T�pico n�o encontrado");
                break;
        }
    }

}
