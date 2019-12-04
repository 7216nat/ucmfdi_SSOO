#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

#define N_PARADAS 5 // numero de paradas de la ruta
#define EN_RUTA 0 // autobus en ruta
#define EN_PARADA 1 // autobus en la parada
#define MAX_USUARIOS 40 // capacidad del autobus
#define USUARIOS 4 // numero de usuarios
// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobus
// personas que desean subir en cada parada
int esperando_parada[N_PARADAS];
// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS];
// Otras definiciones globales (comunicacion y sincronizacion)
pthread_t usuarios[USUARIOS];
pthread_t bus;
sem_t maximo, mutex, entrar[USUARIOS], todoEntrado, salir[USUARIOS], todoSalido;

void Autobus_En_Parada(){
	/* Ajustar el estado y bloquear al autobus hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Despues se pone en marcha */
	estado = EN_PARADA;
	sem_wait(&mutex);
	printf("Para el autobus\n");
	if (esperando_bajar[parada_actual] > 0){
		sem_post(&salir[parada_actual]);
		sem_wait(&todoSalido); 		
	}
	if (esperando_parada[parada_actual] > 0){
		sem_post(&entrar[parada_actual]);
		sem_wait(&todoEntrado);	
	}
	sem_post(&mutex);	
}

void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	estado = EN_RUTA;	
	printf("Conduciendo hasta siguiente parada...\n");
	sleep(random() %5);
	parada_actual = (parada_actual +1 >= N_PARADAS)? 0: parada_actual+1;
	printf("El bus ha llegado a la parada nº %d\n", parada_actual);
}

void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicara que quiere subir en la parada ’origen’, esperara a que
	el autobus se pare en dicha parada y subira. El id_usuario puede utilizarse para
	proporcionar informacion de depuracion */
	sem_wait(&maximo);
	sem_wait(&mutex);
	printf("Espera la persona %d en la parada %d ...\n", id_usuario,origen);
	esperando_parada[origen]++;
	sem_post(&mutex);
	sem_wait(&entrar[origen]);		
	esperando_parada[origen]--;
	n_ocupantes++;
	printf("Sube la persona %d ...\n", id_usuario);
	if (esperando_parada[origen] == 0) sem_post(&todoEntrado);
	else sem_post(&entrar[origen]);		
}

void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicara que quiere bajar en la parada ’destino’, esperara a que
	el autobus se pare en dicha parada y bajara. El id_usuario puede utilizarse para
	proporcionar informacion de depuracion */
	sem_wait(&mutex);
	esperando_bajar[destino]++;
	printf("Quiere salir la persona %d en la parada %d ...\n", id_usuario, destino);
	sem_post(&mutex);
	sem_wait(&salir[destino]);
	sem_post(&maximo);
	esperando_bajar[destino]--;
	n_ocupantes--;
	printf("Baja la persona %d ...\n", id_usuario);
	if (esperando_bajar[destino] == 0) sem_post(&todoSalido);
	else sem_post(&salir[destino]);

}

void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus este en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estacion destino
	Bajar_Autobus(id_usuario, destino);
}

void * thread_autobus(void * args) {
	while (1/*condicion*/) {
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}

void * thread_usuario(void * arg) {
	int id_usuario, a, b;
	id_usuario = (int)arg;
	// obtener el id del usario
	while (1/*condicion*/) {
		a=rand() % N_PARADAS;
		do{
			b=rand() % N_PARADAS;
		} while(a==b);
		Usuario(id_usuario,a,b);
	}
}

int main(int argc, char* argv[]){
	int i;
	// Definicion de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	// Crear el thread Autobus
	/*int USUARIOS, MAX_USUARIOS, N_PARADAS;
	if (argc == 1){
		USUARIOS = 4;
		MAX_USUARIOS = 40;
		N_PARADAS = 5;
	}
	else if(argc == 4){
		MAX_USUARIOS = argv[1];
		USUARIOS = argv[2];
		N_PARADAS = argv[3];
	}
	else {
		fprintf(stderr, "Usage: %s <MAX_USUARIOS> <USUARIOS> <N_PARADAS> \n", argv[0]);
        	exit(EXIT_FAILURE);
	}*/
	sem_init(&mutex, 0, 1);
	for(i=0; i<USUARIOS; i++){
		sem_init(&entrar[i], 0, 0);
   		sem_init(&salir[i], 0, 0);
	}
	sem_init(&todoEntrado, 0, 0);
	sem_init(&todoSalido, 0, 0);
	sem_init(&maximo, 0, MAX_USUARIOS);
	pthread_create(&bus, NULL, thread_autobus, NULL);
	for (i = 0; i < USUARIOS; i++){
	// Crear thread para el usuario i
		pthread_create(&usuarios[i], NULL, thread_usuario, (void *)i);
	}
	// Esperar terminacion de los hilos	
	for (i = 0; i < USUARIOS; i++){
	// Crear thread para el usuario i
		pthread_join(usuarios[i], NULL);
	}
	pthread_join(bus, NULL);
	return 0;
}


