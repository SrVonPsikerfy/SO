#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>

#define NUM_STOPS 5	 // número de paradas de la ruta
#define MAX_USERS 40 // capacidad del autobús
#define USERS 4		 // numero de usuarios

#define DRIVING 0 // autobús en ruta
#define ON_STOP 1 // autobús en la parada

int state = DRIVING;   // estado inicial
int currStop = 0;	   // parada en la que se encuentra el autobus
int numPassengers = 0; // ocupantes que tiene el autobús

// personas que desean subir en cada parada
int waitingToGoIn[NUM_STOPS]; //= {0,0,...0};
// personas que desean bajar en cada parada
int waitingToGoOut[NUM_STOPS]; //= {0,0,...0};

//comunicación
pthread_mutex_t m;
//sincronización
pthread_cond_t bus, users;

void printInfo(const char * messi) {
	printf("%s", messi);
	for (int i = 0; i < NUM_STOPS; i++)
		printf("%d ", waitingToGoIn[i]);
	printf("\n");
}

void driveToNextStop()
{
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	printf("Bus DRIVING to the next stop \n\n");
	sleep(random() % 2);
	currStop = (currStop + 1 == NUM_STOPS) ? 0 : currStop + 1; // goes to the next stop
}

void busOnStop()
{
	/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Después se pone en marcha */
	pthread_mutex_lock(&m);

	printf("Bus has STOPPED on the stop %d\n\n", currStop);

	state = ON_STOP;

	printInfo("GoIn: "); printInfo("GoOut: ");

	pthread_cond_broadcast(&users);

	while ((numPassengers < MAX_USERS && waitingToGoIn[currStop] > 0) || waitingToGoOut[currStop] > 0) {
		printf("Bus waiting on the stop %d...\n\n", currStop);
		pthread_cond_wait(&bus, &m);
	}

	state = DRIVING;
	printf("Bus drove out the stop %d\n\n", currStop);

	pthread_cond_broadcast(&users);
	pthread_mutex_unlock(&m);
}

void *busFunc(void *args)
{
	while (1)
	{
		// esperar a que los viajeros suban y bajen
		busOnStop();
		// conducir hasta siguiente parada
		driveToNextStop();
	}
}

void goOnTheBus(int id_user, int og)
{
	/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
	el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&m);

												 //currStop == og &&
	while (state != ON_STOP || currStop != og || numPassengers == MAX_USERS) {
		printf("ID: %d User-IN waiting on stop %d...\n", id_user, og);
		pthread_cond_wait(&users, &m);
	}

	--waitingToGoIn[og];

	printf("ID: %d User got IN the bus on the stop %d\n", id_user, og);

	pthread_cond_signal(&bus);
	pthread_mutex_unlock(&m);
}

void goOutTheBus(int id_user, int dst)
{
	/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
	el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
	proporcionar información de depuración */
	pthread_mutex_lock(&m);

	while (state != ON_STOP || currStop != dst) {
		printf("ID: %d User-OUT waiting on stop %d...\n", id_user, dst);
		pthread_cond_wait(&users, &m);
	}

	--waitingToGoOut[dst];

	printf("ID: %d User got OUT the bus on the stop %d\n", id_user, dst);

	pthread_cond_signal(&bus);
	pthread_mutex_unlock(&m);
}

void user(int id_user, int og, int dst)
{
	// Esperar a que el autobus esté en parada origen para subir
	printf("ID: %d User is waiting on the stop %d to get IN the bus...\n\n", id_user, og);
	waitingToGoIn[og]++;
	goOnTheBus(id_user, og);

	sleep(random() % 2);

	// Bajarme en estación destino
	printf("ID: %d User is waiting for the stop %d to get OUT the bus...\n\n", id_user, dst);
	waitingToGoOut[dst]++;
	goOutTheBus(id_user, dst);

	sleep(random() % 2);
}

void *usersFunc(void *arg)
{
	int id_users = (int)(uintptr_t)arg;
	// obtener el id del usuario
	int ori, dest;
	while (1)
	{
		ori = rand() % NUM_STOPS;

		do
		{
			dest = rand() % NUM_STOPS;
		} while (ori == dest);

		user(id_users, ori, dest);
	}
}

int main(int argc, char *argv[])
{
	int i;

	// Definición de variables locales a main
	pthread_t tid_users[USERS];
	pthread_t tid_bus;

	pthread_mutex_init(&m, NULL); //cerrojo o locker si eres britanico

	pthread_cond_init(&bus, NULL);
	pthread_cond_init(&users, NULL);

	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas

	for (i = 0; i < USERS; i++)
		// Crear thread para el usuario i
		pthread_create(&tid_users[i], NULL, usersFunc, (void *)(uintptr_t)i);

	// Crear el thread Autobus
	pthread_create(&tid_bus, NULL, busFunc, NULL);

	for (i = 0; i < USERS; i++)
		pthread_join(tid_users[i], NULL);

	//Esperar a que termine el hilo
	pthread_join(tid_bus, NULL);

	pthread_cond_destroy(&users);
	pthread_cond_destroy(&bus);

	pthread_mutex_destroy(&m);

	return 0;
}