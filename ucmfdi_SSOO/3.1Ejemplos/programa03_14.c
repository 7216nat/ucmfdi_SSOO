#include <pthread.h>
#include <stdio.h>

void * func(void * arg) {
  printf("Thread %d\n", pthread_self());
  pthread_exit(0);
}

int main(void) {
  pthread_t th1, th2;

  /* se crean dos threads con atributos por defecto */
  pthread_create(&th1, NULL, func, NULL);
  pthread_create(&th2, NULL, func, NULL);

  printf("El thread principal continua ejecutando\n");

  /* se espera su terminacion */
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  return 0;
}
