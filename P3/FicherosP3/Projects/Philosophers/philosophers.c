#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define true 1

#define NUM_PHILOSOPHERS 5

enum STATES { HUNGRY, EATING, RESTING };

int philoStates[NUM_PHILOSOPHERS];

pthread_t philosophers[NUM_PHILOSOPHERS];
pthread_mutex_t forks[NUM_PHILOSOPHERS];

void init() {
    int i;
    for(i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i], NULL);
}

void think(int i) {
    printf("Philosopher %d thinking... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d stopped thinking!!! \n" , i);

}

void eat(int i) {
    printf("Philosopher %d eating... \n" , i);
    sleep(random() % 5);
    printf("Philosopher %d is not eating anymore!!! \n" , i);

}

void toSleep(int i) {
    printf("Philosopher %d sleeping... \n" , i);
    sleep(random() % 10);
    printf("Philosopher %d is awake!!! \n" , i);
    
}

void* philosopher(void* i) {
    int nPhilosopher = (int)i;
    int right = nPhilosopher;
    int left = (nPhilosopher - 1 == -1) ? NUM_PHILOSOPHERS - 1 : (nPhilosopher - 1);
    while(true) {
        
        think(nPhilosopher);
        
        /// TRY TO GRAB BOTH FORKS (right and left)
        // pthread_mutex_lock(&forks[nPhilosopher]);
        //We verify if we're HUNGRY since later we need to put back the forks on the table

        if(!pthread_mutex_trylock(&forks[right]) && !pthread_mutex_trylock(&forks[left]) ){
            
        }




        eat(nPhilosopher);
        
        // PUT FORKS BACK ON THE TABLE
        
        toSleep(nPhilosopher);
        // if(philoStates[nPhilosopher] == HUNGRY && philoStates[right] != EATING && philoStates[left] != EATING){
        //    philoStates[nPhilosopher] = EATING;
        // }
   }

}

int main()
{
    init();
    unsigned long i;
    for(i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void*)i);
    
    for(i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL);
    return 0;
} 
