//XUKAI CHEN LAB11PT07
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PATH "/dev/chardev_leds"
int main(int argc, char* argv[]){
	int n, fd, aux;
	if(argc != 2){
		fprintf(stderr, "Usage: %s <num>\n", argv[0]);
	        exit(EXIT_FAILURE);
	}
	n = atoi(argv[1]);
	fd = open(PATH ,O_WRONLY);
	if (fd < 0){
		perror("open");
		exit(EXIT_FAILURE);
	}
	while(n>0){
		aux = n % 10;
		write_num(aux, fd);
		n = n / 10;
		sleep(3);
	}
	close(fd);
	return 0;		
}
void write_num(int num, int fd){
	switch(num){
	case 0:
		write(fd, "", 1);
		sleep(1);
		break;
	case 1:	
		write(fd, "3", 2);
		sleep(1);
		break;
	case 2:	
		write(fd, "2", 2);
		sleep(1);
		break;
	case 3:	
		write(fd, "23", 3);
		sleep(1);
		break;
	case 4:	
		write(fd, "1", 2);
		sleep(1);
		break;
	case 5:	
		write(fd, "13", 3);
		sleep(1);
		break;
	case 6:	
		write(fd, "12", 3);
		sleep(1);
		break;
	case 7:	
		write(fd, "123", 4);
		sleep(1);
		break;
	case 8:	
		write(fd, "123", 4);
		sleep(1);
		write(fd, "3", 2);
		sleep(1);
		break;
	case 9:	
		write(fd, "3", 2);
		sleep(1);
		write(fd, "2", 2);
		sleep(1);
		break;
	}
}

