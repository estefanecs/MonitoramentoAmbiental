
<h1  align="center"> Protótipo de sistema para monitoramento ambiental </h1>

Executar dentro da pasta dist,no terminal: java -jar MonitoramentoAmbiental.jar

<h1>Apresentação do sistema</h1>

<h1>Single Board Computer (SBC)</h1>

<h2>Sensor DHT11</h2>

<h2>Sensor de luminosidade e pressão atmosférica</h2>

<h2>Protocolo Message Queuing Telemetry Transport (MQTT)</h2>

<h2>Histórico de medições</h2>

<h1>Interface Desktop</h1>

<h2>Modo de uso</h2>

<h1>Interface Local</h1>
<p p align="justify"> Para o acesso e visualização dos dados dos sensores no SBC foi construída uma interface homem máquina local que viabiliza o usuário interagir com o SBC</p>

<p align="justify"> A interface foi construída usando um display LCD 16x2, e push buttons, para o controle do display LCD foi utilizada a biblioteca
		LCD.h e para uso dos botões e GPIO da raspberry foi utilizada a biblioteca wiringPi, as bibliotecas definem as funções para a comunicação com os dispositivos,          abaixo tem-se a listagem das funções utilizadas e sua finalidade. 
	</p>
<p align="justify">A interface local exibe os dados de temperatura,umidade,pressão,luminosidade  com o histórico das dez últimas medições de cada medida além do ajuste do tempo de medição, A exibição dos dados do display foi organizada em menus. Cada medida possui um menu e um submenu, no menu é exibida a última medição realizada e um opção para acessar o histórico ao pressionar o <strong>botão 2</strong> , dentro do submenu do histórico são exibidos dois dados do histórico um em cada linha ao pressionar o <strong>botão 1</strong> os dados são alternados indo dois primeiros aos dois últimos e ao pressionar o <strong>botão 2</strong> o display volta ao menu anterior que estava, para alternar entre os menus o <strong>botão 1</strong> deve ser pressionado permitindo que sejam exibidos 5 menus diferentes.O último menu é o menu de alteração do tempo de sensoriamento discutido abaixo.</p>

<p align="justify">O menu de  alterar tempo de sensoriamento exibe o tempo atual do intervalo de medição e a opção para alterar o tempo de medição ao pressionar o <strong>botão 2</strong>, no submenu de alterar o tempo de exibição são exibidos 7 dígitos que podem ser incrementados de 0-9 para definir o intervalo de tempo novo, os dois primeiros dígitos correspondem ao valor de minutos, o terceiro e quarto dígitos correspondem ao tempo em segundos e os trẽs últimos ao tempo em milisegundos, para alternar entre os dígitos o <strong>botão 1</strong> é usado, para incrementar o digito o <strong>botão 2</strong> é pressionado os valores para o dígito vão de 0 até 9 de modo circular, para sair do submenu e alterar o tempo de medição o <strong>botão 3</strong> de ser pressionado.
</p>
	<p align="center"><img src ="imagens/modelo interface.png"></p>
<p align="justify">A logica de controle do display foi implementada como uma máquina de estados, para cada estado listado uma configuração diferente é exibida no display, com essa abordagem pode-se garantir uma progressão lógica na exibição da interface e a facilidade de inserir novos estados se fossem necessários. A variável <strong>estado</strong> guarda o estado atual para o display, os estados são definidos no arquivo SBC.h, a implementação da máquina de estados seguiu a abordagem com o uso do switch-case assim em cada case do switch as instrunções para o display são definidas, a lógica da mudança de estado é feita com base no estado atual e com os botões, nos menus e submenus as transições ocorrem quando os botões <strong>B1</strong> e <strong>B2</strong> são pressionados apenas com a exceção do submenu de alterar tempo em que só ocorre uma transição de estado quando <strong>botão 3</strong> é pressionado.
</p>
	<p align="center"><img src ="imagens/estados display.png"></p>
<p align="justify">O programa da interface é rodado numa thread em que possui um loop infinito que executa infinitamente o switch-case	
</p>
<p align="center"><img src ="imagens/loop do switch-case display.jpg"></p>
<p align="justify">
Ao final do loop é verificado se um botão foi pressionado, se algum botão é pressionado o display é limpo para escrita do novo estado
</p>
```s
  if(!b0 || !b1 || !b2){  //Limpa o display se algum botão foi pressionado
         lcdClear(lcd);      
  }
```

<p align="justify"> O switch verifica qual o case para o estado atual e então imprime no display os dados para a configuração atual(funções lcdPrint e lcdPosition) </p>
<p align="center"><img src ="imagens/switch case statment e logica de mudança de estado.jpg"></p>
<p align="justify"> No fim de cada case há um trecho que verifica a lógica da mudança de próximo estado, baseado no estado atual e 	em qual botão foi pressionado a variável estado é atualizada para o próximo estado
</p>

<h1>Descrição e análise dos testes realizados</h1>

<h1>Referências</h1>

<h1>Como executar o projeto</h1>

<h2>Interface Desktop</h2>

<h2>SBC</h2>

