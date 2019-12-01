#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main(void) {
  pid_t pid;
  int status;

  pid = fork();
  switch (pid) {
    case -1:  /* error del fork() */
      return 1;
    case 0:   /* proceso hijo */
      execlp("ls","ls","-l",NULL);
      perror("exec");
      return 2;
    default:  /* padre */
      printf("Proceso padre\n");
  }
  return 0;
}
