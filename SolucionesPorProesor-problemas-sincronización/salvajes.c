#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define M 10

char* progname;

int caldero = 0;
pthread_mutex_t m;
pthread_cond_t empty, full;

void putServingsInPot(int m)
{
	caldero += m;
	printf("Llenando caldero %d\n", caldero);

}

int getServingsFromPot(int id)
{
	printf("Salvaje %d sirviendo ración del caldero %d\n",
			id, caldero);
	caldero--;
}

void eat(int id)
{
	printf("Salvaje %d comiendo\n", id);
	sleep(rand() % 5);
}

void* salvaje(void* arg)
{
	int id = (int) arg;

	while(1) {
		pthread_mutex_lock(&m);
		while (caldero == 0) {
			pthread_cond_signal(&empty);
			pthread_cond_wait(&full, &m);
		}
		getServingsFromPot(id);
		pthread_mutex_unlock(&m);
		eat(id);
	}

}

void* cocinero(void* arg)
{
	while(1) {
		pthread_mutex_lock(&m);
		while (caldero != 0)
			pthread_cond_wait(&empty, &m);
		putServingsInPot(M);
		pthread_cond_broadcast(&full);
		pthread_mutex_unlock(&m);
	}
}


void usage() {
	printf("%s Num_de_salvajes\n", progname);
}

int main(int argc, char *argv[])
{
	int n,i;
	char *endptr;
	pthread_t *tid;
	pthread_t tid_cook;

	progname = argv[0];

	if (argc < 2) {
		usage();
		exit(EXIT_FAILURE);
	}

	n = strtol(argv[1], &endptr, 10);
	if (*endptr != '\0'){
		usage();
		exit(EXIT_FAILURE);
	}

	tid = malloc(n * sizeof(pthread_t));
	if (tid == NULL){
		perror("malloc tid");
		exit(EXIT_FAILURE);
	}

	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&empty, NULL);
	pthread_cond_init(&full, NULL);

	for (i = 0; i < n; i++)
		pthread_create(&tid[i], NULL, salvaje, (void*) i);

	pthread_create(&tid_cook, NULL, cocinero, NULL);

	for (i = 0; i < n; i++)
		pthread_join(tid[i], NULL);

	pthread_join(tid_cook, NULL);

	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&full);
	pthread_cond_destroy(&empty);

	return 0;
}
