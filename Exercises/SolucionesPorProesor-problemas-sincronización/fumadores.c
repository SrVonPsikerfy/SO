#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define M 10

char* progname;

const char * ing2str[] = {
"TABACO",
"CERILLAS",
"PAPEL"
};

int table[3] = {0};

pthread_mutex_t m;
pthread_cond_t empty, full;

void smoke(int ing0)
{
	printf("fumador con infinito %s fumando\n", ing2str[ing0]);
	sleep(rand() % 5);
}

void * smokers(void *arg)
{
	int i, j, k;
	i = (int) arg;
	j = (i + 1) % 3;
	k = (i + 2) % 3;

	while (1) {
		pthread_mutex_lock(&m);
		while (!table[j] || !table[k])
			pthread_cond_wait(&full, &m);

		table[j] = 0;
		table[k] = 0;
		pthread_cond_signal(&empty);
		pthread_mutex_unlock(&m);

		smoke(i);
	}
}

void * agent(void *arg)
{
	int i, j;

	while (1) {
		i = rand() % 3;
		j = (i + 1 + (rand() % 2)) % 3;

		pthread_mutex_lock(&m);
		while (table[0] || table[1] || table[2])
			pthread_cond_wait(&empty, &m);

		table[i] = 1;
		table[j] = 1;
		printf("agente poniendo en la mesa %s y %s\n", ing2str[i], ing2str[j]);
		pthread_cond_broadcast(&full);
		pthread_mutex_unlock(&m);
	}
}

int main(int argc, char *argv[])
{
	int n,i;
	char *endptr;
	pthread_t tid_smokers[3];
	pthread_t tid_agent;

	progname = argv[0];

	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&empty, NULL);
	pthread_cond_init(&full, NULL);

	for (i = 0; i < 3; i++)
		pthread_create(&tid_smokers[i], NULL, smokers, (void*) i);

	pthread_create(&tid_agent, NULL, agent, NULL);

	for (i = 0; i < 3; i++)
		pthread_join(tid_smokers[i], NULL);

	pthread_join(tid_agent, NULL);

	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&full);
	pthread_cond_destroy(&empty);

	return 0;
}
