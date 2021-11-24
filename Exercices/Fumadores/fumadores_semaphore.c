#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

int caldero = 0;
sem_t empty;
sem_t full;
sem_t cola_salvajes;

void eat(int id){
    printf("Salvaje %d comiendo", id);
    sleep(1 + rand()% 3);
}

void ServingInPot(int M){

    //Esperar a que el caldero este vacio
    sem_wait(&empty);
    caldero += M; //rellenar caldero
    printf("Llenando caldero %d\n", caldero);
    //Avisar salvajes de que se ha llenado
    sem_post(&full);
}

void getServingFromPot(int id){
    //Si no hay, avisar al cocinero y esperar, en ese orden
    sem_wait(&cola_salvajes);
    if(caldero == 0){
        sem_post(&empty); //post a la linea 13, para que el cocinero rellene
        sem_wait(&full); //bloqueamos para que en la linea 17 se desbloqueeeeeee
    }
    printf("Salvaje %d sirviendo ración del caldero %d\n", id, caldero);

    //Si hay, tomamos una racion
    caldero--;
    sem_post(&cola_salvajes);
}

int main(int argc, char *argv[]){
    sem_init(&empty, 0,1); //1 porque esta vacio
    sem_init(&cola_salvajes, 0,1); //1 para que solo pase el primero
    sem_init(&full, 0,0); //
}