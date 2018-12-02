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

int taVindoTiro (Grid *g, Position myPos, Direction d) {
/*verifica se esta vindo projetil da direcao d e o tempo em turnos 
  q vai levar para o projetil chegar.
  retorna o tempo em turnos para o projetil chegar caso exista;
  retorna 0 caso nao haja projeteis vindo
  */

	Position pos = getNeighbor (myPos, d);
	int tempo = 0;

	while (pos.x >= 0 && pos.x < g.m && pos.y >= 0 && pos.y < g.n && g->map[pos.x][pos.y].type != ROBOT) {
		if (g->map[pos.x][pos.y].type == PROJECTILE) {
			if (g->map[pos.x][pos.y].object.projectile.dir == (d+3)%6) {
			//direcao oposta a d
				tempo += 1;
			}
		}
	}
	return tempo;
}



Action processTurn(Grid *g, Position p, int turnsLeft) {
	int i, j;
	Position s;
	Robot *r = &g->map[p.x][p.y].object.robot;

	/*Se estiver em cima de um control point, SCORE TIME*/
	if(isControlPoint(g,p))
		if (taVindoTiro()) {
			
		}
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
