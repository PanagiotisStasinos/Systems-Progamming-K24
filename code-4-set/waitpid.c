#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int main(){
   pid_t  pid;
   int status, exit_status, i;

   if ( (pid = fork()) < 0 ) {
      perror("fork failed");
      exit(1);
   }

   if ( pid == 0 ){
        printf("Child %d is sleeping... \n",getpid());
        sleep(5);
        exit(57);
        }

   printf("reaching the father %lu process \n",getpid());
   printf("My child's PID is %lu \n", pid);
   while( (waitpid(pid, &status, WNOHANG)) == 0 ){
        printf("Still waiting for child to return\n");
        sleep(1);
        }

   printf("reaching the father %lu process \n",getpid());

   if (WIFEXITED(status)){
       exit_status = WEXITSTATUS(status);
       printf("Exit status from %lu was %d\n", pid, exit_status);
       }
   exit(0);
}


