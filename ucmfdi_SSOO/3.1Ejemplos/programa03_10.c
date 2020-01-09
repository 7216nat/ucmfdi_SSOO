#include <sys/types.h>
#include <stdio.h>

main(int argc, char *argv[]) {
  pid_t pid;

  pid = fork();
  switch (pid) {
    case -1:  /* error del fork() */
      return 1;
    case 0:   /* proceso hijo */
      execvp(argv[1], &argv[1]);
      perror("exec");
      return 2;
    default:  /* padre */
      printf("Proceso padre\n");
  }

  return 0;
}
