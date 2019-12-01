#include <sys/types.h>
#include <stdio.h>
/* programa que ejecuta el mandato ls -l */
main() {
	pid_t pid;
	int status;
	pid = fork();
	if (pid == 0) { /* proceso hijo */
		execlp("ls","ls","-l",NULL);
		exit(-1);
	}
	else{ /* proceso padre */
		printf(&status);
		while (pid != wait(&status));
		sleep(1000);
	}
	exit(0);
}
