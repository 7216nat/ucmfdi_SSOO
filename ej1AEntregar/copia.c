//Xukai Chen
#include <stdio.h>
#include <stdlib.h>
void main(void){
	int c = 0;
	FILE *origen; 
	FILE *destino;
	origen = fopen("origen.txt", "r");
	if(origen == NULL){ 
		printf("Error origen.");
	}
	else {
		destino = fopen("destino.txt", "w+");
		c = fgetc(origen);
		while( c > 0){ 
			fputc(c, destino);
			c = fgetc(origen);
		}
		fclose(destino);
	}
	fclose(origen);
   	return; 
}
