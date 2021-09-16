#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char *argv[]){
	struct stat statbuf;

	if (stat(argv[1], &statbuf) == -1)
		perror("Failed to get file status");
	else 
		printf("%s accessed: %s modified: %s",argv[1], 
                       ctime(&statbuf.st_atime), ctime(&statbuf.st_mtime));
	return 1;
}

