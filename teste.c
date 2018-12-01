#include "../robot_fight.h"
#include <stdlib.h>

static int move_time = 0;
static int shoot = 1;
static int turn = 0;

/*Checa se a posicao dada esta dentro do mapa e nao esta
sendo ocupada*/
int valid(Position p, int m, int n, Grid *g) {
	return ((p.x >= 0 && p.x < m && p.y >= 0 && p.y < n) && (g->map[p.x][p.y].type == NONE));
}

void prepareGame(Grid *g, Position p, int turnCount) {

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


/*Dada uma direcao inicial e uma direcao final, ve
para qual lado virando eh mais rapido de se chegar*/
Action fastTurn(int ini, int end) {
	int dif = end-ini;
	if((dif <= 3 && dif >= 0) || (dif <= -3))
		return TURN_RIGHT;
	else
		return TURN_LEFT;		
}

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

/*Simples e concisa, Billy so precisa correr para viver*/
Action processTurn(Grid *g, Position p, int turnsLeft) {
	Robot *r = &g->map[p.x][p.y].object.robot;
	
	return run(g, p, r);
}

/*Com apenas um pensamento, Jill entra no campo de batalha:
só poder haver um campeão*/
Action processTurn(Grid *g, Position p, int turnsLeft) {
	Robot *r = &g->map[p.x][p.y].object.robot;
	
	return shootTime(g, p, r);
}
