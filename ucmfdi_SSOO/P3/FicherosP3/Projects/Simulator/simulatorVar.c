//Raúl Benito Montoro 02282781P
//Adrián Martín Tiscar 02555869V

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <signal.h>
#include <unistd.h>

#define N_PARADAS 5 // número de paradas de la ruta
#define EN_RUTA 0 // autobús en ruta
#define EN_PARADA 1 // autobús en la parada
#define MAX_USUARIOS 40 // capacidad del autobús
#define USUARIOS 4 // numero de usuarios

// estado inicial
int estado = EN_RUTA;
int parada_actual = 0; // parada en la que se encuentra el autobus
int n_ocupantes = 0; // ocupantes que tiene el autobús

// personas que desean subir en cada parada
int esperando_parada[N_PARADAS]; //= {0,0,...0};

// personas que desean bajar en cada parada
int esperando_bajar[N_PARADAS]; //= {0,0,...0};

//semaforo bajar gente
pthread_mutex_t cerrojo;

pthread_cond_t subir;
pthread_cond_t bajar;
pthread_cond_t bus;

//Quiere un cerrojo y tres colas condicionales
//Autobus: cola condicional para que espere. Solo despierte cuando todos hallan acabado. Llega a parada: condwait. Le hacen signal para despertar la ultima que entre o la ultima que baje.
//Cuando el bus llega a parada, que despierte con broadcast.
//Cola de los que suben. cond_wait(parada). Se despierta cuando el bus llega a su parada.
//Cola de los que bajan: cond_wait(paradabajar)

void Autobus_En_Parada(){
/* Ajustar el estado y bloquear al autobús hasta que no haya pasajeros que
quieran bajar y/o subir la parada actual. Después se pone en marcha */
 
    pthread_mutex_lock(&cerrojo);
    printf("Bus en %d\n", parada_actual);
    pthread_cond_broadcast(&subir);
    pthread_cond_broadcast(&bajar);
    estado=EN_PARADA;

    while(esperando_parada[parada_actual] > 0 || esperando_bajar[parada_actual] > 0)
        pthread_cond_wait(&bus, &cerrojo);
 
    estado = EN_RUTA;    
    pthread_mutex_unlock(&cerrojo);
    

}

void Conducir_Hasta_Siguiente_Parada(){
/* Establecer un Retardo que simule el trayecto y actualizar numero de parada */
    pthread_mutex_lock(&cerrojo);

    sleep(random() % 10); 

    if(parada_actual +1 >= N_PARADAS)
        parada_actual = 0;
    else
        parada_actual += 1;


    pthread_mutex_unlock(&cerrojo);

    
}

void Subir_Autobus(int id_usuario, int origen){
/* El usuario indicará que quiere subir en la parada ’origen’, esperará a que
el autobús se pare en dicha parada y subirá. El id_usuario puede utilizarse para
proporcionar información de depuración */
    pthread_mutex_lock(&cerrojo);
    esperando_parada[origen]++;
    
    while(estado == EN_RUTA || parada_actual != origen)
        pthread_cond_wait(&subir, &cerrojo);

    esperando_parada[origen]--;
    printf("Pasajero %d se ha subido en %d\n", id_usuario, origen);
    n_ocupantes++;
    pthread_cond_signal(&bus);
    pthread_mutex_unlock(&cerrojo);

}

void Bajar_Autobus(int id_usuario, int destino){
/* El usuario indicará que quiere bajar en la parada ’destino’, esperará a que
el autobús se pare en dicha parada y bajará. El id_usuario puede utilizarse para
proporcionar información de depuración */
    pthread_mutex_lock(&cerrojo);
    esperando_bajar[destino]++;

    while(parada_actual != destino || estado == EN_RUTA){
        pthread_cond_wait(&bajar, &cerrojo);
    }

    if(parada_actual == destino && estado == EN_PARADA){
        esperando_bajar[destino]--;
        printf("Pasajero %d se ha bajado en %d\n", id_usuario, destino);
        n_ocupantes--;
        pthread_cond_signal(&bus);   
    }
    pthread_mutex_unlock(&cerrojo);
 
}

// Otras definiciones globales (comunicación y sincronización)
void * thread_autobus(void * args) {
    while (1) {// esperar a que los viajeros suban y bajen
    Autobus_En_Parada();
    // conducir hasta siguiente parada
    Conducir_Hasta_Siguiente_Parada();
    }
}

void Usuario(int id_usuario, int origen, int destino) {
    
    // Esperar a que el autobus esté en parada origen para subir
    Subir_Autobus(id_usuario, origen);
    // Bajarme en estación destino
    Bajar_Autobus(id_usuario, destino);
}

void * thread_usuario(void * arg) {
    int id_usuario = arg;
    int a = 0, b=0;
    // obtener el id del usario
    while (1) {
        a=rand() % N_PARADAS;
        do{
            b=rand() % N_PARADAS;
        } while(a==b);
    
        Usuario(id_usuario,a,b);
    }
}




int main(int argc, char *argv[]) {
    
    pthread_attr_t attr;
    pthread_t thid[MAX_USUARIOS];
    pthread_attr_init(&attr);

    // Definición de variables locales a main
    // Opcional: obtener de los argumentos del programa la capacidad del
    // autobus, el numero de usuarios y el numero de paradas
    // Crear el thread Autobus
    pthread_t guagua;
    pthread_create(&guagua, NULL, thread_autobus, NULL);

    pthread_mutex_init(&cerrojo, NULL);
    pthread_cond_init(&subir, NULL);
    pthread_cond_init(&bajar, NULL);
    pthread_cond_init(&bus, NULL);

    for (int i = 0; i< USUARIOS; i++){
        pthread_create(&thid[i], NULL, thread_usuario, i);
    }

    for(int j = 0; j < USUARIOS; j++)
        pthread_join(thid[j], NULL);
    pthread_join(guagua, NULL);
    pthread_mutex_destroy(&cerrojo);
    pthread_cond_destroy(&subir);
    pthread_cond_destroy(&bajar);
    pthread_cond_destroy(&bus);

    // Crear thread para el usuario i
    // Esperar terminación de los hilos
    return 0;
    
}
