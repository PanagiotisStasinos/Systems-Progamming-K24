#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

main(){
	static struct sigaction act, oldact;
	
	printf("Saving the default way of handling the control-c\n");
	sigaction(SIGINT, NULL, &oldact);
	printf("sleep call #1\n"); sleep(4);

	printf("Changing (Ignoring) the way of handling control-c is ignored\n");
	act.sa_handler=SIG_IGN;   // the handler is set to IGNORE
	sigfillset(&(act.sa_mask));
	sigaction(SIGINT, &act, NULL);

	printf("sleep call #2\n"); sleep(4);

	printf("Restablishing to old way of handling (control-c)\n");
	sigaction(SIGINT, &oldact, NULL);
	printf("sleep call #3\n"); sleep(8);

	printf("Exiting \n");
	exit(0);
	}	
