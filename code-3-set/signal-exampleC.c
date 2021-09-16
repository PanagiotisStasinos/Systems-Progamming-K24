#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

main(){
	static struct sigaction act;

	act.sa_handler=SIG_IGN;   // the handler is set to IGNORE
	sigfillset(&(act.sa_mask));

	sigaction(SIGINT, &act, NULL);
	sigaction(SIGTSTP, &act, NULL);
	
	printf("sleep call #1\n");
	sleep(1);
	printf("sleep call #2\n");
	sleep(1);
	printf("sleep call #3\n");
	sleep(1);

	act.sa_handler=SIG_DFL;  // reestablish the DEFAULT behavior
	sigaction(SIGINT, &act, NULL);  // default for control-c

	printf("sleep call #4\n");
	sleep(1);
	printf("sleep call #5\n");
	sleep(1);
	printf("sleep call #6\n");
	sleep(1);

	sigaction(SIGTSTP, &act, NULL); // default for control-z

	printf("Exiting \n");
	exit(0);
	}	
