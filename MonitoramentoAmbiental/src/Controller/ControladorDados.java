package Controller;

import java.util.ArrayList;
import model.ClienteMQTT;
import model.DadoSensores;
import model.Editor;
import model.Ouvinte;

public class ControladorDados {

    private DadoSensores dados; //Armazena os dados ouvidos do broker
    private Editor publicador; //ClienteMQTT do tipo editor
    private static ControladorDados instancia;

    private ControladorDados() {
        dados = DadoSensores.getInstancia();//obtem a instancia da classe DadosSensores
        //Cria o editor e inicializa a conexao
        publicador = new Editor("tcp://10.0.0.101:1883","aluno", "aluno*123"); //Cria o publicador, passando o endereço do broker
        publicador.iniciar();
        //Cria o cliente MQTT e inicializa a conexao
        ClienteMQTT clienteMQTT = new ClienteMQTT("tcp://10.0.0.101:1883", "aluno", "aluno*123");
        clienteMQTT.iniciar();
        //Instancia os ouvintes, que recebe as informações sobre cada sensor.
        Ouvinte ouvinteTemperatura = new Ouvinte(clienteMQTT, "monitoramentoAmbiental/temperatura", 2);
        Ouvinte ouvinteUmidade = new Ouvinte(clienteMQTT, "monitoramentoAmbiental/umidade", 2);
        Ouvinte ouvinteLuminosidade = new Ouvinte(clienteMQTT, "monitoramentoAmbiental/luminosidade", 2);
        Ouvinte ouvintePressao = new Ouvinte(clienteMQTT, "monitoramentoAmbiental/pressao", 2);
        Ouvinte ouvinteTempo = new Ouvinte(clienteMQTT, "monitoramentoAmbiental/tempo", 2);
       }
    
    public static synchronized ControladorDados getInstancia() {
        if (instancia == null) {
            instancia = new ControladorDados();
        }
        return instancia;
    }
        
    /**
     * Método que publica no broker, o intervalo de tempo entre as medidas
     *
     * @param tempo - a mensagem a ser publicada
     */
    public void alterarTempo(String tempo) {
        publicador.publicar("monitoramentoAmbiental/tempo", tempo.getBytes(), 2);
    }

    /**
     * Método que retorna a medida da temperatura para a interface
     *
     * @return String - temperatura
     */
    public String getTemperatura() {
        return dados.getTemperatura();
    }

    /**
     * Método que retorna a medida da luminosidade para a interface
     *
     * @return String - luminosidade
     */
    public String getLuminosidade() {
        return dados.getLuminosidade();
    }

    /**
     * Método que retorna a medida da umidade para a interface
     *
     * @return Sting - umidade
     */
    public String getUmidade() {
        return dados.getUmidade();
    }

    /**
     * Método que retorna a medida da pressao para a interface
     *
     * @return String - pressao
     */
    public String getPressao() {
        return dados.getPressao();
    }

    /**
     * Método que retorna o intervalo de tempo de medida, para a interface
     *
     * @return String - tempo
     */
    public String getTempo() {
        return dados.getTempo();
    }
   
        /**
     * Método que retorna a lista contendo o histórico da temperatura
     *
     * @return temperaturas
     */
    public ArrayList<String> getTemperaturas() {
        return dados.getTemperaturas();
    }

    /**
     * Método que retorna a lista contendo o histórico da luminosidade
     *
     * @return luminosidades
     */
    public ArrayList<String> getLuminosidades() {
        return dados.getLuminosidades();
    }

    /**
     * Método que retorna a lista contendo o histórico da pressao atmosferica
     *
     * @return pressoes
     */
    public ArrayList<String> getPressoes() {
         return dados.getPressoes();
    }

    /**
     * Método que retorna a lista contendo o histórico da umidade
     *
     * @return umidades
     */
    public ArrayList<String> getUmidades() {
        return dados.getUmidades();
    }
}
