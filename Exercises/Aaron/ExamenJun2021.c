#include <pthread.h>

#define NUM_THREADS 5

typedef struct thread_data {
    char* name;
    int num_id;
} thread_data_t

void* fun(void* args) {

}

int main(){
    pthread_t threads[NUM_THREADS];

    for(int i = 0; i < NUM_THREADS; i++) {
        thread_data_t data;
        data.name = 'A' + i;
        data.num_id = i + 1;

        pthread_create(&threads[i], NULL, fun, (void *)data);
    }

    for (int j = 0; j < count; j++) {
        pthread_join(threads[i], NULL);
    }

    exit(0);    
}