/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package Controller;

import org.eclipse.paho.client.mqttv3.IMqttMessageListener;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class Ouvinte implements IMqttMessageListener {

    public Ouvinte(ClienteMQTT clienteMQTT, String topico, int qos) {
        clienteMQTT.subscribe(qos, this, topico);
    }

    @Override
    public void messageArrived(String topico, MqttMessage mm) throws Exception {
        System.out.println("Mensagem recebida:");
        System.out.println("\tTópico: " + topico);
        System.out.println("\tMensagem: " + new String(mm.getPayload()));
        System.out.println("");
    }

}