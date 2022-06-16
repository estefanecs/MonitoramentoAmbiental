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
		
	Ao final do loop é verificado se um botão foi pressionado, se algum botão é pressionado o display é limpo para escrita do novo estado
	
	O switch verifica qual o case para o estado atual e então imprime no display os dados para a configuração atual(funções lcdPrint e lcdPosition)
	No fim de cada case há um trecho que verifica a logica da mudança de proximo estado, baseado no estado atual e em qual botão foi pressionado a varivel estado é atulizada para o proximo estado

	
Sensoriamento

Protocolo de comunicação MQTT

Interface Remota