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

   /* Create the FIFOs, then open them -- one for
    * reading and one for writing.
    */
 
   if ( (mkfifo(FIFO1, PERMS) < 0) && (errno != EEXIST) ) {
       perror("can't create fifo");
   }
   if ((mkfifo(FIFO2, PERMS) < 0) && (errno != EEXIST)) {
       unlink(FIFO1);
       perror("can't create fifo");
   }


   if ( (readfd = open(FIFO1, 0))  < 0)  {
      perror("server: can't open read fifo");
   }
 
   if ( (writefd = open(FIFO2, 1))  < 0)  {
      perror("server: can't open write fifo");
   }

   server(readfd, writefd);
   
   close(readfd);
   close(writefd);

   exit(0);

}

     
server(int readfd, int writefd) {

   char buff[MAXBUFF];
   char errmesg[256];
   int n, fd;


   /* Read the filename from the IPC descriptor. */

   if ((n= read(readfd, buff, MAXBUFF)) <= 0) {
       perror("server: filename read error ");
   }

   buff[n] = '\0';  /* null terminate filename */

   if ( (fd = open(buff, 0)) <0) {
     /* Error. Format an error message and send it
      *  back to the client
      */

     sprintf(errmesg, ":can't open, %s\n", buff);
     strcat(buff, errmesg);
     n = strlen(buff);
     if (write(writefd, buff, n) != n)  {
        perror("server: errmesg write error");
     }
  } else {
    /* 
     * Read the data from the file and write to
     * the IPC descriptor.
     */

     while ( (n = read(fd, buff, MAXBUFF)) > 0)
           if (write(writefd, buff, n) != n) {
               perror("server: data write error");
           }
     if (n < 0) {
       perror("server: read error");
    }
  }
}




