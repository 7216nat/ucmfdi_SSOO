#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

pid_t pid;

void matar_proceso(int signum) {
  kill(pid, SIGKILL); /* se envia la senal al hijo */
}

int main(int argc, char *argv[]) {
  int status;
  char **argumentos;
  struct sigaction act;

  argumentos = &argv[1];
  /* Se crea el proceso hijo */
  pid = fork();
  switch (pid) {
    case -1:  /* error del fork() */
      exit(1);
    case 0:   /* proceso hijo */
      /* El proceso hijo ejecuta el mandato recibido */
      execvp(argumentos[0], argumentos);
      perror("exec");
      exit(1);
    default:  /* padre */
      /* establece el manejador */
      act.sa_handler = matar_proceso; /*funcion a ejecutar*/
      act.sa_flags = 0; /* ninguna accion especifica */
      sigemptyset(&act.sa_mask);
      sigaction(SIGALRM, &act, NULL);
      alarm(5);

      /* Espera al proceso hijo */
      wait(&status);
  }

  return 0;
}
