#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 5

typedef struct thread_data
{
    char *name;
    int num_id;
    int order;
    int numP;
    int *idxP;
} thread_data_t;

pthread_mutex_t m;
pthread_cond_t executed;

int execution_order = 1;

int indexProcess[NUM_THREADS] = {0, 0, 0, 0, 0};

void *fun(void *args)
{
    thread_data_t *infoTh = (thread_data_t *)args;

    int isProcess;   
    do{
        isProcess = 1;     
        pthread_mutex_lock(&m);
        for (size_t i = 0; i < infoTh->numP; i++)
            if(!indexProcess[infoTh->idxP[i]]) {
                // printf("Hilo %d atascado por hilo %d\n", infoTh->num_id, infoTh->idxP[i]);
                isProcess = 0; break;
            }
        
        if(!isProcess)
            pthread_cond_wait(&executed, &m);
        pthread_mutex_unlock(&m);

    } while(!isProcess);

    sleep(rand() % 3 + 1);
    printf("Hilo %s con id %d\n", infoTh->name, infoTh->num_id);

    pthread_mutex_lock(&m);
    infoTh->order = execution_order++;
    indexProcess[infoTh->num_id - 1]= 1;
    pthread_cond_broadcast(&executed);
    pthread_mutex_unlock(&m);

    pthread_exit(0);
}

int num(int i)
{
    switch (i)
    {
    case 0:
        return 0;
    case 1:
        return 0;
    case 2:
        return 1;
    case 3:
        return 2;
    case 4:
        return 1;
    }

    return -1;
}

int *idx(int i)
{
    int *buf = NULL;

    switch (i)
    {
    case 0:
        return buf;
    case 1:
        return buf;
    case 2:
        buf = malloc(sizeof(int));
        buf[0] = 0;
        return buf;
    case 3:
        buf = malloc(sizeof(int));
        buf[0] = 0;
        buf[1] = 1;
        return buf;
    case 4:
        buf = malloc(sizeof(int));
        buf[0] = 1;
        return buf;
    }

    return buf;
}

char *loadstr(FILE *file)
{

	// reads chars until the end of a string --> [\0] or
	// the end of file [EOF]
	int c, numChars = 1;
	while ((c = getc(file)) != '\n')
	{
		if (c == EOF) {
            // numChars--;
			break;
        }
        // printf("%d - %d\n", c, numChars);
		numChars++;
	}

	// return to the original start point in the stream
	fseek(file, -numChars, SEEK_CUR);

	// reserve space in heap and then store the string on the variable
	char *strVar = malloc(numChars);
	fgets(strVar, numChars, file);
    // if(strVar[numChars - 1] == '\n'){
        // strVar[numChars - 1] = '\0';
    // }

	// read \n not read by fgets()
	// fgetc(file);

	// returns string (may be NULL)
	return strVar;
}

int main(int argc, char *argv[])
{
    FILE *file = fopen(argv[1], "r");
    if(file == NULL)
        exit(-1);

    srand(time(NULL));

    pthread_t threads[NUM_THREADS];
    thread_data_t data[NUM_THREADS];

    pthread_mutex_init(&m, NULL);
    pthread_cond_init(&executed, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        data[i].name = loadstr(file);
        data[i].num_id = i + 1;
        data[i].numP = num(i);
        data[i].idxP = idx(i);

        pthread_create(&threads[i], NULL, fun, (void *)&data[i]);
    }

    for (int j = 0; j < NUM_THREADS; j++)
        pthread_join(threads[j], NULL);

    for (int i = 0; i < NUM_THREADS; i++)
        printf("%s(%d) ", data[i].name, data[i].order);

    pthread_cond_destroy(&executed);
    pthread_mutex_destroy(&m);

    exit(0);
}