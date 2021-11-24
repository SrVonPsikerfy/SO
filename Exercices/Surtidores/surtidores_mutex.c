#include <pthread.h>

#define LIBRE 1
#define OCUPADO 0
//Friki
int surtidores[2] = {0, 0};
int ticketsTotales = 0;
int turnoActual = 0;

pthread_mutex_t m;
pthread_cond_t surt_libre;

void cliente(void * args) {

    // < declaración de variables locales >
    int dinero = (int) args;
    int surtidor;
    int mi_turno;

    // Comprobar que se cumplen los requisitos para repostar
    //REGION CRITICA
    pthread_mutex_lock(&m);
    mi_turno = ticketsTotales++;

    //Si no es mi turno y no hay ningun surtidores libres, pues hay que esperar
    while (mi_turno != turnoActual || (surtidores[0] == OCUPADO && surtidores[1] == OCUPADO) ){
        pthread_cond_wait(&surt_libre, &m);
    }

    surtidor = (surtidores[0] == LIBRE) ? 0 : 1;
    surtidores[surtidor] = OCUPADO;
    turnoActual++;
    pthread_cond_broadcast(&surt_libre); //Avisa que ha ocupado un surtidor. Desbloquea a todos los hilos que han sido bloqueados
    pthread_mutex_unlock(&m);
    //FIN REGION CRITICA
    ServirCombustible(surtidor,dinero);

    //REGION CRITICA
    //Ahora que ya ha repostado, libera el surtidor
    pthread_mutex_lock(&m);
    surtidores[surtidores] = LIBRE;
    pthread_cond_broadcast(&surt_libre);
    pthread_mutex_lock(&m);
    // Acciones de salida
}

//EL MAIN NOS LOS SUBE ÉL