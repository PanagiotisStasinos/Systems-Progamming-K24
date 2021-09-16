#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void catchinterrupt(int signo){
	printf("\nCatching: signo=%d\n",signo);
	printf("Catching: returning\n");
	}

main(){
	static struct sigaction act;

	act.sa_handler=catchinterrupt;
	sigfillset(&(act.sa_mask));

	sigaction(SIGINT, &act, NULL);
	
	printf("sleep call #1\n");
	sleep(1);
	printf("sleep call #2\n");
	sleep(1);
	printf("sleep call #3\n");
	sleep(1);
	printf("sleep call #4\n");
	sleep(1);

	printf("Exiting \n");
	exit(0);
	}	
