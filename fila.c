#include <stdio.h>
#include <stdlib.h>
typedef struct {//ja tem no .h do jogo
	int x,y;
} Position;

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

/*#######################################################
	ESTA MAIN EH UMA PEQUENA ROTINA DE TESTE PARA A FILA
#########################################################
int main () {
	
	Position p1, p2, p3;
	p1.x = 10; p1.y = 15;
	p2.x = 20; p2.y = 25;
	p3.x = 30; p3.y = 35;

	Fila f = criaFila (f);
	
	insereFila (f, p1);
	insereFila (f, p2);
	insereFila (f, p3);
	
	printf ("inicio: %d %d\n",f->head->info.x, f->head->info.y);
	retiraFila (f);
	printf ("inicio: %d %d\n",f->head->info.x, f->head->info.y);
	retiraFila (f);
	printf ("inicio: %d %d\n",f->head->info.x, f->head->info.y);
	retiraFila (f);
	printf ("inicio: %d %d\n",f->head->info.x, f->head->info.y);

	destroiFila (f);

	return 0;
}

*/
