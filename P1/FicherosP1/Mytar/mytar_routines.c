#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	// Complete the function
	int c = 0, ret= 0;
	int count = 0;
	while ((count < nBytes) &&(c = getc(origin)) != EOF) {
                
                ret=putc((unsigned char) c, destination);
		count++;
                if (ret==EOF){
			return -1;
                }
        }
	 
	return count;
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	// Complete the function
	int c, count = 0;
	while ((c = getc(file)) != '\0')
	       count++;
	char* pointer;
	pointer = malloc(sizeof(char) * (count+1));
	lseek(fileno(file), -(count +1), SEEK_CUR);
	
	for(int i = 0; i < count +1; i++)
		pointer[i] = getc(file);
	return pointer;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	// Complete the function
	stHeaderEntry* array = NULL;
	int nr_files = 0;
	
	fread(&nr_files, sizeof(int), 1, tarFile);
	if(nr_files <= 0) return NULL;
	
	array = malloc(sizeof(stHeaderEntry)*nr_files);
	for(int i = 0; i < nr_files; i++){
		array[i].name = loadstr(tarFile);
		fread(&array[i].size, sizeof(int), 1, tarFile);
	}

	(*nFiles) = nr_files;
	
	return array;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	// Complete the function
	FILE * origin;
	FILE * destination;
	stHeaderEntry* array = NULL;
	int offData = 0, count = 0;
	// open store file 
	destination = fopen(tarName, "w+");
	//if(destination == NULL) return EXIT_FAILURE;
	// malloc of header's array
	//if(nFiles <= 0) return EXIT_FAILURE;
	array = (stHeaderEntry*)malloc(sizeof(stHeaderEntry)*nFiles);
	
	offData += (sizeof(int) + nFiles*sizeof(int));

	for(int i = 0; i < nFiles; i++){
		array[i].name = malloc(strlen(fileNames[i]) +1);
		strcpy(array[i].name, fileNames[i]);
		offData += (strlen(fileNames[i]) +1);
	}
	lseek(fileno(destination), offData, SEEK_SET);
	for(int i = 0; i < nFiles; i++){
		origin = fopen(fileNames[i], "r");
		if (origin == NULL) return EXIT_FAILURE;
		count = copynFile(origin, destination, INT_MAX);
		array[i].size = count;
		fclose(origin);
	}
	lseek(fileno(destination), 0, SEEK_SET);
	fwrite(&nFiles, sizeof(int), 1, destination);
	for(int i = 0; i < nFiles; i++){
		fwrite(array[i].name, strlen(array[i].name)+1, 1, destination);
		fwrite(&array[i].size, sizeof(int), 1, destination);
	}
	for(int i = 0; i < nFiles; i++){
		free(array[i].name);	
	}
	free(array);
	fclose(destination);
	
	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	// Complete the function
	FILE * tarball = fopen(tarName, "r");
	if (tarball == NULL) return EXIT_FAILURE;
	FILE * outputFile = NULL;
	int num = 0;
	int *nFiles = &num;
	stHeaderEntry* array = readHeader(tarball, nFiles);
	for(int i = 0; i < *nFiles; i++){	
		outputFile = fopen(array[i].name, "w");
		copynFile(tarball, outputFile, array[i].size);
		fclose(outputFile);
	}
	for(int i = 0; i < *nFiles; i++){
		free(array[i].name);
	}
	free(array);
	fclose(tarball);
	return EXIT_SUCCESS;
}
