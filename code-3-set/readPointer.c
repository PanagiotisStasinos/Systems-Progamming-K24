#include  <stdio.h>
#include  <stdlib.h> /* for exit */
#include  <fcntl.h>
#include  <unistd.h>
#include  <sys/stat.h>

int main(){
  char buf[8];
  int bytesread;

  int fd, fd2;

  if ( ( fd=open("t2", O_RDONLY) ) == -1 ){
	perror("open");
	exit(1);
  }
  
  bytesread = read(fd, buf, 8);
  printf("%d bytes were read \n", bytesread);
  printf("%s\n", buf);

  char *buf2;
  int bytesread2;

  if ( ( fd2=open("t2", O_RDONLY) ) == -1 ){
	perror("open");
	exit(1);
  }
  bytesread2 = read(fd2, buf2, 8);

  printf("%d bytes were read \n", bytesread2);
  printf("%s\n", buf2);

  close(fd);
  return 1;
}


