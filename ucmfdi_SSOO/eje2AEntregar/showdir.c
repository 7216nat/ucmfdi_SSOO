//XUKAI CHEN Pt04Lab11
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysmacros.h>

int main (int argc, char* argv[]){
  DIR *miDir;
  struct dirent *midirent;
  struct stat mistat;
  if (argc == 1)  
	miDir = opendir("./");
  else if(argc == 2){
	miDir = opendir(argv[1]);
  }
  else { 
	fprintf(stderr, "Usage: %s <pathname>\n", argv[0]);
        exit(EXIT_FAILURE);
  }
  if (miDir != NULL)
    {
      chdir(argv[1]);
      while ((midirent = readdir (miDir)) != NULL){
	if ((strcmp(midirent->d_name, ".")) == 0 || (strcmp(midirent->d_name, "..")) == 0)
		continue;
	lstat(midirent->d_name, &mistat);
	
        printf ("%lld\t", (long long)mistat.st_size);
	printf ("%ld\t", (long)mistat.st_ino);
	printf("%lo (octal)\t",(unsigned long) mistat.st_mode);
	switch (mistat.st_mode & S_IFMT) {
           case S_IFBLK:  printf("block device\t");            break;
           case S_IFCHR:  printf("character device\t");        break;
           case S_IFDIR:  printf("directory\t");               break;
           case S_IFIFO:  printf("FIFO/pipe\t");               break;
           case S_IFLNK:  printf("symlink\t");                 break;
           case S_IFREG:  printf("regular file\t");            break;
           case S_IFSOCK: printf("socket\t");                  break;
           default:       printf("unknown?\t");                break;
           }

	printf("%s\n", ctime(&mistat.st_mtime));
      }
      (void) closedir (miDir);
    }
  else{
    perror ("Couldn't open the directory");
    exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
