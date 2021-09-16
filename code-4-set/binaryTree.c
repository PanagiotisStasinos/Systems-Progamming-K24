#include <stdio.h>     /* For printf */
#include <stdlib.h>    /* For exit */
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]){
   int i, depth, numb, pid1, pid2, status;

   if (argc >1)  depth = atoi(argv[1]);     /* Make integer */
   else { printf("Usage: %s #-of-Params",argv[0]); exit(0);}
   if (depth>5) {     /* Avoid deep trees */
      printf("Depth should be up to 5\n");
      exit(0);
      }

   numb = 1;      /* Holds the number of each process */
   for(i=0;i<depth;i++){
      printf("I am process no %5d  with PID %5d and PPID %d\n",
              numb, getpid(), getppid());
      switch (pid1=fork()){
      case 0:                       /* Left child code */
         numb=2*numb; break;    
      case -1:                      /* Error creating left child */
         perror("fork"); exit(1);   
      default:                      /* Parent code */
         switch (pid2=fork()){
            case 0:                 /* Right child code */
               numb=2*numb+1; break;
            case -1:
               perror("fork"); exit(1);  /* Error creating right child */
            default:
               wait(&status); wait(&status);    /* Parent code */
               exit(0);
            }
     }
   }
}

