#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define true 1

#define NUM_PHILOSOPHERS 5


int philoStates[NUM_PHILOSOPHERS];

pthread_t philosophers[NUM_PHILOSOPHERS];
pthread_mutex_t forks[NUM_PHILOSOPHERS];

void init() {
    int i;
    for(i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_mutex_init(&forks[i], NULL);
}

void destroy(){
    int i;
    for(i = 0; i< NUM_PHILOSOPHERS; i++)
        pthread_mutex_destroy(&forks[i]);
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
        //We try to grab the right one, if its already grabbed, the thread waits
        pthread_mutex_lock(&forks[right]);

        //We try to grab the left one, locking its mutex. If succeded, returns 0 and we eat. Else, is already locked then we put back the right one 
        if(pthread_mutex_trylock(&forks[left])){  // is already locked
            pthread_mutex_unlock(&forks[right]);
            continue; //(?)
        }
        //Is 0 
        eat(nPhilosopher);

        // PUT FORKS BACK ON THE TABLE
        pthread_mutex_unlock(&forks[left]);
        pthread_mutex_unlock(&forks[right]);
        
        toSleep(nPhilosopher);
   }
}

int main()
{
    init();
    unsigned long i;
    for(i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_create(&philosophers[i], NULL, philosopher, (void*)i);
    
    for(i = 0; i < NUM_PHILOSOPHERS; i++)
        pthread_join(philosophers[i],NULL); // we wait each thread to terminate

    destroy();
    return 0;
} 
