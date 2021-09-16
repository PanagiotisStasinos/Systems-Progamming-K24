#include <stdio.h>
#include <signal.h>

int main(){
  int i;

  signal(SIGINT, SIG_IGN);

  printf("you can't stop me here! \n");
  while(1){
	sleep(1);
	printf("haha \n");
	}
}

/* use cntrl-\ to get rid of this process */


