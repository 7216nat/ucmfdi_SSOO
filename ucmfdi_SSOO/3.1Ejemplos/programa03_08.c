#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int run_now = 1 ;
char message [] = "Hello World" ;
void * thread_function (void * arg) {
	int print_count2 = 0 ;
	while ( print_count2 < 4 ) {
		if ( run_now == 2 ) {
			printf ( "2" );
			run_now = 1 ;
			print_count2 ++;
		}
		else {
			sleep ( 1 );
		}
	}
	sleep ( 3 );
}
int main () {
	int res;
	pthread_t a_thread;
	void * thread_result;
	int print_count1 = 0;
	if ( fork () == 0 ) {
		res = pthread_create (&a_thread, NULL,thread_function, (void *) message );
		while ( print_count1 < 4 ) {
			if ( run_now == 1 ) {
				printf ( "1" );
				run_now = 2;
				print_count1 ++;
			}
			else {
				sleep ( 1 );
			}
		}
	}
	else {
		execlp ( "/bin/echo" , "/bin/echo" , message , NULL );
	}
	printf ( "\nWaiting for thread to finish...\n" );
	res = pthread_join (a_thread , &thread_result);
	printf ( "Thread joined\n" );
	exit ( EXIT_SUCCESS );
}
