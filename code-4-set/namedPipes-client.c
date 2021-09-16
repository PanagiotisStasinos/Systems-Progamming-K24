#include  <sys/types.h>
#include  <sys/stat.h>
#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int errno;

#define MAXBUFF 1024
#define FIFO1   "/tmp/fifo.1"
#define FIFO2   "/tmp/fifo.2"
#define PERMS   0666


main(){
   int readfd, writefd;

   /* Open the FIFOs.  We assume server has already created them.  */

   if ( (writefd = open(FIFO1, 1))  < 0)  {
      perror("client: can't open write fifo \n");
   }
   if ( (readfd = open(FIFO2, 0))  < 0)  {
      perror("client: can't open read fifo \n");
   }

   client(readfd, writefd);
   
   close(readfd);
   close(writefd);

   /* Delete the FIFOs, now that we're done.  */

   if ( unlink(FIFO1) < 0) {
    perror("client: can't unlink \n");
  }
   if ( unlink(FIFO2) < 0) {
    perror("client: can't unlink \n");
  }
  
  exit(0);
}

     
client(int readfd, int writefd) {

   char buff[MAXBUFF];
   int n;


   /* Read the filename from standard input, 
    *  write it to the IPC descriptor.
    */

   if (fgets(buff, MAXBUFF, stdin) == NULL)
    perror("client: filename read error \n");

   n = strlen(buff);
   if (buff[n-1] == '\n')
      n--;     /* ignore newline from fgets() */

   if (write(writefd, buff, n) != n)
      perror("client: filename write error");

   /* Read data from the IPC descriptor and write to
    * standard output. 
    */

   while ( (n = read(readfd, buff, MAXBUFF)) > 0)
      if (write(1, buff, n) != n)   /* fd 1 = stdout */ {
         perror("client: data write error \n");
      }
   if (n <0) { 
      perror("client: data read error \n");
   }

}



