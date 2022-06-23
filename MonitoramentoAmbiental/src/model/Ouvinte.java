package model;

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;

/**
 * Classe responsável por receber os dados do sensor, enviados pelo SBC
 */
public class Ouvinte implements IMqttMessageListener {

    private DadoSensores dados; //Atributo para armazenar os valores das medições

    /**
     * Método construtor da classe
     *
     * @param clienteMQTT cliente
     * @param topic Tópico da mensagem.
     * @param qos Nível de conexão.
     */
    public Ouvinte(ClienteMQTT clienteMQTT, String topic, int qos) {
        dados = DadoSensores.getInstancia();
        clienteMQTT.subscribe(qos, this, topic);
    }

    /**
     * Método executado quando uma mensagem é recebida do Broker.
     *
     * @param topic Tópico em que a mensagem foi enviada.
     * @param mm Mensagem recebida.
     * @throws Exception
     */
    @Override
    public void messageArrived(String topic, MqttMessage mm) throws Exception {
        String mensagem = new String(mm.getPayload());
        String[] medicao;
        System.out.println("Dado lido no tópico: " + topic + ": " + mensagem);
        switch (topic) {
            //Altera o valor da temperatura se o tópico for referente a ela e adiciona no historico
            case "monitoramentoAmbiental/temperatura":
                medicao = mensagem.split("-"); //Separa a string de dados recedida
                dados.setTemperatura(medicao[0]);
                if(dados.getTemperaturas().size()==10){
                   dados.getTemperaturas().remove(9);//Remove o ultimo elemento se houver 10 itens
                }
                dados.getTemperaturas().add(0,mensagem); //Adiciona no historico
                break;

            //Altera o valor da umidade se o tópico for referente a ela e adiciona no historico
            case "monitoramentoAmbiental/umidade":
                medicao = mensagem.split("-"); //Separa a string de dados recedida
                dados.setUmidade(medicao[0]);
                if(dados.getUmidades().size()==10){
                   dados.getUmidades().remove(9);//Remove o ultimo elemento se houver 10 itens
                }
                dados.getUmidades().add(0,mensagem); //adiciona no historico
                break;

            //Altera o valor da luminosidade se o tópico for referente a ela e adiciona no historico
            case "monitoramentoAmbiental/luminosidade":
                medicao = mensagem.split("-"); //Separa a string de dados recedida
                dados.setLuminosidade(medicao[0]);
                 if(dados.getLuminosidades().size()==10){ 
                   dados.getLuminosidades().remove(9);//Remove o ultimo elemento se houver 10 itens
                }
                dados.getLuminosidades().add(0,mensagem);
                break;

            //Altera o valor da pressao atmosferica se o tópico for referente a ela e adiciona no historico
            case "monitoramentoAmbiental/pressao":
                medicao = mensagem.split("-"); //Separa a string de dados recedida
                dados.setPressao(medicao[0]);
                 if(dados.getPressoes().size()==10){
                   dados.getPressoes().remove(9); //Remove o ultimo elemento se houver 10 itens
                }
                dados.getPressoes().add(0,mensagem); //Adiciona no historico
                break;

            //Altera o tempo se o tópico for referente a ele
            case "monitoramentoAmbiental/tempo":
                dados.setTempo(mensagem);
                break;
            default:
                System.out.println("Tópico não encontrado");
                break;
        }
    }

}
