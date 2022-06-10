package model;

public class DadoSensores {

    private static DadoSensores instancia;
    private String umidade;
    private String temperatura;
    private String luminosidade;
    private String pressao;
    private String tempo;

    /**
     * Método construtor para a classe DadoSensores
     */
    private DadoSensores() {
    }

    public static synchronized DadoSensores getInstancia() {
        if (instancia == null) {
            instancia = new DadoSensores();
        }
        return instancia;
    }

    /**
     * Método que retorna o valor da umidade
     *
     * @return umidade
     */
    public String getUmidade() {
        return instancia.umidade;
    }

    /**
     * Metodo que altera a umidade lida
     *
     * @param umidade - nova umidade
     */
    public void setUmidade(String umidade) {
        instancia.umidade = umidade;
    }

    /**
     * Método que retorna o valor da temperatura
     *
     * @return temperatura
     */
    public String getTemperatura() {
        return instancia.temperatura;
    }

    /**
     * Metodo que altera a temperatura lida
     *
     * @param temperatura - nova temperatura
     */
    public void setTemperatura(String temperatura) {
        instancia.temperatura = temperatura;
    }

    /**
     * Método que retorna o valor da luminosidade
     *
     * @return luminosidade
     */
    public String getLuminosidade() {
        return instancia.luminosidade;
    }

    /**
     * Metodo que altera o valor da luminosidade
     *
     * @param luminosidade - novo valor
     */
    public void setLuminosidade(String luminosidade) {
        instancia.luminosidade = luminosidade;
    }

    /**
     * Método que retorna o valor da pressao atmosferica
     *
     * @return pressao
     */
    public String getPressao() {
        return instancia.pressao;
    }

    /**
     * Metodo que altera o valor da pressao atmosferica lida
     *
     * @param pressao - novo valor da pressao atmosferica
     */
    public void setPressao(String pressao) {
        instancia.pressao = pressao;
    }

    /**
     * Método que retorna o valor do intervalo de tempo de medicao
     *
     * @return tempo
     */
    public String getTempo() {
        return instancia.tempo;
    }

    /**
     * Metodo que altera o intervalo de tempo de medicao dos sensores
     *
     * @param tempo - novo intervalo
     */
    public void setTempo(String tempo) {
        instancia.tempo = tempo;
    }

}
