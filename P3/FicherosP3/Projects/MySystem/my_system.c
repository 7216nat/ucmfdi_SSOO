#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argc, char* argv[])
{
	if (argc!=2)
	{
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}
	else 
	{
		pid_t pid;
		int status;
		if ((pid=fork()) == -1) 
		{
                        printf("ERROR. Process %u could not fork. Exit\n",pid);
                        exit(-1);
                }
		else if (pid==0) 
		{
			execl("/bin/bash", "/bin/bash","-c", argv[1], NULL);
			exit(-1);	
		}
		
		else{
			while(pid != wait(&status));
		}
		exit(0);
	}

	return system(argv[1]);
}

