#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
int main(void) {
  pid_t pid;
  int fd = open("prueba.txt", O_RDWR);
  int status;
  pid = fork();
  if (pid == 0) {
	char buf[1] = "h";
	char c;
	while((c=read(fd, buf, 1))>0)
		printf("%c",buf[0]);
	printf("Proceso %d; padre = %d\n", getpid(), getppid());
	exit(0);
 }
  else {
	char buf[1] = "h";
	char c;
	while((c=read(fd, buf, 1))>0)
		printf("%c",buf[0]);
	wait(&status);
	close(fd);
	printf("Proceso %d; padre = %d\n", getpid(), getppid());
  }
  return 0;
}
