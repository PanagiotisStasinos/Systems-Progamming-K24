#include <stdio.h>
#include <signal.h>

main(){
  void  wakeup(int);

  printf("about to sleep for 4 seconds \n");
  signal(SIGALRM, wakeup);

  alarm(4);
  pause();
  printf("Hola Amigo\n");
}

void wakeup(int signum){
  printf("Alarm received from kernel\n");
}


