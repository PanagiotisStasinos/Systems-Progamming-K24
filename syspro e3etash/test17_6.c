#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details. 
#include <pthread.h> 


int main() 
{
	printf("%ld\n", (long)getpid());
	if (fork() && fork())
	{
		printf("%ld\n", (long)getpid());
		fork();
	}
	printf("My thing\n");
	return 0;
}