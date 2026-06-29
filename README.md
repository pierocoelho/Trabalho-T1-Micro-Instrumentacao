# Sensor de Chuva para Limpador de Para-Brisa

> Projeto integrado das disciplinas **Instrumentação** e **Microcontroladores e Sistemas Microcontrolados** do Instituto Mauá de Tecnologia.

---

## Integrantes

- Aline Miyuki Arakaki — RA 21.00252-5
- Enzo Jun Yasuda — RA 20.00363-3
- Lucas Lopes de Assis — RA 22.00012-7
- Piero Rutkowski Coelho — RA 23.00470-3

## Professores

- Andressa Corrente Martins
- Rodrigo de Marca França

---

## Estrutura do Software
Todo o firmware do projeto foi desenvolvido em linguagem C, utilizando o SDK oficial do Raspberry Pi Pico (RP2040). A implementação foi organizada em módulos lógicos dentro do arquivo main.c, facilitando a compreensão do funcionamento do sistema.

## Inicialização do Hardware
Ao iniciar a execução, o programa realiza a configuração de todos os periféricos utilizados no projeto:
- GPIOs responsáveis pelo controle do motor;
- PWM para controle da potência aplicada ao motor DC;
- Conversor Analógico-Digital (ADC) para leitura do sensor capacitivo;
- Display LCD 16x2 utilizado como interface com o usuário;
- LED integrado da placa, utilizado como indicador de funcionamento.

Após a inicialização, o display apresenta uma mensagem indicando que o sistema foi iniciado corretamente.

## Leitura do Sensor
O sensor capacitivo fornece um sinal analógico proporcional à quantidade de água presente sobre sua superfície.

Esse sinal é adquirido continuamente pelo ADC interno do RP2040, permitindo que o sistema monitore, em tempo real, o nível de umidade.

## Processamento dos Dados
O firmware compara continuamente o valor obtido pelo ADC com faixas previamente definidas durante a etapa de calibração experimental.

Cada faixa corresponde a um nível de umidade, permitindo classificar automaticamente o funcionamento do limpador em quatro estados:

- Nível 0: motor desligado;
- Nível 1: acionamento a cada 4 segundos;
- Nível 2: acionamento a cada 2 segundos;
- Nível 3: funcionamento contínuo.

Essa abordagem simplifica a tomada de decisão e torna o comportamento do sistema previsível e facilmente ajustável.

## Controle do Motor
Após determinar o nível de umidade, o software controla um motor DC através do módulo ponte H L298N.

O motor é acionado utilizando PWM com aproximadamente 50% de potência. O intervalo entre os acionamentos varia conforme o nível de umidade detectado, simulando o comportamento de um limpador de para-brisa automotivo.

## Interface com o Usuário
Sempre que ocorre mudança no nível de funcionamento, o display LCD é atualizado automaticamente, informando o estado atual do sistema.

As mensagens apresentadas são:
- Motor: DESLIGADO
- Motor: NÍVEL 1
- Motor: NÍVEL 2
- Motor: NÍVEL 3

A atualização ocorre somente quando há alteração do estado, evitando escritas desnecessárias no display.

## Estrutura do Programa
O funcionamento do firmware pode ser resumido no seguinte ciclo:
1. Inicialização dos periféricos;
2. Leitura contínua do sensor;
3. Processamento da leitura analógica;
4. Definição do nível de umidade;
5. Atualização do display LCD;
6. Controle do motor;
7. Retorno ao início do ciclo.

Esse processo é executado continuamente enquanto o sistema permanece ligado, permitindo resposta praticamente em tempo real às variações de umidade detectadas pelo sensor.

## Objetivo
Desenvolver um sistema embarcado capaz de detectar automaticamente a presença de água utilizando um sensor capacitivo de umidade, processar o sinal por meio de um Raspberry Pi Pico (RP2040) e acionar um mecanismo que simula o funcionamento de um limpador de para-brisa. O sistema também exibe o nível de funcionamento do motor em um display LCD.

## Descrição Geral
Este projeto consiste em um sistema automatizado para acionamento de um limpador de para-brisa a partir da detecção de umidade. Um sensor capacitivo mede a presença de água, um circuito realiza o condicionamento do sinal e o Raspberry Pi Pico interpreta a leitura analógica para controlar um motor DC através de um driver L298N. O estado do sistema é apresentado em um display LCD.

## Funcionamento
1. O sensor detecta a presença de água.
2. O circuito eletrônico condiciona o sinal.
3. O RP2040 realiza a leitura pelo ADC.
4. O firmware classifica o nível de umidade.
5. O driver L298N aciona o motor.
6. O LCD informa o nível atual do sistema.

## Diagrama de Blocos

<img width="940" height="291" alt="image" src="https://github.com/user-attachments/assets/9819b00c-3799-44e3-b5b5-bce4d1e359c6" />

> *Figura 1 - Diagrama de blocos*

## Componentes Utilizados
- Raspberry Pi Pico (RP2040)
- Sensor capacitivo de umidade
- Driver L298N
- Motor DC
- Display LCD 16x2
- PCB de condicionamento de sinais
- Diodo 1N4007
- Capacitores de 10 nF e 1 µF
- Resistores de 10 kΩ e 1 MΩ
- Fonte de alimentação
- Estrutura mecânica impressa em 3D

## Condicionamento de Sinais
O sensor funciona como um capacitor cuja capacitância varia conforme a quantidade de água presente. O circuito converte essa variação em um sinal DC proporcional ao nível de umidade, que é lido pelo ADC do RP2040.

## Níveis de Operação
Nível 0: acima de 1,8 V → Motor desligado

Nível 1: 1,8 a 1,3 V → Acionamento a cada 4 s

Nível 2: 1,3 a 0,9 V → Acionamento a cada 2 s

Nível 3: abaixo de 0,9 V → Funcionamento contínuo


## Foto do Projeto
<img width="939" height="626" alt="image" src="https://github.com/user-attachments/assets/74478340-9fa4-4a5a-816b-267155f3559c" />

> *Figura 2 - Sensor Final*

## Software
O firmware foi desenvolvido em linguagem C utilizando o SDK oficial do Raspberry Pi Pico. O código implementa leitura ADC, controle PWM do motor, temporização do limpador e interface com display LCD.

## Resultados
Os testes demonstraram que o sistema detecta corretamente diferentes níveis de umidade e ajusta automaticamente a frequência de funcionamento do limpador, exibindo o nível correspondente no LCD.

## Limitações
- Protótipo validado apenas em laboratório.
- Necessidade de calibração.
- Sensível à distribuição da água.
- Não mede diretamente a intensidade da chuva.
- Influência das condições ambientais.

## Organização do Repositório
Este repositório contém todos os arquivos utilizados durante o desenvolvimento do projeto, incluindo código-fonte, documentação, arquivos de hardware, modelos mecânicos e registros experimentais.

## Apresentação
Arquivo: Apresentação Sensor de Chuva

## Relatório Técnico
Arquivo: Relatório Técnico.pdf

## Vídeo de Demonstração
Vídeo e foto final do projeto estão no repositório dentro da pasta (Arquivos->Fotos->Sensor Final).

## Pasta Código
Contém todas as versões desenvolvidas do firmware em linguagem C para o Raspberry Pi Pico (RP2040). As diferentes versões foram mantidas para documentar a evolução do desenvolvimento e facilitar consultas ao histórico do projeto.

## Pasta Fotos
Esta pasta reúne toda a documentação visual do projeto e está organizada em três subpastas:
- Fotos: imagens registradas durante as etapas de desenvolvimento e montagem;
- Sensor Final: fotos e vídeo do protótipo concluído;
- Vídeos: gravações dos testes e validações realizadas ao longo do desenvolvimento. Os nomes dos arquivos descrevem o conteúdo de cada vídeo.

## Pasta PCB
Contém todos os arquivos utilizados para o desenvolvimento e fabricação das placas de circuito impresso (PCB) empregadas no projeto, incluindo os arquivos necessários para produção.

## Pasta SolidWorks
Reúne todos os arquivos CAD utilizados na modelagem da estrutura mecânica impressa em 3D, permitindo a reprodução ou futuras modificações do protótipo.

##Apresentação Sensor de Chuva (.pdf)
Arquivo utilizado na apresentação final do projeto, contendo as explicações do funcionamento do sistema, diagramas, resultados experimentais e as atualizações realizadas após as sugestões dos professores.

##Experimental Excel & Porcentagem de Umidade (.xlsx)
Planilha contendo todos os dados experimentais obtidos durante a calibração do sensor, incluindo as medições de tensão, conversão para porcentagem de umidade e os cálculos utilizados para definir os níveis de funcionamento do sistema. O procedimento de coleta dos dados encontra-se registrado em vídeo na pasta Fotos/Vídeos.

##Relatório Técnico (.pdf)
Relatório completo do projeto, apresentando a fundamentação teórica, metodologia, resultados experimentais, discussão e conclusões.
