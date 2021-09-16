#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

// argv[1] my id
// argv[2] mirror_dir
// argv[3] other client's id

int main(int argc, char **argv){
	char rm[20];
	sprintf(rm, "rm -rf %s/%s", argv[2], argv[3]);
	printf("%s\n", rm);
	system(rm);
}