#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

main(){
	sigset_t set1, set2;

	sigfillset(&set1); // completely full set

	sigfillset(&set2); 
	sigdelset(&set2, SIGINT);
	sigdelset(&set2, SIGTSTP);  // a set minus INT & TSTP
	
	printf("This is simple code... \n");
	sleep(5);
	sigprocmask(SIG_SETMASK, &set1, NULL);  // disallow everything here!
	
	printf("This is CRITICAL code... \n"); sleep(10);

	sigprocmask(SIG_UNBLOCK, &set2, NULL);  // allow all but INT & TSTP
	printf("This is less CRITICAL code... \n"); sleep(10);
	
	sigprocmask(SIG_UNBLOCK, &set1, NULL);  // remove all signals in set1
	printf("All signals are welcome!\n");
	sleep(10);
	exit(0);
}
