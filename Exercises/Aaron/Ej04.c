
#include <pthread.h>

#define TABACO 0
#define PAPEL 1
#define CERILLAS 2

pthread_mutex_t isTaking;

pthread_cond_t fumTaked;
pthread_cond_t agentUpdate;

pthread_t fum[3];
pthread_t agent;

int buf[2];
bool hasBeenTaked = false;

void* fumador(void* args) {
    int elem = (int)(long long int)args;

    while(1) {
        pthread_mutex_lock(&isTaking);

        while (buf[0] != elem && buf[1] != elem)
            pthread_cond_wait(&fumTaked, &isTaking);
        
        hasBeenTaked = true;

        pthread_cond_signal(&agentUpdate);

        pthread_mutex_unlock(&isTaking);

        printf("fumador con infinito %s fumando\n", ing2str[ing0]);
	    sleep(rand() % 5);
    }
}

void* agente(void* args) {
     while(1) {
        pthread_mutex_lock(&isTaking);
        do {
            buf[0] = rand() % 3;
            buf[1] = rand() % 3;
        } while (buf[0] == buf[1]);
        pthread_cond_broadcast(&fumTaked);

        while(!hasBeenTaked)
            pthread_cond_wait(&agentUpdate, &isTaking);
        
        hasBeenTaked = false;

        pthread_mutex_unlock(&isTaking);
     }
}

int main(){
    pthread_mutex_init(&isTaking);
    pthread_cond_init(&agentUpdate, NULL);
	pthread_cond_init(&fumTaked, NULL);

    for (size_t i = 0; i < 3; i++) {
        pthread_create(&fum[i], NULL, fumador, (void *)i);
    }
    pthread_create(&agent, NULL, agente, (void *)i);

    pthread_mutex_destroy(&isTaking);
    pthread_cond_destroy(&fumTaked);
	pthread_cond_destroy(&agentUpdate);
}