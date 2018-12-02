#include "../robot_fight.h"
#include <stdlib.h>

typedef struct lis{
    Position pos;
    int dist;
    struct lis *prox;
} ponto;
typedef ponto *Ponto;

static int move_time = 0;
static int shoot = 1;
static int turn = 0;

/*Checa se a posicao dada esta dentro do mapa e nao esta
sendo ocupada*/
int valid(Position p, int m, int n, Grid *g) {
	return ((p.x >= 0 && p.x < m && p.y >= 0 && p.y < n) && (g->map[p.x][p.y].type == NONE));
}

Ponto inserePonto(Ponto lista, Ponto novo){
    /* Insere o ponto na lista de pontos ordenado
    por distância */
    if(lista == NULL){
        novo->prox = NULL;
        return novo;
    }
    if(lista->dist > novo->dist){
        novo->prox = lista;
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
    Ponto controles = NULL;
    for(int i = 0; i < 6; i++){
        int dist = 1 + quickTurn(r->dir, i);
        Position pos = getNeighbor(eu, i);
        while(valid(pos, g->m, g->n, g)){
            if(isControlPoint(g, pos)){
                aux = malloc(sizeof(ponto));
                aux->pos = pos;
                aux->dist = dist;
                controles = inserePonto(controles, aux);
            }
            dist++;
            pos = getNeighbor(pos, i);
        }
    }
    return Ponto;
}

Position searchNearestRobot(Grid *g, Position p) {
    /* Essa função foi bastante inspirada pela função searchNearestControl,
    do controller_basic.c */
	int i, min = 500, dist;
    Position best_bot;
	for(i = 0; i < 6; i++) {  //  checa na direção de cada lado o hexágono por vez
		dist = 1;  //  como o ponto de controle
		Position s = getNeighbor(p,i);
		while(valid(s, g->m, g->n, g)) {
			if(s->type == ROBOT) {
				if(dist < min) {
					min = dist + quickTurn(s->object->dir, (i-3)%6);  // leva em conta o número de vezes que o robo terá que virar;
					best_bot = s;
					//break;  // precisa continuar procurando na linha, pois pode ter outro bot que precise girar menos
				}
			}
			dist++;
			s = getNeighbor(s, i);
		}
	}

	/* Nao existe bot perto (impossivel??) */
	if (min == 500)
		return -1;

	else
		return best_bot;
}

int marchosias_modo;
void prepareGame(Grid *g, Position p, int turnCount){
	/* A ideia nessa função é:
	 	1. Mapear os pontos de controle do mapa por ordem de proximidade;
		2. Verificar se você é o robô mais próximo de algum deles;
		3. Se for:
			3.1. Mandar o sinal de correr até o ponto;
		4. Se não for:
			4.1. Mandar o sinal de ativar o modo de combate; */
	Ponto controlPoints = mapearPontos(g, p, g->object);
	Ponto checador = controlPoints;
	while(checador != NULL && (p != searchNearestRobot(g, checador->pos)))
		checador = checador->prox;
	if(checador == NULL) marchosias_modo = 0; // MODO DE COMBATE
	else marchosias_modo = 1; // MODO DE CONTROLE
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


/*Jill esta pronta para o tiroteio*/
Action shootTime(Grid *g, Position p, Robot *r) {
	int j;
	Position s;
	/*Hora de atirar! Jill vai ficar rotacionando
	em sentido horario e atirando, ate que ela ache
	necessario mudar de posicao*/
	if(move_time == 0){
		j = rand()%200;
		if(j < 5) {
			move_time = 1;
		}
		if(shoot){
			shoot = 0;
			turn = 1;
			return SHOOT_CENTER;
		}
		else {
			shoot = 1;
			turn = 0;
			return TURN_RIGHT;
		}
	}
	/*Jill percebeu que eh hora de surpreender
	o inimigo, e mudar sua posicao tatica*/
	else {
		move_time = 0;
		s = getNeighbor(p, r->dir);
		if(valid(s, g->m, g->n, g)){
			return WALK;
		}
		else
			return SHOOT_CENTER;
	}
	/*Jill para um momento para refletir sobre a vida*/
	return STAND;
}

/*COISAS DO RUNNER*/


/*Segura a direcao aleatoria que Billy esta seguindo*/
static int turn_dir = -1;



/*Escolha uma direcao validao aleatoria. Se
apos 20 tentativas nao encontrar nenhuma, manda ficar
parado.*/
int chooseDir(Grid *g, Position p) {
	int i, j;
	Position s;
	i = rand() % 6;
	s = getNeighbor(s, i);
	j = 0;
	while(!valid(s, g->m, g->n, g) && j < 20) {
		i = rand() % 6;
		s = getNeighbor(s, i);
		j++;
	}
	if (j == 10)
		return -1;
	else
		return i;
}

/*Como Billy faz sua pseudo-magia*/
Action run(Grid *g, Position p, Robot *r) {
	int i;
	Position s;
	/*Se Billy ja esta em sua direcao de vida, ele
	a persegue sem fim... ou ate encontrar um
	obstaculo.*/
	if(r->dir == turn_dir) {
		s = getNeighbor(p, turn_dir);
		if(valid(s, g->m, g->n, g)){
			return WALK;
		}
		turn_dir = -1;
	}
	/*Billy chegou aos limites fisicos do mundo, e
	precisa encontrar uma nova direcao de vida*/
	if(turn_dir == -1) {
		i = chooseDir(g, p);
		if (i == -1)
			return STAND;
		else {
			turn_dir = i;
		}
	}
	return fastTurn(r->dir, turn_dir);
}


/* COISAS DO CONTROLE */


static int control_dir;
/*Checa se a posicao dada esta dentro do mapa e nao esta sendo ocupada*/


int isControlPoint(Grid *g, Position p) {
	return (g->map[p.x][p.y].isControlPoint);
}
/*Dado uma posicao, checa se para alguma direcao
existe um control point, e retorna qual direcao esta
o mais perto, contando giradas necessárias*/
int searchNearestControl(Grid *g, Position p, Robot *r) {
	int i, min = 500, best_dir = 0, cont;
	for(i = 0; i < 6; i++) {
		/*Conta para chegar o numero de viradas necessarias
		ja que elas gastam um turno*/
		cont = 1 + quickTurn(r->dir, i);
		Position s = getNeighbor(p,i);
		while(valid(s, g->m, g->n, g)) {
			if(isControlPoint(g,s)) {
				if(cont < min) {
					min = cont;
					best_dir = i;
					break;
				}
			}
			cont++;
			s = getNeighbor(s, i);
		}
	}

	/*Nao existe control points no mapa*/
	if (min == 500)
		return -1;

	else
		return best_dir;
}


/* checa se vc consegue andar pra casa desejada sem levar um tiro */
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

			}
		}
		return WALK;

	}
}

Action processTurn(Grid *g, Position p, int turnsLeft) {
	int i, j;
	Position s;
	Robot *r = &g->map[p.x][p.y].object.robot;

	/*Se estiver em cima de um control point, SCORE TIME*/
	if(isControlPoint(g,p))
		return STAND;

	else {
		/*procura algum control point em alguam direcao do robo*/
		control_dir = searchNearestControl(g, p, r);
		/*Caso em nenhuma direcao tem um control point livre
		andar em uma direcao valida, ou comeca a virar para uma direcao valida*/
		if (control_dir == -1) {
			for(i = r->dir, j = 0; j < 6; i++,j++){
				if (i >= 6) i-=6;
				s = getNeighbor(p,i);
				if(valid(s, g->m, g->n, g)) {
					if(i == r->dir) {
						return WALK;
					}
					else {
						return fastTurn(r->dir, i);
					}
				}
			}
			/*Se nenhuma posicao em volta eh valida, SAD TIME*/
			return STAND;
		}
		/*Se encontrou um control point em alguma direcao,
		 comeca a virar e andar em sua direcao*/
		else if(control_dir == r->dir)
			return WALK;
		else {
			return fastTurn(r->dir, control_dir);
		}
	}
}
