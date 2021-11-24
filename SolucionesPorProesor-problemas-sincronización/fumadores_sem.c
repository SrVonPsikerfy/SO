#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


/* Esta versión no cumple las condiciones de la propuesta de Patil, en las que el
 * agente no se pude modificar y hace un post a un semáforo independiente por
 * cada ingrediente generado.
 */

#define M 10

char* progname;

const char * ing2str[] = {
"TABACO",
"CERILLAS",
"PAPEL"
};

int table[3] = {0};
sem_t m, agentq;
sem_t semsmokers[3];

void smoke(int ing0)
{
	printf("fumador con %s fumando\n", ing2str[ing0]);
	sleep(rand() % 5);
}

void * smokers(void *arg)
{
	int ing0, ing1, ing2;
	ing0 = (int) arg;
	ing1 = (ing0 + 1) % 3;
	ing2 = (ing0 + 2) % 3;

	while (1) {
		sem_wait(&semsmokers[ing0]);
		printf("fumador con %s accediendo a la mesa\n", ing2str[ing0]);
		sem_wait(&m);
		table[ing1] = 0;
		table[ing2] = 0;
		sem_post(&agentq);
		sem_post(&m);

		smoke(ing0);
	}
}

void * agent(void *arg)
{
	int i,j,k;

	srand((unsigned int) time(NULL));

	while (1) {
		i = rand() % 3;
		j = (i + 1) % 3;
		k = (i + 2) % 3;

		sem_wait(&m);
		table[j] = 1;
		table[k] = 1;
		printf("agent poniendo en la mesa %s y %s \n", ing2str[j], ing2str[k]);
		sem_post(&semsmokers[i]);
		sem_post(&m);
		sem_wait(&agentq);
	}
}

int main(int argc, char *argv[])
{
	int n,i;
	char *endptr;
	pthread_t tid_smokers[3];
	pthread_t tid_agent;

	progname = argv[0];

	sem_init(&m, 0, 1);
	sem_init(&agentq, 0, 0);

	for (i = 0; i < 3; i++)
		sem_init(&semsmokers[i], 0, 0);

	for (i = 0; i < 3; i++)
		pthread_create(&tid_smokers[i], NULL, smokers, (void*) i);

	pthread_create(&tid_agent, NULL, agent, NULL);

	for (i = 0; i < 3; i++)
		pthread_join(tid_smokers[i], NULL);

	pthread_join(tid_agent, NULL);

	sem_destroy(&m);
	sem_destroy(&agentq);

	for (i = 0; i < 3; i++)
		sem_destroy(&semsmokers[i]);

	return 0;
}
