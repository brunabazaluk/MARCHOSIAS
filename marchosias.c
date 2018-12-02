/* Este programa foi escrito por: Bruna Bazaluk, Gustavo Bastos e Rafael Tsuha. */
/* Utilizamos algumas funcoes ja existentes de Yan Couto e Ricardo Fonseca.     */
/* As funcoes escritas por eles sao as seguintes: 				*/
/* int valid; int isControlPoint; int quickTurn; int fastTurn; POE O RESTO AQUI DEPOIS NAO ESQUECE*/
/**/

///////////////////////////////////////////////////////////////////////////////////
/* Marchosias eh um marques do inferno, um demonio com 31 legioes em seu comando. */
/* Nao espere que esse demonio tenha piedade do seu robo. 			 */
/* Marchocias magnus Marchio est. Se ostentat specie lupae ferocissimae cum alis  */
/* gryphi, cauda serpentina, & ex ore nescio quid evomens. Quum hominis imaginem  */
/* induit, pugnator est optimus. Ad quaesita vere respondet: fidelis in cunctis   */
/* exorcistae mandatis. Fuit ordinis Dominationum. Huic subjacent legiones 	 */
/* triginta. Sperat se post mille ducentos annos ad septimum Thronum reversurum:  */
/* sed ea spe falsus est.*/
///////////////////////////////////////////////////////////////////////////////////


#include "../robot_fight.h"
#include <stdlib.h>

typedef struct lis{
    Position pos;
    int dist;
    struct lis *prox;
} ponto;
typedef ponto *Ponto;


//definicoes da fila
struct elemento{
	Position info;
	struct elemento *prox;
};
struct fila {
	struct elemento* head;
};

typedef  struct fila * Fila;
typedef struct elemento * tipoDaFila;


Fila criaFila ();
void destroiFila (Fila f);

tipoDaFila inicioDaFila (Fila f);
int filaVazia (Fila f);
void insereFila (Fila f, Position p);
int retiraFila (Fila f);
//fim das definicoes da fila

/*Checa se a posicao dada esta dentro do mapa e nao esta
sendo ocupada*/
int valid(Position p, int m, int n, Grid *g) {
	return ((p.x >= 0 && p.x < m && p.y >= 0 && p.y < n) && (g->map[p.x][p.y].type == NONE));
}

int isControlPoint(Grid *g, Position p) {
	return (g->map[p.x][p.y].isControlPoint);
}

/*Dada uma direcao inicial e uma direcao final, ve qual
o menor numero de viradas sao necessarias*/
int quickTurn(int ini, int end) {
	int i, j;
	for(i = ini, j = 0; i != end; i = (i+1)%6, j++)
	if (i >= 6) i-= 6;
	if (j > 3) j = 6-j;
	return j;
}
/*Dada uma direcao inicial e uma direcao final, ve
para qual lado virando eh mais rapido de se chegar*/
Action fastTurn(int ini, int end) {
	int dif = end-ini;
	if((dif <= 3 && dif >= 0) || (dif <= -3))
	return TURN_RIGHT;
	else
	return TURN_LEFT;
}

int distanciaEmTurnos (Grid *g, Position partida, Position chegada, Robot *r) {
/*devolve o tempo em turnos necessario para partir a posicao chegada e
  ir ate a posicao partida, ignorando possiveis obstaculos*/
	Fila f = criaFila ();
	Position p;
	int distancia;
	int mapa[30][30];
	for (int i = 0; i < 30; i++) {
		for (int j = 0; j < 30; j++) {
			mapa[i][j] = 0;
		}
	}
	
	p = chegada;
	mapa[p.x][p.y] = 1;
	insereFila (f, p);
	while (!filaVazia (f)) {
		for (int i = 0; i < 6; i ++) {
			p = getNeighbor (inicioDaFila(f)->info, i);
			if (p.x >= 0 && p.x < g->m && p.y >= 0 && p.y < g->n) {
				if (mapa[p.x][p.y] == 0){
					mapa[p.x][p.y] = mapa[inicioDaFila(f)->info.x][inicioDaFila(f)->info.y]+1;
					insereFila(f, p);
				}
			}
		}
		retiraFila(f);
	}
	//distancia em casas, sem considerar rotacao;
	distancia = mapa[chegada.x][chegada.y] -1;
	
	return distancia;
}

Ponto inserePonto(Ponto lista, Position p, int dist){
    /* Insere o ponto na lista de pontos ordenado
    por distância */
	Ponto novo = malloc(sizeof(ponto));
	novo->pos = p; novo->dist = dist;
    if(lista == NULL){
        novo->prox = NULL;
        return novo;
    }
    if(lista->dist > novo->dist){
		Ponto aux = malloc(sizeof(ponto));
		aux->pos = lista->pos;
		aux->dist = lista->dist;
		aux->prox = lista->prox;
        novo->prox = aux;
        return novo;
    }
    Ponto aux = lista;
    while(aux->prox != NULL && aux->prox->dist < novo->dist){
        aux = aux->prox;
    }
    novo->prox = aux->prox;
    aux->prox = novo;
    return lista;
}

Ponto mapearPontos(Grid *g, Position eu, Robot *r){
    /* Varre a grid hexagonal olhando as casas em cada direção
    do demônio e cria uma lista com todos os pontos de controle
    por ordem de distância.
    Essa função foi bastante inspirada pela função searchNearestControl,
    do controller_basic.c */
	int dist;
    Ponto controles = NULL;
    for(int i = 0; i < g->m; i++){
		for(int j = 0; j < g->n; j++){
			Position p; p.x = i; p.y = j;
			if(isControlPoint(g, p)){
				dist = distanciaEmTurnos(g, eu, p, r);
				controles = inserePonto(controles, p, dist);
			}
		}
    }
    return controles;
}

Position searchNearestRobot(Grid *g, Position p) {
    /* Essa função foi bastante inspirada pela função searchNearestControl,
    do controller_basic.c */
	int i, min = 500, dist;
    Position best_bot;
	for(i = 0; i < 6; i++) {  //  checa na direção de cada lado o hexágono por vez
		dist = 1;  //  como o ponto de controle
		Position s = getNeighbor(p,i);
		while((s.x >= 0 && s.x < g->m) && (s.y >= 0 && s.y < g->n)) {
			if(g->map[s.x][s.y].type == ROBOT) {
				if(dist < min) {
					int direc = i-3;
					if(i-3 < 0) direc += 6;
					min = dist + quickTurn(g->map[s.x][s.y].object.robot.dir, direc);  // leva em conta o número de vezes que o robo terá que virar;
					best_bot = s;
					break;
				}
			}
			dist++;
			s = getNeighbor(s, i);
		}
	}
	return best_bot;
}

int marchosias_modo;
Ponto maisProx;
void prepareGame(Grid *g, Position p, int turnCount){
	/*
	A ideia nessa função é:
	 	1. Mapear os pontos de controle do mapa por ordem de proximidade;
		2. Verificar se você é o robô mais próximo de algum deles;
		3. Se for:
			3.1. Mandar o sinal de correr até o ponto;
		4. Se não for:
			4.1. Mandar o sinal de ativar o modo de combate;
	*/

	setName("MARCHOSIAS");
	Ponto controlPoints = mapearPontos(g, p, &g->map[p.x][p.y].object.robot);

	Ponto checador = controlPoints;

	// TESTE
	while(checador != NULL){
		printf("x: %d, y:%d, dist:%d \n", checador->pos.x, checador->pos.y, checador->dist);
		checador = checador->prox;
	}
	// FIM DO TESTE

	maisProx = controlPoints;
	Position robo_prox = searchNearestRobot(g, maisProx->pos);
	while(maisProx != NULL && (p.x != robo_prox.x || p.y != robo_prox.y)){
		maisProx = maisProx->prox;
		if(maisProx == NULL) break;
		robo_prox = searchNearestRobot(g, maisProx->pos);
	}
	if(maisProx == NULL) marchosias_modo = 0; // MODO DE COMBATE
	else marchosias_modo = 1; // MODO DE CONTROLE
}


/* checa se vc consegue andar pra casa desejada sem levar um tiro */
/* caso um tiro estiver na msm posicao q vc pretende ir na prox rodada */
/* a funcao checa se a quantidade de tiros na sua direcao eh menor que */
/* a qtdade de bullets q marchosias tem, se sim, ele atira; se nao, ele*/
/* ele vira para mudar de casa na rodada seguinte */
Action andar(Grid *g, Position p, Position robo)
{
	Tile bloco;
	Position pos, linha;
	Direction ida, volta;
	int d = 0;
	pos = getNeighbor(p,d);
	bloco = g->map[pos.x][pos.y];
	int cont=0, m=pos.x, n=pos.y;


	/* checa os vizinhos da casa que vc pretende ir */
	for(ida = 0, volta = 0; ida <= 5 && volta <= 5; ida++, volta++)
	{
		if(bloco.type ==  PROJECTILE)
		{
			volta = bloco.object.projectile.dir;
			if(volta - ida == 3 || volta-ida == -3)
			{
				linha=pos;
				while(valid(linha, m, n, g))
				{
					if((g->map[pos.x][pos.y]).type == PROJECTILE) cont++;  //  pos ou linha?
					linha=getNeighbor(linha, ida);
				}

				robo=getNeighbor(linha,volta);
				if((g->map[robo.x][robo.y]).object.robot.bullets > cont)
				{
					if((g->map[robo.x][robo.y]).object.robot.dir ==2) return SHOOT_RIGHT;
					if((g->map[robo.x][robo.y]).object.robot.dir ==1) return SHOOT_LEFT;

				}
				else return TURN_LEFT;

			}

		}
		return WALK;
	}
	return WALK;

}



Fila criaFila () {
	Fila f;
	f = malloc (sizeof (struct fila));
	f->head = NULL;
	return f;
}
void insereFila (Fila f, Position p) {
	if (filaVazia (f)) {
		f->head = malloc (sizeof(struct elemento));
		f->head->prox = NULL;
		f->head->info = p;
	}
	else{
		struct elemento * aux = f->head;
		while (aux->prox != NULL) {
			aux = aux->prox;
		}
		aux->prox = malloc (sizeof(struct elemento));
		aux->prox->prox = NULL;
		aux->prox->info = p;
	}
}
int retiraFila (Fila f) {
	if (filaVazia (f)) {
		return 0;
	}
	else {
		struct elemento * aux = f->head;
		f->head = f->head->prox;
		free (aux);
	}
	return 1;
}

int filaVazia (Fila f) {
	if (f->head == NULL) 
		return 1;
	else 
		return 0;
}
tipoDaFila inicioDaFila (Fila f) {
	return f->head;
}

void destroiFila (Fila f)  {
	struct elemento * atual = f->head;

	while (atual != NULL) {
		struct elemento *tmp = atual->prox;
		free (atual);
		atual = tmp;
	}
	free (f);
}


int taVindoTiro (Grid *g, Position myPos, Direction d) {
/*verifica se esta vindo projetil da direcao d e o tempo em turnos
  q vai levar para o projetil chegar.
  retorna o tempo em turnos para o projetil chegar caso exista;
  retorna 0 caso nao haja projeteis vindo
  */

	Position pos = getNeighbor (myPos, d);
	int tempo = 0;

	while (pos.x >= 0 && pos.x < g->m && pos.y >= 0 && pos.y < g->n && g->map[pos.x][pos.y].type != ROBOT) {
		if (g->map[pos.x][pos.y].type == PROJECTILE) {
			if (g->map[pos.x][pos.y].object.projectile.dir == (d+3)%6) {
			//direcao oposta a d
				tempo += 1;
			}
		}
		pos = getNeighbor (myPos, d);
	}
	return tempo;
}

void metralhaGeral(Grid *g, Position p, Direction dir)
{
	Tile bloco;
	Position pos_inimigo,pos=p;

	pos_inimigo = searchNearestRobot(g, p);

	while do centro
		if (getNeighbor(pos, dir)==pos_inimigo)
			ATIRA LA
		else pos=getNeighbor(pos, dir);
		if(!valid(pos, m, n, g)) break;

	pos=p;
	while da direita
		if (getNeighbor(pos, (dir+1)%6)==pos_inimigo)
			ATIRA LA
		else pos=getNeighbor(pos, (dir+1)%6);
		if(!valid(pos, m, n, g)) break;
	pos=p;
	while da esquerda
		if  (getNeighbor(pos, (dir-1)%6)==pos_inimigo)
			ATIRA LA
		else pos=getNeighbor(pos, (dir-1)%6);
		if(!valid(pos, m, n, g)) break;
}

Action processTurn(Grid *g, Position p, int turnsLeft) {
	printf("%d\n", marchosias_modo);
	Robot *r = &g->map[p.x][p.y].object.robot;

	if(marchosias_modo == 1){ // MODO DE CONTROLE
		/* SE ESTIVER NO PONTO DE CONTROLE */
		if(isControlPoint(g,p)) {
			for (Direction d1 = 0; d1 < 6; d1++) {
				if (taVindoTiro(g, p, d1)) {
					if (d1 == ((r->dir)+2)%6 && taVindoTiro(g, p, d1) == 1) {
						return OBSTACLE_RIGHT;
					}
					else if (d1 == (r->dir+3)%6 && taVindoTiro(g, p, d1) == 1) {
						return OBSTACLE_CENTER;
					}
					else if (d1 == (r->dir+4)%6 && taVindoTiro(g, p, d1) == 1) {
						return OBSTACLE_LEFT;
					}
					else if (d1 == r->dir && taVindoTiro(g, p, d1) == 1) {
						return SHOOT_CENTER;
					}
					else if (d1 == (r->dir+1)%6 && taVindoTiro(g, p, d1) == 1) {
						return SHOOT_RIGHT;
					}
					else if (d1 == (r->dir+5)%6 && taVindoTiro(g, p, d1) == 1) {
						return SHOOT_LEFT;
					}
				}
			}
			return STAND;
		}
		else{
			/*

			FUNÇÃO PARA ANDAR ATÉ LÁ (USANDO A FUNÇÃO "andar" DA BRUNA)

			*/
			Position destino = maisProx->pos;
			return STAND;
		}
	}
	else {  // MODO DE COMBATE
		/* PRECISA CODAR ISSO AQUI */
		return STAND;
	}
}
