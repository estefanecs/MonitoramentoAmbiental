package view;

import Controller.ControladorDados;

public class Historico extends javax.swing.JFrame implements Runnable {

    private ControladorDados controlador;

    public Historico() {
        initComponents();
        this.setLocationRelativeTo(null);
        controlador = ControladorDados.getInstancia();;
        
        Thread t = new Thread(this);
        t.start();

    }
    /**
     * This method is called from within the constructor to initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is always
     * regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        jPanel1 = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        jPanel7 = new javax.swing.JPanel();
        temperaturaLabel = new javax.swing.JLabel();
        jScrollPane2 = new javax.swing.JScrollPane();
        historicoTemperatura = new javax.swing.JTextPane();
        jPanel8 = new javax.swing.JPanel();
        umidadeLabel1 = new javax.swing.JLabel();
        jScrollPane1 = new javax.swing.JScrollPane();
        historicoUmidade = new javax.swing.JTextPane();
        jPanel9 = new javax.swing.JPanel();
        pressaoLabel = new javax.swing.JLabel();
        jScrollPane3 = new javax.swing.JScrollPane();
        historicoPressao = new javax.swing.JTextPane();
        jPanel13 = new javax.swing.JPanel();
        luminosidadeLabel = new javax.swing.JLabel();
        jScrollPane4 = new javax.swing.JScrollPane();
        historicoLuminosidade = new javax.swing.JTextPane();
        voltar = new javax.swing.JButton();

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

        jPanel1.setBackground(new java.awt.Color(255, 255, 255));

        jLabel1.setFont(new java.awt.Font("Berlin Sans FB Demi", 0, 30)); // NOI18N
        jLabel1.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel1.setText("HIST�RICO");

        jPanel7.setBackground(new java.awt.Color(255, 255, 255));
        jPanel7.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED, new java.awt.Color(0, 51, 255), new java.awt.Color(0, 51, 255), new java.awt.Color(204, 204, 255), new java.awt.Color(51, 102, 255)));
        jPanel7.setForeground(new java.awt.Color(0, 51, 255));

        temperaturaLabel.setFont(new java.awt.Font("Berlin Sans FB Demi", 1, 19)); // NOI18N
        temperaturaLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        temperaturaLabel.setText("TEMPERATURA");

        historicoTemperatura.setBorder(null);
        historicoTemperatura.setFont(new java.awt.Font("Arial", 0, 18)); // NOI18N
        jScrollPane2.setViewportView(historicoTemperatura);

        javax.swing.GroupLayout jPanel7Layout = new javax.swing.GroupLayout(jPanel7);
        jPanel7.setLayout(jPanel7Layout);
        jPanel7Layout.setHorizontalGroup(
            jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(temperaturaLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 148, Short.MAX_VALUE)
            .addGroup(jPanel7Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane2)
                .addContainerGap())
        );
        jPanel7Layout.setVerticalGroup(
            jPanel7Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel7Layout.createSequentialGroup()
                .addGap(7, 7, 7)
                .addComponent(temperaturaLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane2, javax.swing.GroupLayout.PREFERRED_SIZE, 185, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(30, Short.MAX_VALUE))
        );

        jPanel8.setBackground(new java.awt.Color(255, 255, 255));
        jPanel8.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED, new java.awt.Color(0, 51, 255), new java.awt.Color(0, 51, 255), new java.awt.Color(204, 204, 255), new java.awt.Color(51, 102, 255)));
        jPanel8.setForeground(new java.awt.Color(0, 51, 255));

        umidadeLabel1.setFont(new java.awt.Font("Berlin Sans FB Demi", 1, 19)); // NOI18N
        umidadeLabel1.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        umidadeLabel1.setText("UMIDADE");

        historicoUmidade.setBorder(null);
        historicoUmidade.setFont(new java.awt.Font("Arial", 0, 18)); // NOI18N
        historicoUmidade.setAlignmentX(1.0F);
        jScrollPane1.setViewportView(historicoUmidade);

        javax.swing.GroupLayout jPanel8Layout = new javax.swing.GroupLayout(jPanel8);
        jPanel8.setLayout(jPanel8Layout);
        jPanel8Layout.setHorizontalGroup(
            jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(umidadeLabel1, javax.swing.GroupLayout.DEFAULT_SIZE, 148, Short.MAX_VALUE)
            .addGroup(jPanel8Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane1)
                .addContainerGap())
        );
        jPanel8Layout.setVerticalGroup(
            jPanel8Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel8Layout.createSequentialGroup()
                .addGap(7, 7, 7)
                .addComponent(umidadeLabel1)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.PREFERRED_SIZE, 185, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(31, Short.MAX_VALUE))
        );

        jPanel9.setBackground(new java.awt.Color(255, 255, 255));
        jPanel9.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED, new java.awt.Color(0, 51, 255), new java.awt.Color(0, 51, 255), new java.awt.Color(204, 204, 255), new java.awt.Color(51, 102, 255)));
        jPanel9.setForeground(new java.awt.Color(0, 51, 255));

        pressaoLabel.setFont(new java.awt.Font("Berlin Sans FB Demi", 1, 19)); // NOI18N
        pressaoLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        pressaoLabel.setText("PRESS�O ATM");

        historicoPressao.setBorder(null);
        historicoPressao.setFont(new java.awt.Font("Arial", 0, 18)); // NOI18N
        jScrollPane3.setViewportView(historicoPressao);

        javax.swing.GroupLayout jPanel9Layout = new javax.swing.GroupLayout(jPanel9);
        jPanel9.setLayout(jPanel9Layout);
        jPanel9Layout.setHorizontalGroup(
            jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(pressaoLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 148, Short.MAX_VALUE)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, jPanel9Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane3)
                .addContainerGap())
        );
        jPanel9Layout.setVerticalGroup(
            jPanel9Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel9Layout.createSequentialGroup()
                .addGap(7, 7, 7)
                .addComponent(pressaoLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane3, javax.swing.GroupLayout.PREFERRED_SIZE, 185, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(30, Short.MAX_VALUE))
        );

        jPanel13.setBackground(new java.awt.Color(255, 255, 255));
        jPanel13.setBorder(javax.swing.BorderFactory.createBevelBorder(javax.swing.border.BevelBorder.RAISED, new java.awt.Color(0, 51, 255), new java.awt.Color(0, 51, 255), new java.awt.Color(204, 204, 255), new java.awt.Color(51, 102, 255)));
        jPanel13.setForeground(new java.awt.Color(0, 51, 255));

        luminosidadeLabel.setFont(new java.awt.Font("Berlin Sans FB Demi", 1, 19)); // NOI18N
        luminosidadeLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        luminosidadeLabel.setText("LUMINOSIDADE");

        historicoLuminosidade.setBorder(null);
        historicoLuminosidade.setFont(new java.awt.Font("Arial", 0, 18)); // NOI18N
        jScrollPane4.setViewportView(historicoLuminosidade);

        javax.swing.GroupLayout jPanel13Layout = new javax.swing.GroupLayout(jPanel13);
        jPanel13.setLayout(jPanel13Layout);
        jPanel13Layout.setHorizontalGroup(
            jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(luminosidadeLabel, javax.swing.GroupLayout.DEFAULT_SIZE, 148, Short.MAX_VALUE)
            .addGroup(jPanel13Layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jScrollPane4)
                .addContainerGap())
        );
        jPanel13Layout.setVerticalGroup(
            jPanel13Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel13Layout.createSequentialGroup()
                .addGap(7, 7, 7)
                .addComponent(luminosidadeLabel)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(jScrollPane4, javax.swing.GroupLayout.PREFERRED_SIZE, 185, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(30, Short.MAX_VALUE))
        );

        voltar.setFont(new java.awt.Font("Arial", 0, 14)); // NOI18N
        voltar.setText("Voltar");
        voltar.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                voltarActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addComponent(jLabel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(jPanel8, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jPanel7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                        .addComponent(jPanel9, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(jPanel13, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                    .addGroup(jPanel1Layout.createSequentialGroup()
                        .addGap(20, 20, 20)
                        .addComponent(voltar)))
                .addGap(0, 14, Short.MAX_VALUE))
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(jPanel1Layout.createSequentialGroup()
                .addGap(26, 26, 26)
                .addComponent(jLabel1)
                .addGap(33, 33, 33)
                .addGroup(jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(jPanel7, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jPanel8, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jPanel9, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addComponent(jPanel13, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGap(52, 52, 52)
                .addComponent(voltar)
                .addContainerGap(88, Short.MAX_VALUE))
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 654, Short.MAX_VALUE)
            .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 519, Short.MAX_VALUE)
            .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(jPanel1, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void voltarActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_voltarActionPerformed
       this.setVisible(false);
    }//GEN-LAST:event_voltarActionPerformed

   

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JTextPane historicoLuminosidade;
    private javax.swing.JTextPane historicoPressao;
    private javax.swing.JTextPane historicoTemperatura;
    private javax.swing.JTextPane historicoUmidade;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel13;
    private javax.swing.JPanel jPanel7;
    private javax.swing.JPanel jPanel8;
    private javax.swing.JPanel jPanel9;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JScrollPane jScrollPane3;
    private javax.swing.JScrollPane jScrollPane4;
    private javax.swing.JLabel luminosidadeLabel;
    private javax.swing.JLabel pressaoLabel;
    private javax.swing.JLabel temperaturaLabel;
    private javax.swing.JLabel umidadeLabel1;
    private javax.swing.JButton voltar;
    // End of variables declaration//GEN-END:variables

    @Override
    public void run() {
        int i, j, t, z;
        String umidades, temperaturas, luminosidades, pressoes;
        while (true) {
            //Hist�rico de umidade
            umidades = "";
            historicoUmidade.setText(null);
            for (i = 0; i < controlador.getUmidades().size(); i++) {
                umidades = umidades.concat(controlador.getUmidades().get(i) + "\n");
            }
            historicoUmidade.setText(umidades);
            //Hist�rico de temperatura
            temperaturas = "";
            historicoTemperatura.setText(null);
            for (j = 0; j < controlador.getTemperaturas().size(); j++) {
                temperaturas = temperaturas.concat(controlador.getTemperaturas().get(j)+ "\n");
            }
            historicoTemperatura.setText(temperaturas);
            //Hist�rico de luminosidade
            luminosidades = "";
            historicoLuminosidade.setText(null);
            for (t = 0; t < controlador.getLuminosidades().size(); t++) {
                luminosidades = luminosidades.concat(controlador.getLuminosidades().get(t)+ "\n");
            }
            historicoLuminosidade.setText(luminosidades);
            //Hist�rico de pressao atmosferica
            pressoes = "";
            historicoPressao.setText(null);
            for (z = 0; z < controlador.getPressoes().size(); z++) {
                pressoes = pressoes.concat(controlador.getPressoes().get(z)+ "\n");
            }
            historicoPressao.setText(pressoes);
        }
    }

}
