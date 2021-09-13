#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/* pode-se compilar o programa com valores diferentes
 * para N. Por exemplo:
 * $ gcc -DN=4 -o main main.c
 */
#ifndef N
#define N 10
#endif

int vet[N];
int terminados = 0;
pthread_mutex_t mutex;
pthread_cond_t condvar;

struct thread_args {
	int thread_id;
	int acum; 	// resultado acumulado
};

void print_exit(char *msg, int retval) {
	fprintf(stderr, msg);
	exit(retval);
}

void popula_vetor(int *v, size_t len) {
	// inicializa seed aleatória
	srand(clock());
	for (int i = 0; i < len; i++) {
		int r = rand() % 10;
		v[i] = r;
	}
}

void *thread_func(void *vptr) {
	struct thread_args *args = (struct thread_args *)vptr;
	int i, iter;
	int soma = 0;
	int this_id = args->thread_id;
	for (iter = 0; iter < N; iter++) {
		// soma elementos do vetor
		for (i = 0; i < N; i++) {
			soma += vet[i];
		}
		pthread_mutex_lock(&mutex);
		fprintf(stderr, "Thread %d somou e adquiriu "
				"o lock (iteração %d)\n",
				this_id, iter);
		terminados += 1;
		// se nao for o ultimo, espera
		if (terminados < N) {
			fprintf(stderr, "Thread %d vai esperar "
					"(iteração %d)\n",
					this_id, iter);
			pthread_cond_wait(&condvar, &mutex);
		}
		/* quem for o último libera as outras threads e
		 * reseta variável `terminados`, para que a próxima
		 * etapa possa ser iniciada */
		else {
			fprintf(stderr, "Thread %d vai liberar "
					"demais threads (iteração %d)\n",
					this_id, iter);
			terminados = 0;
			pthread_cond_broadcast(&condvar);
		}
		pthread_mutex_unlock(&mutex);

		// repopula o vetor (cada thread uma posição)
		vet[this_id] = rand() % 10; // seed de rand() já foi inicializada
		pthread_mutex_lock(&mutex);
		fprintf(stderr, "Thread %d populou e adquiriu "
				"o lock (iteração %d)\n",
				this_id, iter);
		terminados += 1;
		// se não for o último, espera
		if (terminados < N) {
			fprintf(stderr, "Thread %d vai esperar "
					"(iteração %d)\n",
					this_id, iter);
			pthread_cond_wait(&condvar, &mutex);
		} else {
			fprintf(stderr, "Thread %d vai liberar "
					"demais threads (iteração %d)\n",
					this_id, iter);
			terminados = 0;
			pthread_cond_broadcast(&condvar);
		}
		pthread_mutex_unlock(&mutex);
	}
	fprintf(stderr, "Thread %d vai retornar "
			"(valor somado = %d)\n",
			this_id, soma);
	// retorna o resultado
	args->acum = soma;
	pthread_exit(NULL);
}

int main(void) {
	int vet[N];
	pthread_t threads[N];
	struct thread_args args[N];
	int thread;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condvar, NULL);

	// popula vetor com valores aleatorios
	popula_vetor(vet, N);
	
	/* cria threads */
	for (thread = 0; thread < N; thread++) {
		args[thread] = (struct thread_args){
			.thread_id = thread,
			.acum = 0,
		};
		if(pthread_create(&threads[thread],NULL,thread_func,&args[thread])) {
			print_exit("Erro ao inicializar thread.", EXIT_FAILURE);
		}
	}

	/* coleta threads */
	for (thread = 0; thread < N; thread++) {
		if(pthread_join(threads[thread],NULL)) {
			print_exit("Erro ao fazer join() na thread.", EXIT_FAILURE);
		}
		if (thread > 1) {
			/* verifica se o resultado obtido é compatível com o anterior */
			if (args[thread].acum != args[thread - 1].acum) {
				print_exit("Resultados diferem.", EXIT_FAILURE);
			}
		}
	}
	fprintf(stderr, "Sucesso!\n");

	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&condvar);

	return 0;
}
