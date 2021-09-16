#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#define BUFSIZE 10

int main(void){
   char bufin[BUFSIZE]="empty";
   char bufout[BUFSIZE]="hello";
   pid_t  childpid; 
   int fd[2];

   if (pipe(fd) == -1 ){
      perror("failed to create pipe"); exit(23);
      }

   childpid = fork();

   if (childpid == -1) { perror("failed to fork"); exit (23);}

   if (childpid)  // parent code 
       write(fd[1],bufout,strlen(bufout)+1);
   else           // child code 
       read(fd[0],bufin,strlen(bufin)+1);

   printf("[%ld]: my bufin is %s, my bufout is %s (parent process %ld)\n",
       (long)getpid(), bufin, bufout, (long)getppid());
   return 0;
}


