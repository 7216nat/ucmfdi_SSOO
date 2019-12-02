
-
#include <stdio.h> 
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>


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
int esperando_parada[N_PARADAS]; //= {0,0,...0};

// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};

// Otras definiciones globales (comunicacion y sincronizacion)
pthread_t usuarios[USUARIOS];
pthread_t bus;
sem_t maximo = MAX_USUARIOS;
sem_t mutex = 1;
sem_t entrar = 0;
sem_t todoEntrado = 0;
sem_t salir = 0;
sem_t todoSalido = 0;

void * thread_autobus(void * args) {
	while (1/*condicion*/) {
		// esperar a que los viajeros suban y bajen
		Autobus_En_Parada();
		// conducir hasta siguiente parada
		Conducir_Hasta_Siguiente_Parada();
	}
}

void * thread_usuario(void * arg) {
	int id_usuario = int(*arg);
	// obtener el id del usario
	while (1/*condicion*/) {
		a=rand() % N_PARADAS;
		do{
			b=rand() % N_PARADAS;
		} while(a==b);
		Usuario(id_usuario,a,b);
	}
}

void Usuario(int id_usuario, int origen, int destino) {
	// Esperar a que el autobus este en parada origen para subir
	Subir_Autobus(id_usuario, origen);
	// Bajarme en estacion destino
	Bajar_Autobus(id_usuario, destino);
}

int main(int argc, char* argv[]){
	int i;
	// Definicion de variables locales a main
	// Opcional: obtener de los argumentos del programa la capacidad del
	// autobus, el numero de usuarios y el numero de paradas
	// Crear el thread Autobus
	pthread_create(&bus, NULL, thread_autobus, NULL);
	for (i = 0; i < USUARIOS; i++){
	// Crear thread para el usuario i
		pthread_create(&usuarios[i], NULL, thread_usuario, (void*)i);
	}
	// Esperar terminacion de los hilos	
	for (i = 0; i < USUARIOS; i++){
	// Crear thread para el usuario i
		pthread_join(&usuarios[i], NULL);
	}
	pthread_join(&bus, NULL);
	return 0;
}

void Autobus_En_Parada(){
	/* Ajustar el estado y bloquear al autobus hasta que no haya pasajeros que
	quieran bajar y/o subir la parada actual. Despues se pone en marcha */
	estado = EN_PARADA;
	sem_wait(mutex);
	if (esperando_bajar[parada_actual] > 0){
		sem_post(salir);
		sem_wait(todoSalido); 		
	}
	if (esperando_entrar[parada_actual] > 0){
		sem_post(entrar);
		sem_wait(todoEntrado);	
	}
	sem_post(mutex);	
}

void Conducir_Hasta_Siguiente_Parada(){
	/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
	estado = EN_RUTA;	
	printf("Conduciendo hasta siguiente parada...\n");
	sleep(random() %5);
	parada_actual = (parada_actual +1 > N_PARADAS)? 0: parada_actual+1;
	printf("El bus ha llegado a la parada nº %d\n", parada_actual);
}

void Subir_Autobus(int id_usuario, int origen){
	/* El usuario indicara que quiere subir en la parada ’origen’, esperara a que
	el autobus se pare en dicha parada y subira. El id_usuario puede utilizarse para
	proporcionar informacion de depuracion */
	sem_wait(maximo);
	sem_wait(mutex);
	esperando_parada[origen]++;
	sem_post(mutex);
	bool Entrado = false;
	while(!Entrado){
		if(origen == parada_actual){
			sem_wait(entrar);
			esperando_parada[origen]--;
			if (esperando_parada[origen] == 0) sem_post(todoEntrado);
			else sem_post(entrar);
			Entrado = true;		
		}
	}
}

void Bajar_Autobus(int id_usuario, int destino){
	/* El usuario indicara que quiere bajar en la parada ’destino’, esperara a que
	el autobus se pare en dicha parada y bajara. El id_usuario puede utilizarse para
	proporcionar informacion de depuracion */
	sem_wait(mutex);
	esperando_parada[destino]++;
	sem_post(mutex);
	bool Salido = false;
	while(!Salido){
		if(destino == parada_actual){
			sem_wait(salir);
			sem_post(maximo);
			esperando_parada[destino]--;
			if (esperando_parada[destino] == 0) sem_post(todoSalido);
			else sem_post(salir);
			Salido = true;		
		}
	}
}
}
