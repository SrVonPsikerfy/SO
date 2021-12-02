#include <pthread.h>

int caldero = 0;
pthread_mutex_t m;
pthread_cond_t full;
pthread_cond_t empty;

void ServingInPot(int M){
    //Wait calderon to be void
    pthread_mutex_lock(&m);
    while(caldero != 0) pthread_cond_wait(&empty, &m);

    //refill, calderon is void
    caldero = M;
    //tell the wilds
    pthread_cond_broadcast(&full);
    pthread_mutex_unlock(&m);
}

void getServingFromPot(){
    //wait calderon to have at leat 1 portion
    pthread_mutex_lock(&m);
    while(caldero == 0){
        //Is void
        //We tell the productor to refill before we get blocked
        pthread_cond_signal(&empty);
        pthread_cond_wait(&full, &m);
    } 
    //we take one portion
    caldero--;

    pthread_mutex_unlock(&m);
}

//We insert num of wilds to attend
int main(int argc, char *argv[]){

}
