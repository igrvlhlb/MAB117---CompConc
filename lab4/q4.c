#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 4

// pelo que observei dessa tarefa, não precisamos de uma variável de condição.
// Um lock basta para garantir a execução correta do programa, pois queremos que
// a thread 2 execute primeiro, a 1 e 4 (em qualquer ordem) em seguida e, por fim,
// a thread 3.
//
// Para isso, basta definirmos uma variável compartilhada entre as threads (global)
// que conta quantas threads já imprimiram, garantindo que somente uma thread a acesse
// por vez (mutex lock).
// A lógica do programa é:
// -- Se nenhuma thread tiver imprimido, `concluidas`
// valerá 0 e somente a thread 2 poderá executar e incrementar a variável;
// -- Se 1 ou 2 threads já tiverem imprimido (`concluidas` = 1 ou 2), tanto a thread
// 1 quanto a 4 poderão executar (em qualquer ordem) e incrementar a variável
// -- Se `concluidas` valer 3, todas as threads, com a exceção da 3, já imprimiram,
// então a 3 pode imprimir e o programa termina

pthread_mutex_t mutex; // mutex de secao critica
int concluidas = 0; // quantas threads terminaram

// essa thread deve executar depois
// da thread 2 e antes da 3, podendo
// executar antes ou depois da 4.
// Ou seja, `concluidas` deve valer 1 ou 2
void *thread1(void *arg) {
	// fazemos um while que só se encerrará
	// caso a thread imprima
	while (1) {
		pthread_mutex_lock(&mutex);
		// so imprime se o valor de `concluidas` permitir
		if (concluidas > 0 && concluidas < NTHREADS - 1) {
			printf("Fique a vontade.\n");
			concluidas += 1;
			break; // termina while
		}
		// libera lock para outra thread poder executar
		// a seção crítica e recomeça o while()
		pthread_mutex_unlock(&mutex);
	}
	// se sair do while(), devemos libear o lock!
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *thread2(void *arg) {
	// esta thread será a primeira, então
	// não precisa fazer nenhuma verificação
	// (mesmo assim precisamos pegar o lock)
	pthread_mutex_lock(&mutex);
	printf("Seja bem vindo!\n");
	concluidas += 1;
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *thread3(void *arg) {
	while (1) {
		pthread_mutex_lock(&mutex);
		if (concluidas == NTHREADS - 1) {
			printf("Volte sempre!\n");
			// nao precisamos incrementar
			// `concluidas`, pois esta será
			// a última thread
			break;
		}
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

// essa thread deve executar depois
// da thread 2 e antes da 3, podendo
// executar antes ou depois da 1.
// Ou seja, `concluidas` deve valer
// 1 ou 2
void *thread4(void *arg) {
	// fazemos um while que só se encerrará
	// caso a thread imprima
	while (1) {
		pthread_mutex_lock(&mutex);
		// so imprime se o valor de `concluidas` permitir
		if (concluidas > 0 && concluidas < NTHREADS - 1) {
			printf("Sente-se por favor\n");
			concluidas += 1;
			break; // termina while
		}
		// libera lock para outra thread poder executar
		// a seção crítica e recomeça o while()
		pthread_mutex_unlock(&mutex);
	}
	// se sair do while(), devemos libear o lock!
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}

void *(*funcoes[NTHREADS])(void *) = {
	thread1,
	thread2,
	thread3,
	thread4
};

int main(void) {
	pthread_t threads[NTHREADS];
	pthread_mutex_init(&mutex, NULL);
	int thread;

	for (thread = 0; thread < NTHREADS; thread++) {
		pthread_create(&threads[thread], NULL, funcoes[thread], NULL);
	}

	for (thread = 0; thread < NTHREADS; thread++) {
		pthread_join(threads[thread], NULL);
	}

	pthread_mutex_destroy(&mutex);

	return 0;
}
