#include <sys/types.h>
#include <stdio.h>

int main(void) {
  pid_t pid;
  int i;
  int n = 10;

  for (i = 0; i < n; i++) {
    pid = fork();
    if (pid == 0)
      break;
  }
  //Esperamos un poco con la intención de que terminen los hijos antes de morir el padre
  if (pid!=0 && i==n)
    sleep(1);
  printf("El padre del proceso %d es %d\n", getpid(), getppid());
  return 0;
}
