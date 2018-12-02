#include "../robot_fight.h"

/*Controller_basic -- Codename: SleepyTron*/
/*Criado e desenvolvido pelo meu melhor amigo, Ricardo Fonseca*/
/*                                            

	Essa inteligencia artificial defensiva
	fica em busca dos preciosos	pontos de
	controle, e ao encontrar o mais proximo,
	permanece parada, dormindo em seu solo
	sagrado, e usufruindo de seus deliciosos pontos.

*/



/*Checa se a posicao dada esta dentro do mapa e nao esta sendo ocupada*/
int valid(Position p, int m, int n, Grid *g) {
	return ((p.x >= 0 && p.x < m && p.y >= 0 && p.y < n) && (g->map[p.x][p.y].type == NONE));
}

void prepareGame(Grid *g, Position p, int turnCount) {
	setName("inputAI");
	/*Um verdadeiro guerreiro nao precisa preparar nada*/

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

	char acao[20];
	printf("s: STAND\nol: OBSTACLE_LEFT\noc: OBSTACLE_CENTER\nor: OBSTACLE_RIGHT\nw: WALK\ntl: TURN_LEFT\ntr: TURN_RIGHT\nsl: SHOOT_LEFT\nsc: SHOOT_CENTER\nsr: SHOOT_RIGHT\n");
	scanf("%s", acao);
	if (!strcmp (acao, "st") ) {
		return STAND;
	}else
	if (!strcmp (acao, "ol") ) {
		return OBSTACLE_RIGHT;
	}else
	if (!strcmp (acao, "oc") ) {
		return OBSTACLE_CENTER;
	}else
	if (!strcmp (acao, "or") ) {
		return OBSTACLE_RIGHT;
	}else
	if (!strcmp (acao, "w") ) {
		return WALK;
	}else
	if (!strcmp (acao, "tl") ) {
		return TURN_LEFT;
	}else
	if (!strcmp (acao, "tr") ) {
		return TURN_RIGHT;
	}else
	if (!strcmp (acao, "sl") ) {
		return SHOOT_LEFT;
	}else
	if (!strcmp (acao, "sc") ) {
		return SHOOT_CENTER;
	}else
	if (!strcmp (acao, "sr") ) {
		return SHOOT_RIGHT;
	}else return STAND;


	/*Se estiver em cima de um control point, SCORE TIME*/
}
