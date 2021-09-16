#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int counter = 0;
char c = 'x';

void alarm_handler(int singnum){
	printf("%d\n", counter);
	exit(0);
}

int main(void){
	int p[2];
	static struct sigaction act;
	if(pipe(p)==-1){
		perror("pipe call");
		exit(1);
	}
	act.sa_handler = alarm_handler;
	sigfillset(&(act.sa_mask));
	sigaction(SIGALRM, &act, NULL);

	while(1){
		alarm(20);
		counter++;
		write(p[1], &c, 1);
		alarm(0);
	}
}