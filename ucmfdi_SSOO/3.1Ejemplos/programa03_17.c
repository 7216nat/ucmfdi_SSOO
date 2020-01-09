#include <stdio.h>
#include <signal.h>

void tratar_alarma(int param) {
  printf("Activada\n");
}

int main(void) {
  struct sigaction act;
  sigset_t mask;

  /* estable el manejador */
  act.sa_handler = tratar_alarma; /* funcion a ejecutar */
  act.sa_flags = 0;  /* ninguna accion especifica*/

  /* Se bloquea la se�al SIGINT cuando se ejecute la funci�n
   tratar_alarma */
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGINT);

  sigaction(SIGALRM, &act, NULL);

  for (;;) {
    alarm(3); /* se arma el temporizador */
    pause(); /* se suspende el proceso hasta que se reciba una senal */
  }
  return 0;
}
