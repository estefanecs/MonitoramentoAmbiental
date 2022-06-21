
<h1  align="center"> Protótipo de sistema para monitoramento ambiental </h1>

<h1>Apresentação do sistema</h1>
<p align="justify">O sistema proposto realiza o monitoramento ambiental no cenário da Internet das Coisas (IoT, do inglês, Internet of Things), no qual implementa  o protocolo de troca de mensagens Message Queue Telemetry Transport (MQTT). É divido em 3 módulos: SBC, Interface remota e Interface Local.</p>
<p align="center"><img src ="imagens/arquitetura do sistema.drawio.png"></p>
<p align="justify"> O protótipo é capaz de tratar e controlar medidas de temperatura, umidade, pressão atmosférica e luminosidade através de leitura de sensores. Esse módulo é chamado de Single Board Computer (SBC).  Além disso, o protótipo possui duas Interface Homem-Máquina (IHM) que apresenta em tempo real as medições de cada sensor, histórico com as 10 últimas medições de cada sensor, além de permitir o ajuste do intervalo de tempo entre as medições. A IHM local utiliza o display LCD e a IHM remota é uma interface desktop em JAVA.</p>

<p align=”justify”>Para o desenvolvimento do SBC utilizou-se o conceito de threads para que fosse possível realizar a leitura de forma simultânea a exibição das informações no display LCD, bem como a alteração do intervalo de tempo na interface. Optou-se por utilizar threads pela facilidade de implementação além de ser viável para a solução deste problema.</p>

<p align="justify">Para o desenvolvimento deste protótipo foram utilizados a Raspberry PI 0, o sensor DHT11, dois potenciômetros de 10k Ohm, display LCD 16x2, três botões, a linguagem de programação C e JAVA. </p>

<h1>Single Board Computer (SBC)</h1>
<p align="justify">O SBC é o módulo composto pela leitura de umidade e temperatura através do sensor digital DHT11, simulação dos sensores analógicos de luminosidade (BH1750) e pressão atmosférica (BMP180), além de ser responsável pelo gerenciamento desses dados e envio para a interface local (display LCD) e a interface remota. </p>

<p align="justify">O SBC possui o fluxo de execução principal (main) e duas threads: uma thread para a leitura dos dados dos sensores e envio dos dados para a IHM remota, e outra thread para a exibição no display LCD, dos dados lidos. Na <i>main</i> são criados os clientes MQTT para o envio e recebimento dos dados via o protocolo, configuração da biblioteca WiringPi, inicialização da biblioteca <i>Mosquitto</i> e criação das threads. Para a utilização de threads utilizou-se a biblioteca <i>pthread.h</i> </p>


<h2>Sensor DHT11</h2>

<h2>Sensor de luminosidade e pressão atmosférica</h2>
<p align="justify">O sensor de luminosidade e pressão atmosférica foi simulado por dois potenciômetros, que estão no canal 0 e 3 respectivamente da Raspberry Pi utilizada. Para o sensor de luminosidade simulou-se o <i>BH1750</i> que tem faixa de medição de 1 até 65535 LUX. Já para o sensor de pressão atmosférica, simulou-se o <i>BMP180</i> que possui faixa de medição de 300 a 1100 hPa.</p>

<p align="justify">O potenciômetro gera sinal analógico que é convertido para sinal digital, utilizando o ADS1115 que é um conversor de sinais analógicos I2C. Para essa conversão utilizou-se uma biblioteca criada pelo Embarcados (2019).</p>

<p align="justify">Na biblioteca foram utilizados 2 registradores de memória do ADS1115, que são acessados através do <i>Address Pointer Register</i>: <i>Conversion Register</i> que armazena os valores da conversão no formato de 16 bits e o<i>Config Register</i> para configuração de parâmetros. </p>

<p align="justify">A biblioteca configura os parâmetros MUX, PGA e MODE do registrador <i>Config Register</i>, antes de realizar a leitura dos dados. O parâmetro MUX configura o multiplexador para a leitura dos canais. O PGA(Programmable Gain Amplifier) configura a faixa de medição do conversor. O parâmetro MODE corresponde ao bit 8 e configura a forma de operação: <i>conversões continuamente</i> ou uma conversão e aguardo pela leitura. Foi configurado nesse projeto a segunda forma, inserindo o valor 1 no bit 8.</p>

<p align="justify">A interface l2C é inicializada com o endereço do barramento I2C <i>/dev/i2c-1</i>, que é passado como parâmetro da função:</p>

```
int openI2CBus(char *bus);
```
<p align="justify">A configuração do endereço do ADS1115 é através do pino <strong>ADDR</strong>. Nesse problema, utilizou-se o pino ADDR conectado ao GND, no qual o endereço correspondente é 0x48 em hexadecimal. A função que realiza essa configuração é:</p>

```
int setI2CSlave(unsigned char endereco);
```

<p align="justify">Por fim, para realizar a leitura e conversão do dado em sinal analógico para digital é utilizado a função <strong>readVoltage</strong>, que tem como parâmetro o canal do potenciômetro. A conversão do sinal lido é feito da seguinte forma:</p>

```
voltagem = (float)valorAnalogico*4.096/32767.0;
```

<p align="justify">Para transformar o sinal digital na medida equivalente dos sensores simulados, foi realizado o mapeamento com regra de três composta. A função têm como parâmetros o valor lido e convertido do potenciômetro, valor mínimo e máximo de tensão do potenciômetro e a faixa mínima e máxima de valor do sensor simulado. </p>

<p align="justify"> A conta realizada para o mapeamento resulta um valor equivalente de uma medida do sensor e é a seguinte: </p>

```
float fmap(float valorLido, float minPotenciometro, float maxPotenciometro, float minSensor, float maxSensor);
```

<h2>Protocolo Message Queuing Telemetry Transport (MQTT)</h2>

<h2>Histórico de medições</h2>
<p align="justify">
   Para garantir uma visualização do perfil das medidas do sensoriamento foi estabelecido um historico de dados
   que armazena as dez ultimas medições realizadas sendo visivel na interface local e remota além da SBC gerar um arquivo de texto com esse historico para garantir a persistencia dos dados   
</p>

<p align="justify">
   O Historico foi implementado como uma lista estática, usando um vetor de tamanho MAX definido no cabeçalho SBC.h e pelos ponteiros de indice L( ultima medida feita) e O( medida mais velha), foram definidas duas funções para o controle da estrutura do historico são elas ADD e getOrdenada.
 </p>
 <p align="justify">
    ADD insere um novo elemento no vetor na próxima posição para o ponteiro L, a posição do ponteiro L é baseada na ocupação do vetor, ao atingir o tamanho máximo o ponteiro passa para a posição da medida mais antiga assim sempre a medida mais antiga é sobrescrita e o ponteiro O é incrementado para uma próxima posição.
</p>
<p align="justify">
   A função getOrdenada criar um vetor do tipo Dados com os valores para o historico mas de forma ordenada da medição mais recente para mais antiga, essa função é usada para fornecer ao display um vetor ordenado para exibir o historico no display de modo sequencial sem nescessidade de ler o historico real na ordem de exibição. 
</p>
<h1>Interface Desktop</h1>
<p align="justify">A interface Desktop foi desenvolvida usando a linguagem Java e a biblioteca Paho para a implementação do cliente MQTT.</p>

<p align="justify">A interface Desktop comunica-se com o SBC através do protocolo MQTT. No qual é um ouvinte (subscriber) do broker, nos tópicos referentes a temperatura, umidade, luminosidade, pressão, tempo e nos tópicos referentes ao histórico das medições dos sensores. Além disso, a interface é um editor (publisher) no tópico de tempo, para que seja possível determinar o intervalo de tempo dentre as medições dos sensores.</p>

<p align="justify">Para armazenar os dados de cada sensor com consistência, foi criado uma classe <i>DadoSensores</i> no qual implementa o padrão singleton, possuindo assim uma única instância da classe. A classe possui os atributos do tipo <i>String</i> para as medidas do sensor, para o intervalo de tempo e armazena o histórico de cada sensor em <i>ArrayList</i>.</p>

<p align="justify">Quando uma mensagem é publicada nos tópicos, o ouvinte MQTT no método <i>messageArrived</i> verifica em qual tópico a mensagem foi publicada e salva o dado recebido no atributo correspondente na classe <i>DadoSensores</i>. Essa verificação é realizada utilizando a estrutura condicional <i>Switch case</i>.</p>

<p align="justify">Por exemplo, se a mensagem foi publicada no tópico <i>"monitoramentoAmbiental/luminosidade"</i>, o dado recebido será salvo alterando o valor do atributo <i>luminosidade</i> da classe <i>DadoSensores</i>.</p>

<p align="justify">Se o tópico for referente ao histórico de uma das medições, inicialmente a string de dados recebida é separada e convertida para um vetor. Em seguida, o atributo ArrayList da medição correspondente é limpo com o método <i>clear()</i>, e por fim, cada elemento do vetor contendo os dados recebidos é adicionado na ArrayList, atualizando assim o histórico atual.</p>

<p align="justify">Já a publicação no tópico <i>"monitoramentoAmbiental/tempo"</i> acontece sempre. Na interface há um campo para que o usuário insira o intervalo de tempo que deseja. Esse valor inserido na interface é convertido para String e em seguida para byte, e passado como parâmetro no método que realiza a publicação no tópico. É interessante ressaltar que o método publicar da biblioteca Paho tem como parâmetro o tópico, o dado a ser publicado no tipo Byte e a qualidade de serviço(0,1 ou 2). </p>

```
public void publicar(String topico, byte[] informacao, int qos);
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

```
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

```
java -jar MonitoramentoAmbiental.jar
```
<h2>SBC</h2>
<p align="justify">Os arquivos da pasta <i>SBC</i> devem ser salvos na Raspberry. Dentro da pasta contendo os arquivos baixados, execute no terminal os comandos:</p>

```
make

sudo ./sbc
```
