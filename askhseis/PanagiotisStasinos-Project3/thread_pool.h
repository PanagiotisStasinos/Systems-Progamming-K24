#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <pthread.h>
#include <semaphore.h>
#include "file_utils.h"
// #include "client_utils.h"

struct job{
	char pathname[128];
	int version;
	char ip[50];
	char port[10];

	char my_ip[50];
	char my_port[10];
	int my_id;
	char my_dir[50];
};

struct thread_pool{
	int num_of_threads;
	int buffer_size;

	int *returned_values;
	pthread_t *threads;

	struct job* buffer;
	int bufout;
	int bufin;
	int count;

	sem_t mutex;
	sem_t full;
	sem_t empty;
	int counter;

	int end_flag;
	sem_t end_mutex;
};

struct arguments{
	int index;		// number of thread
	struct thread_pool *pool;	// pointer of the pool
};

void perror_exit1(char *message);

int create_thread_pool(struct thread_pool* pool, int num_of_threads, int buffer_size);
int thread_pool_destroy(struct thread_pool* pool);

int push_job(struct thread_pool* pool, struct job* temp_job, char* port, char* ip);
int pull_job(struct thread_pool* pool, struct job* temp_job);

void* ready(void *args);

int get_buffer_len(struct thread_pool* pool);	//returns the number of records currently in buffer

int send_file_list_job(struct job* temp_job);
int send_file_job(struct job* temp_job);
#endif