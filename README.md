
<h1  align="center"> Protótipo de sistema para monitoramento ambiental </h1>

Executar dentro da pasta dist,no terminal: java -jar MonitoramentoAmbiental.jar

<h1>Apresentação do sistema</h1>

<h1>Single Board Computer (SBC)</h1>

<h2>Sensor DHT11</h2>

<h2>Sensor de luminosidade e pressão atmosférica</h2>

<h2>Protocolo Message Queuing Telemetry Transport (MQTT)</h2>

<h2>Histórico de medições</h2>

<h1>Interface Desktop</h1>
<p align="justify">A interface Desktop foi desenvolvida usando a linguagem Java e a biblioteca Paho para a implementação do cliente MQTT.</p>

<p align="justify">A interface Desktop comunica-se com o SBC através do protocolo MQTT. No qual é um ouvinte (subscriber) do broker, nos tópicos referentes a temperatura, umidade, luminosidade, pressão, tempo e nos tópicos referentes ao histórico das medições dos sensores. Além disso, a interface é um editor (publisher) no tópico de tempo, para que seja possível determinar o intervalo de tempo dentre as medições dos sensores.</p>

<p align="justify">Para armazenar os dados de cada sensor com consistência, foi criado uma classe <i>DadoSensores</i> no qual implementa o padrão singleton, possuindo assim uma única instância da classe. A classe possui os atributos do tipo <i>String</i> para as medidas do sensor, para o intervalo de tempo e armazena o histórico de cada sensor em <i>ArrayList</i>.</p>

<p align="justify">Quando uma mensagem é publicada nos tópicos, o ouvinte MQTT no método <i>messageArrived</i> verifica em qual tópico a mensagem foi publicada e salva o dado recebido no atributo correspondente na classe <i>DadoSensores</i>. Essa verificação é realizada utilizando a estrutura condicional <i>Switch case</i>.</p>

<p align="justify">Por exemplo, se a mensagem foi publicada no tópico <i>"monitoramentoAmbiental/luminosidade"</i>, o dado recebido será salvo alterando o valor do atributo <i>luminosidade</i> da classe <i>DadoSensores</i>.</p>
<p align="justify">Se o tópico for referente ao histórico de uma das medições, inicialmente a string de dados recebida é separada e convertida para um vetor. Em seguida, o atributo ArrayList da medição correspondente é limpo com o método <i>clear()</i>, e por fim, cada elemento do vetor contendo os dados recebidos é adicionado na ArrayList, atualizando assim o histórico atual.</p>

<p align="justify">Já a publicação no tópico <i>"monitoramentoAmbiental/tempo"</i> acontece sempre. Na interface há um campo para que o usuário insira o intervalo de tempo que deseja. Esse valor inserido na interface é convertido para String e em seguida para byte, e passado como parâmetro no método que realiza a publicação no tópico. É interessante ressaltar que o método publicar da biblioteca Paho tem como parâmetro o tópico, o dado a ser publicado no tipo Byte e a qualidade de serviço(0,1 ou 2). </p>
```s
   public void publicar(String topico, byte[] informacao, int qos)
```
<p align="justify">Como a interface implementa o padrão MVC, há um controlador que é responsável pela troca de informações entre a model e a view. Sendo assim, é nele que são criados os clientes MQTT editor e ouvintes de cada tópico. Os clientes MQTT conectam-se ao broker <strong>10.0.0.101</strong> com o usuário <strong>aluno</strong> e senha <strong>aluno*123</strong>. Esse broker foi criado em uma rede local, mas caso deseja-se é possível alterar o endereço para um broker disponível na internet, assim como o usuário e senha. </p>

<p align="justify">A interface possui duas telas, uma para a exibição dos dados dos sensores e outra para a exibição do histórico das medições de cada sensor. As telas implementam o Runnable, e ficam sempre atualizando os valores do campos exibidos, além de ficar sempre enviando o valor do campo de alterar o intervalo de tempo entre as medições.</p>
<h2>Modo de uso</h2>
<p align="justify">Ao executar o programa, será exibida uma tela de monitoramento de todos os sensores: temperatura, umidade, luminosidade e pressão atmosférica. Os dados exibidos correspondem aos mais recentes enviados pelo SBC. Além disso, exibe a informação do intervalo de tempo em que as medições estão sendo realizadas.</p>

<p align="justify">Para alterar o tempo das medições, o usuário deve inserir o valor no campo abaixo dos dados dos sensores. Nesta mesma tela há um botão <i>“Histórico de medições”</i>, que ao ser clicado direciona para a tela contendo os históricos de cada sensor.</p>

<p align="justify"> Na tela de histórico, são exibidos o histórico de cada sensor, bem como a data e hora em que cada medida foi realizada. Para voltar para a tela de monitoramento, é necessário que o usuário clique o botão <i>“Voltar”, que está no canto inferior esquerdo.</i></p>

<h1>Interface Local</h1>
<p align="justify"> Para o acesso e visualização dos dados dos sensores no SBC, foi construída uma interface homem máquina local que viabiliza o usuário interagir com o SBC.</p>

<p align="justify"> A interface foi construída usando um display LCD 16x2, e push buttons. Para o controle do display LCD foi utilizada a biblioteca LCD.h e para uso dos botões e GPIO da Raspberry foi utilizada a biblioteca wiringPi. As bibliotecas definem as funções para a comunicação com os dispositivos, abaixo tem-se a listagem das funções utilizadas e sua finalidade. </p>

<p align="justify">A interface local exibe os dados de temperatura, umidade, pressão atmosférica e luminosidade, com o histórico das dez últimas medições de cada sensor, além do ajuste do tempo de medição.</p>

<p align="justify"> A exibição dos dados do display foi organizada em menus, no qual cada medida possui um menu e um submenu. No menu é exibida a última medição realizada e um opção para acessar o histórico ao pressionar o <strong>botão 2</strong>. Dentro do submenu do histórico são exibidos dois dados do histórico, um em cada linha, ao pressionar o <strong>botão 1</strong> os dados são alternados indo dos dois primeiros aos dois últimos e ao pressionar o <strong>botão 2</strong> o display volta ao menu anterior que estava. Para alternar entre os menus, o <strong>botão 1</strong> deve ser pressionado permitindo que sejam exibidos 5 menus diferentes. O último menu é o de alteração do tempo de sensoriamento, que será discutido abaixo.</p>

	<p align="justify">O menu de alterar o tempo de sensoriamento exibe o tempo atual do intervalo de medição e a opção para alterá-lo ao pressionar o <strong>botão 2</strong>. No submenu de alterar o tempo de medição, são exibidos 7 dígitos que podem ser incrementados de 0-9 para definir o novo intervalo.  Os valores para cada dígito vão de 0 até 9 de modo circular. Os dois primeiros dígitos correspondem ao valor de minutos, o terceiro e quarto dígitos correspondem ao tempo em segundos e os três últimos ao tempo em milisegundos. Para alternar entre os dígitos, o <strong>botão 1</strong> é usado, para incrementar o digito o <strong>botão 2</strong> é pressionado. Para sair do submenu e alterar o tempo de medição o <strong>botão 3</strong> deve ser pressionado.</p>

<p align="center"><img src ="imagens/modelo interface.png"></p>

	<p align="justify">A lógica de controle do display foi implementada como uma máquina de estados. Para cada estado listado, uma configuração diferente é exibida no display, com essa abordagem pode-se garantir uma progressão lógica na exibição da interface e a facilidade de inserir novos estados se fossem necessários.</p>

<p align="justify">A variável <strong>estado</strong> guarda qual o estado atual do display, e estes estados estão definidos no arquivo SBC.h. A implementação da máquina de estados seguiu a abordagem com o uso do switch-case, assim, em cada case do switch as instruções para o display são definidas. A lógica da mudança de estado é feita com base no estado atual e com os botões. Nos menus e submenus as transições ocorrem quando os botões <strong>B1</strong> e <strong>B2</strong> são pressionados, apenas com a exceção do submenu de alterar tempo em que só ocorre uma transição de estado quando o <strong>botão 3</strong> é pressionado.</p>

<p align="center"><img src ="imagens/estados display.png"></p>

<p align="justify">O programa da interface é rodado numa thread em que possui um loop que executa infinitamente o switch-case.</p>

<p align="center"><img src ="imagens/loop do switch-case display.jpg"></p>

<p align="justify">Ao final do loop é verificado se um botão foi pressionado. Se algum botão é pressionado o display é limpo para escrita do novo estado.</p>
	```s
  		 if(!b0 || !b1 || !b2){  //Limpa o display se algum botão foi pressionado
            lcdClear(lcd);      
        }
```
<p align="justify"> O switch verifica qual o case para o estado atual e então imprime no display os dados para a configuração atual (funções lcdPrint e lcdPosition).</p>

<p align="center"><img src ="imagens/switch case statment e logica de mudança de estado.jpg"></p>

<p align="justify"> No fim de cada <i>case</i> há um trecho que verifica a lógica da mudança do próximo estado, baseado no estado atual e em qual botão foi pressionado, e desta forma a variável estado é atualizada para o próximo estado.</p>

<h1>Descrição e análise dos testes realizados</h1>

<h1>Referências</h1>

<h1>Como executar o projeto</h1>
<h2>Interface Desktop</h2>
<p align="justify">Para executar a interface desktop é necessário ter o Java instalado na máquina. Dentro da pasta <i>MonitoramentoAmbiental->dist</i> abra o terminal e execute o comando:</p>
```s
  java -jar MonitoramentoAmbiental.jar
```
<h2>SBC</h2>
<p align="justify">Os arquivos da pasta <i>SBC</i> devem ser salvos na Raspberry. Dentro da pasta contendo os arquivos baixados, execute no terminal o comando:</p>
```s
 make
```

