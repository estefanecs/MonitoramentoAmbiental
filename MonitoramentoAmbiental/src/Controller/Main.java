/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package Controller;
import java.text.SimpleDateFormat;

/**
 *
 * @author estef
 */
public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) throws InterruptedException {
        ClienteMQTT clienteMQTT = new ClienteMQTT("tcp://broker.mqttdashboard.com:1883", null, null);
        clienteMQTT.iniciar();

        //Ouvinte ouvinte = new Ouvinte(clienteMQTT, "monitoramentoAmbiental/temperatura", 0);
        Ouvinte ouvinte1= new Ouvinte(clienteMQTT,"monitoramentoAmbiental/tempo",0);
        int i=-5;
        
        while (true) {
            Thread.sleep(1000);
            String mensagem = "451";
            mensagem = mensagem.concat(" "+i);
            clienteMQTT.publicar("monitoramentoAmbiental/temperatura", mensagem.getBytes(), 0);
            clienteMQTT.publicar("monitoramentoAmbiental/umidade", mensagem.getBytes(), 0);
            clienteMQTT.publicar("monitoramentoAmbiental/luminosidade", mensagem.getBytes(), 0);
            clienteMQTT.publicar("monitoramentoAmbiental/pressao", mensagem.getBytes(), 0);
            i++;
        }
    }
}
