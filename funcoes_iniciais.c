typedef struct lis{
    Position pos;
    int dist;
    struct lis *prox;
} ponto;
typedef ponto *Ponto;

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


Ponto mapearPontos(Grid *G, Position eu, Robot *R){
    /* Varre a grid hexagonal olhando as casas em cada direção
    do demônio e cria uma lista com todos os pontos de controle
    por ordem de distância.
    Essa função foi bastante inspirada pela função searchNearestControl,
    do controller_basic.c */
    Ponto controles = NULL;
    for(int i = 0; i < 6; i++){
        int dist = 1 + quickTurn(R->dir, i);
        Position pos = getNeighbor(eu, i);
        while(valid(pos, G->m, G->n, G)){
            if(isControlPoint(G, pos)){
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
    do controller_basic.c
    LEVAR EM CONTA AS RODADAS ATÉ CHEGAR */
	int i, min = 500, dist;
    Position best_bot;
	for(i = 0; i < 6; i++) {
		dist = 1;
		Position s = getNeighbor(p,i);
		while(valid(s, g->m, g->n, g)) {
			if(s->type == ROBOT) {  // vou ter que usar strcmp???
				if(dist < min) {
					min = dist;
					best_bot = s;
					break;
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
