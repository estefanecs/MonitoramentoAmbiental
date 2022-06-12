package model;

import java.util.ArrayList;

public class DadoSensores {

    private static DadoSensores instancia;
    private String umidade;
    private String temperatura;
    private String luminosidade;
    private String pressao;
    private String tempo;
    private ArrayList<String> umidades;
    private ArrayList<String> temperaturas;
    private ArrayList<String> luminosidades;
    private ArrayList<String> pressoes;

    /**
     * M�todo construtor para a classe DadoSensores
     */
    private DadoSensores() {
        umidades = new ArrayList<>();
        temperaturas = new ArrayList<>();
        luminosidades = new ArrayList<>();
        pressoes = new ArrayList<>();
    }

    public static synchronized DadoSensores getInstancia() {
        if (instancia == null) {
            instancia = new DadoSensores();
        }
        return instancia;
    }

    /**
     * M�todo que retorna o valor da umidade
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
     * M�todo que retorna o valor da temperatura
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
     * M�todo que retorna o valor da luminosidade
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
     * M�todo que retorna o valor da pressao atmosferica
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
     * M�todo que retorna o valor do intervalo de tempo de medicao
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

    /**
     * M�todo que retorna a lista contendo o hist�rico da temperatura
     *
     * @return temperaturas
     */
    public ArrayList<String> getTemperaturas() {
        return temperaturas;
    }

    /**
     * M�todo que retorna a lista contendo o hist�rico da luminosidade
     *
     * @return luminosidades
     */
    public ArrayList<String> getLuminosidades() {
        return luminosidades;
    }

    /**
     * M�todo que retorna a lista contendo o hist�rico da pressao atmosferica
     *
     * @return pressoes
     */
    public ArrayList<String> getPressoes() {
        return pressoes;
    }

    /**
     * M�todo que retorna a lista contendo o hist�rico da umidade
     *
     * @return umidades
     */
    public ArrayList<String> getUmidades() {
        return umidades;
    }

}
