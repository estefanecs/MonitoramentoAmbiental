<h1  align="center"> Protótipo de sistema para monitoramento ambiental </h1>

Executar dentro da pasta dist,no terminal: java -jar MonitoramentoAmbiental.jar

<h1>Interface Local</h1>

<p> Para o acesso e visualização dos dados dos sensores no SBC foi construinda um interface homem maquina local que viabiliza o usuario interagir com o SBC</p>
	<p> A interface foi construida usando um display LCD 16x2, e push buttons, para o controle do display LCD foi utilizada a biblioteca
		LCD.h e para uso dos botões e GPIO da raspberry foi utilizada a biblioteca wiringPi, as bibliotecas definem as funções para a comunicação com os dispositivos, abaixo tem-se a listagem das funções utilizadas e sua finalidade. 
	</p>

<p>A interface local exibe os dados de temperatura,umidade,pressao,luminosidade  com o historico das dez ultimas medições de cada medida além do ajuste do tempo de medição, 
	a exibição dos dados do display foi organizada em menus. Cada medida possui um menu e um submenu, no menu é exibida a ultima medição realizada e um opção para acessar o historico ao pressionar o <strong>>botão 2</strong> , dentro do submenu do historico são exibidos dois dados do historico um em cada linha ao pressionar o <strong>botão 1</strong> os dados são alternados indo dois primeiros aos dois ultimos e ao pressionar o <strong>botão 2</strong> o display volta ao menu anterior que estava, para alternar entre os menus o <strong>botão 1</strong> deve ser pressionado permitindo que sejam exibidos 5 menus diferentes.O ultimo menu é o menu de alterção do tempo de sensoriamento discuitido abaixo.</p>

<p>O menu de  alterar tempo de sensoriamento exibe o tempo atual do intervalo de medição e a opção para alterar o tempo de medição ao pressionar o <strong>botão 2</strong>, no
		submenu de alterar o tempo de exbição são exibidos 7 digitos que podem ser incrementados de 0-9 para definir o intervalo de tempo novo, os dois primeiros digitos correspondem ao valor de
		minutos, o terceiro e quarto digitos correspondem ao tempo em segundos e os trẽs ultimos ao tempo em milisegundos, para alternar entre os digitos o <strong>botão 1</strong> é usado, para incrementar o digito o <strong>botão 2</strong> é pressionado os valores para o digito vão de 0 até 9 de modo circular, para sair do submenu e alterar o tempo de medição o <strong>botão 3</strong> de ser pressionado.
	</p>
	<h4>IMAGEM AQUI </h4>
	<p>A logica de controle do display foi implementada como uma maquina de estados, para cada estado listada uma configuração diferente é exibida no display, com essa abordagem pode-se garantir uma progressão logica na exibição da interface e a facilidade de inserir novos estados se fossem nescessarios. A variavel <strong>estado</strong> guarda o estado atual para o display, os estados são definidos no arquivo SBC.h, a implementação da maquina de estados seguiu a abordagem com o uso do switch-case assim em cada case do switch as instrunções para o display são definidas, a logica da mudança de estado é feita com base no estado atual e com os botões, nos menus e submenus as transições ocorrem quando os botões <strong>B1</strong> e <strong>B2</strong> são pressionados apenas com a exceção do submenu de alterar tempo em que só ocore uma transição de estado quando <strong>botão 3</strong> é pressionado.</p>


O programa da interface é rodado numa thread em que existe um loop infinito que executa infinitamente o switch-case
	```s
  		 while(1){
        //Realiza a leitura dos botoes
        //Botao quando precionado tem leitura em nivel logico baixo. Nao precisonado tem como leitura nivel logico alto
    	b0 = digitalRead(B0);  
        b1 = digitalRead(B1); 
    	b2 = digitalRead(B2);

        /*  A interface eh implementada como um maquina de estados usando Switch-case
            |   cada exibicao no display eh considerada como um estado diferente
            |   os botoes sao responsaveis pela interacao do usuario com a interface
            |   b0 eh usado para alternar menus
            |   b1 eh usado para funcao_1 de cada menu e submenu
            |   b2 eh usado para funcao_2 de cada menu e submenu
            */
    	switch(estado){
    		case ES_MENU0:	// Estado que exibe a medida atual da temperatura e a opcao de historico de temperatura
                 /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%s",menu[0],temperatura);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");
    			
    			/*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	  // Botao que seleciona primeira opcao apertado
    				estado=ES_HTEMP; //  Alterna para Historico de medicoes
    			}
    			if(!b0){	  //Botao de alternar Menu pressionado
    				estado=ES_MENU1; // Alterna para proxima opcao do Menu
    			}
    			break;

    		case ES_MENU1:	// Estado que exibe a medida atual de umidade e a opcao de historico de umidade
                /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%s",menu[1],umidade);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_HUMID; //  Alterna para Historico de medicoes
    			}
    			if(!b0){	 //Botao de alternar Menu pressionado
    				estado=ES_MENU2; // Alterna para proxima opcao do Menu
    			}
    			break;

            case ES_MENU2:	// Estado que exibe a medida atual de pressao e a opcao de historico de pressao
               /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[2],pressao);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_HPRES; //  Alterna para Historico de medicoes
    			}
    			if(!b0){	 //Botao de alternar Menu pressionado
    				estado=ES_MENU3; // Alterna para proxima opcao do Menu
    			}
    			break;

    		case ES_MENU3:	// Estado que exibe a medida atual de luminosidade e a opcao de historico de luminosidade
                /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPrintf(lcd,"%s:%d",menu[3],luminosidade);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"-->Historico");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_HLUMI;//  Alterna para Historico de medicoes
    			}
    			if(!b0){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_MENU4;// Alterna para proxima opcao do Menu
    			}
    			break;

    		case ES_MENU4:	// Estado que exibe o valor do tempo e a opcao de altera-lo
                /*Imprime no Display as informacoes do menu atual*/
    			lcdPosition(lcd,0,0);
    			lcdPuts(lcd,menu[4]);
    			lcdPosition(lcd,0,1);
    			lcdPuts(lcd,"->Alterar tempo");

    			 /*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	 // Botao que seleciona primeira opcao apertado
    				estado=ES_TIME; //  Alterna para Historico de medicoes
                    idx=0;
    			}
    			if(!b0){	  //Botao de alternar Menu pressionado
    				estado=ES_MENU0; // Alterna para proxima opcao do Menu
    			}
    			break;

            case ES_HTEMP:
               /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%s", idx_historico+1,historico_display[idx_historico].temp);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%s" ,idx_historico+2,historico_display[idx_historico+1].temp);

    			/*LOGICA DE MUDANCA DE ESTADO*/
    			if(!b1){	  // Botao que seleciona primeira opcao apertado
    				estado=ES_MENU0;  // Alterna para Menu anterior
    				idx_historico=0;  // reseta o indice do historico
    			}
    			 /*Logica adicional para configuracao do menu*/
    			if(!b0){	  //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
    			}
                break;

            case ES_HUMID:
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%s", idx_historico+1,historico_display[idx_historico].umi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%s" ,idx_historico+2,historico_display[idx_historico+1].umi);

                /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU1; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
            	}
                 /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_HPRES:
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].press);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].press);

              /*LOGICA DE MUDANCA DE ESTADO*/
                if(!b1){     // Botao que seleciona primeira opcao apertado
                    estado=ES_MENU2; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                }
                /*Logica adicional para configuracao do menu*/
                if(!b0){       //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_HLUMI:
                /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d-%d", idx_historico+1,historico_display[idx_historico].lumi);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"%d-%d" ,idx_historico+2,historico_display[idx_historico+1].lumi);
                
                 /*Imprime no Display as informacoes do menu atual*/
                if(!b1){      // Botao que seleciona primeira opcao apertado
                    estado= ES_MENU3; // Alterna para Menu anterior
                    idx_historico=0; // reseta o indice do historico
                }
                /*Logica adicional para configuracao do menu*/
                if(!b0){      //Botao de alternar Menu pressionado
                    if(idx_historico==8)
                        idx_historico=0;
                    else
                        idx_historico+=2;
                }
                break;

            case ES_TIME:
                 /*Imprime no Display as informacoes do menu atual*/
                lcdPosition(lcd,0,0);
                lcdPrintf(lcd,"%d%d:",digitos_medicao[6],digitos_medicao[5]);
                lcdPosition(lcd,3,0);
                lcdPrintf(lcd,"%d%d:",digitos_medicao[4],digitos_medicao[3]);
                lcdPosition(lcd,6,0);
                lcdPrintf(lcd,"%d%d%d",digitos_medicao[2],digitos_medicao[1],digitos_medicao[0]);
                lcdPosition(lcd,0,1);
                lcdPrintf(lcd,"Digito:%d",idx+1);

                if(!b1){    // Incrementa o digito
                    digitos_medicao[idx] == 9 ? digitos_medicao[idx]=0: digitos_medicao[idx]++;  
                }
                if(!b2){    // Sai do menu de alterar tempo
                    estado = ES_MENU4;  // altera para o estado do menu de configurar tempo
                    /* atualiza o tempo de medicao*/
                    tempo_medicao= getMilisegundos(digitos_medicao); // chama funcao que converte o tempo definido nos digitos
                }
                if(!b0){    // Alterna o digito
                    idx ==6 ? idx=0:idx++;
                }
                break;
    		default: break;
    	}

        if(!b0 || !b1 || !b2){
            lcdClear(lcd);
        }

    	delay(100);
	}
	```
	
Ao final do loop é verificado se um botão foi pressionado, se algum botão é pressionado o display é limpo para escrita do novo estado
	
O switch verifica qual o case para o estado atual e então imprime no display os dados para a configuração atual(funções lcdPrint e lcdPosition)
	No fim de cada case há um trecho que verifica a logica da mudança de proximo estado, baseado no estado atual e em qual botão foi pressionado a varivel estado é atulizada para o proximo estado

	
Sensoriamento

Protocolo de comunicação MQTT

Interface Remota