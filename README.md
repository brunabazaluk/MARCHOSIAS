### MARCHOSIAS

##### OBSERVAÇÕES IMPORTANTES:
* para pontuar em um ponto de controle eh necessario
 realizar acao NENHUMA no turno;

* acertar um tiro vale duas vezes mais pontos que ficar
 parado num ponto de controle;

* obstaculos duram 5 turnos e nao podem ser destruidos por tiros;

* o efeito de colocar obstaculos em cima de obstaculso eh [nenhum]

* o jogo crasha quando tentamos colocar um obstaculo em cima de
 outro jogador

* o jogo crasha se tentarmos inicializa-lo com mais de 4 jogadores
 [provavelmente pq na main existe um vetor com exatamente 4 cores
 qdo o jogo tenta acessar a quinta cor, ele crasha]

* nao eh permitido 'trapacear' -> as variaveis recebidas pelas nossas
funcoes devem apenas ser lidas

#### Proposta para modos de comportamento da IA:

##### MODO VAI PARA PONTO DE CONTROLE:
1. Verifica qual o ponto de controle mais proximo de vc
   que de qualquer outro jogador;
2. Calcula melhor rota;
3. Segue pela melhor rota caso nao colida com nada no passo
   seguinte (tiros / robos);
4. Caso haja colisao diferente de barreira por tras: _**coloca uma barreira**_
5. Caso haja colisao com tiro pela frente: 
	_**atira** sse numero de tiros pela frente + numero de balas do
	atirador for menor ou igual que nosso total de balas.
	_**vira para direita ou esquerda e anda**_ caso contrario



##### MODO NO PONTO DE CONTROLE:
1. Fica parado
2. caso haja tiros vindo de direcoes diferentes: _**coloca barreira no que vem por tras;**_
3. atira na mesma direcao de onde vem o tiro e nao tem barreira;

##### MODO COMBATE:
1. Procura um robo numa linha reta em relacao a posicao atual;
2. Atira nessa direcao se nosso numero de balas for maior ou igual
   que o numero de balas do jogador alvo;

#### Colaboradores: (assinem ai)
* Tsu
* Gustavo
