#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define DEFAULTN 16
#define DEFAULTSUP 2

char *progname;
sem_t m, freesup;

int ticket = 0;
int turn = 0;
int nsup = DEFAULTSUP;
int *supplies;
#define FREE 0
#define BUSY 1

typedef struct {
	int id;
	int money;
} tharg;

void init_thargs(tharg *thargs, int n)
{
	int i;
	srandom((unsigned int) time(NULL));

	for (i = 0; i < n; i++) {
		thargs[i].id = i;
		thargs[i].money = random() % 60;
	}
}

void do_supply(int id, int tick, int sup, int money)
{
	printf("client %d with ticket %d getting fuel for %d$ on supply %d\n",
			id, tick, money, sup);
	sleep(money / 20);

	printf("client %d on supply %d done\n", id, sup);
}

int free_supply(int *sup)
{
	int i = 0;
	int isfree = 0;

	while ((i < nsup) && (supplies[i] == BUSY))
		i++;

	if (i < nsup) {
		*sup = i;
		isfree = 1;
	}

	return isfree;
}

void *client(void *arg)
{
	int id = ((tharg *) arg)->id;
	int money = ((tharg *) arg)->money;
	int t,s;

	sem_wait(&freesup);
	sem_wait(&m);
	t = ticket++;
	free_supply(&s);
	supplies[s] = BUSY;
	sem_post(&m);

	do_supply(id, t, s, money);

	sem_wait(&m);
	supplies[s] = FREE;
	sem_post(&freesup);
	sem_post(&m);
}

void usage(void)
{
	printf("%s [-n number_of_clients] [-s number_of_supplies] [-h]\n", progname);
}


int parse_int_or_die(void)
{
	int n;
	char *endptr;

	if (!optarg || *optarg == '\0') {
		usage();
		exit(EXIT_FAILURE);
	}

	n = strtol(optarg, &endptr, 10);
	if (*endptr != '\0') {
		usage();
		exit(EXIT_FAILURE);
	}
	return n;
}

int main(int argc, char *argv[])
{
	char c;
	int i;
	int n = DEFAULTN;
	tharg* thargs;
	pthread_t* tids;

	progname = argv[0];

	while ((c = getopt(argc, argv, "n:s:h")) != -1) {

		switch (c) {
		case 'n':
			n = parse_int_or_die();
			break;
		case 's':
			nsup = parse_int_or_die();
			break;
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}

	supplies = malloc(nsup * sizeof(int));
	if (!supplies) {
		perror("supplies malloc");
		exit(EXIT_FAILURE);
	}
	for (i = 0; i < nsup; i++) {
		supplies[i] = FREE;
	}

	thargs = malloc(n * sizeof(tharg));
	if (thargs == NULL) {
		perror("thargs malloc");
		exit(EXIT_FAILURE);
	}
	init_thargs(thargs, n);

	sem_init(&m, 0, 1);
	sem_init(&freesup, 0, nsup);

	tids = malloc(n * sizeof(pthread_t));
	if (tids == NULL) {
		perror("tids malloc");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < n; i++)
		pthread_create(&tids[i], NULL, client, (void *) &thargs[i]);

	for (i = 0; i < n; i++)
		pthread_join(tids[i], NULL);

	free(tids);
	free(thargs);
	free(supplies);

	sem_destroy(&m);
	sem_destroy(&freesup);

	return 0;
}
