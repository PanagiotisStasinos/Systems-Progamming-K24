#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

int main(void){
	sem_t mutex;
	sem_init(&mutex, 0, 1);
	sem_wait(&mutex);
	printf("waiting at 2nd sem_wait()\n");
	sem_wait(&mutex); 
	sem_destroy(&mutex);
	printf("END\n");	
}