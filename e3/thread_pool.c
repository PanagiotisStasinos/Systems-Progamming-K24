#include "thread_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>			/* sockets */
#include <sys/socket.h>			/* sockets */
#include <netinet/in.h>			/* internet sockets */
#include <unistd.h>				/* read, write, close */
#include <netdb.h>				/* gethostbyaddr */
#include <arpa/inet.h>



struct arguments *args;	//malloc in create, free in destroy

void perror_exit1(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}

int create_thread_pool(struct thread_pool* pool, int num_of_threads, int buffer_size){
	pool->num_of_threads = num_of_threads;
	pool->returned_values = (int*)malloc(num_of_threads*sizeof(int));
	pool->threads = (pthread_t*)malloc(num_of_threads*sizeof(pthread_t));

	pool->buffer_size = buffer_size;
	pool->buffer = (struct job*)malloc(buffer_size*sizeof(struct job));
	pool->bufin = 0;
	pool->bufout = 0;	// pool->bufout = -1;
	pool->count = 0;

	sem_init(&(pool->mutex), 0, 1);
	sem_init(&(pool->end_mutex), 0, 1);
	sem_init(&(pool->full), 0, 0);
	sem_init(&(pool->empty), 0, buffer_size);

	pool->end_flag = 0;

	args = (struct arguments*)malloc(num_of_threads*sizeof(struct arguments));

	/*	create threads 	*/
	for (int i= 0; i < num_of_threads; ++i){
		pool->returned_values[i] = -1;

		args[i].index = i;
		args[i].pool = pool;
		pool->returned_values[i] = pthread_create(&(pool->threads[i]), NULL, ready, (void*)(&(args[i])));

		if(pool->returned_values[i]){
			printf("could not create thread\n");
			exit(-1);
		}
	}
}

int thread_pool_destroy(struct thread_pool* pool){
	sem_wait(&(pool->end_mutex));
		pool->end_flag = -1;
	sem_post(&(pool->end_mutex));
	for (int i= 0; i < pool->num_of_threads; ++i){
		sem_post(&(pool->full));
	}

	for (int i= 0; i < pool->num_of_threads; ++i){
		pthread_join(pool->threads[i], NULL);
	}
	sem_destroy(&(pool->mutex));
	sem_destroy(&(pool->full));
	sem_destroy(&(pool->empty));
	sem_destroy(&(pool->end_mutex));

	free(pool->returned_values);
	free(pool->threads);
	free(pool->buffer);

	free(args);
}

int push_job(struct thread_pool* pool, struct job* temp_job, char* port, char* ip){

	sem_wait(&(pool->empty));
	sem_wait(&(pool->mutex));
		// add job
		strcpy(pool->buffer[pool->bufin].pathname , temp_job->pathname);
		pool->buffer[pool->bufin].version = temp_job->version;
		strcpy(pool->buffer[pool->bufin].ip , temp_job->ip);
		strcpy(pool->buffer[pool->bufin].port , temp_job->port);
		strcpy(pool->buffer[pool->bufin].my_ip , ip);
		strcpy(pool->buffer[pool->bufin].my_port , port);
		pool->buffer[pool->bufin].my_id = temp_job->my_id;
		strcpy(pool->buffer[pool->bufin].my_dir , temp_job->my_dir);

		pool->bufin = (pool->bufin+1)%(pool->buffer_size);
		pool->count++;
	sem_post(&(pool->mutex));
	sem_post(&(pool->full));

}

int pull_job(struct thread_pool* pool, struct job* temp_job){
	// struct job temp_job;

	sem_wait(&(pool->full));
	sem_wait(&(pool->mutex));
		// check gor termination
		sem_wait(&(pool->end_mutex));
			if(pool->end_flag == -1){
				sem_post(&(pool->end_mutex));
				sem_post(&(pool->mutex));
				return -1;
			}
		sem_post(&(pool->end_mutex));
		// pull a job
		strcpy(temp_job->pathname , pool->buffer[pool->bufout].pathname);
		temp_job->version = pool->buffer[pool->bufout].version;
		strcpy(temp_job->ip , pool->buffer[pool->bufout].ip);
		strcpy(temp_job->port , pool->buffer[pool->bufout].port);
		strcpy(temp_job->my_ip , pool->buffer[pool->bufout].my_ip);
		strcpy(temp_job->my_port , pool->buffer[pool->bufout].my_port);
		temp_job->my_id = pool->buffer[pool->bufout].my_id;
		strcpy(temp_job->my_dir , pool->buffer[pool->bufout].my_dir);

		pool->bufout = (pool->bufout+1)%(pool->buffer_size);
		pool->count--;
	sem_post(&(pool->mutex));
	sem_post(&(pool->empty));

	return 0;
}

void* ready(void *args){
	struct arguments* arg = args;
	int index = arg->index;
	struct thread_pool* pool = arg->pool;
	// printf("Thread %d\n", index);

	struct job temp_job;
	while(1){

		if ( pull_job(arg->pool, &temp_job) == -1){
			break;
		}
		
		if( temp_job.version==-1 ){
			send_file_list_job(&temp_job);
		}
		else{
			send_file_job(&temp_job);
		}
	}
}

int get_buffer_len(struct thread_pool* pool){
	return pool->count;
}

int send_file_list_job(struct job* temp_job){
	struct file_list_head file_list;
	file_list.count = 0;
	file_list.first = NULL;

	create_file_list(temp_job->pathname, &file_list);

	struct file_node* temp = file_list.first;
	int n=0;
	char message[512];
	while(temp!=NULL){	//find length of message
		n++;

		temp = temp->next;
	}
	sprintf(message, "FILE_LIST <%s, %s, %d> %d ", temp_job->my_ip, temp_job->my_port, temp_job->my_id ,n);
	temp = file_list.first;
	while(temp!=NULL){	//find length of message
		sprintf(message, "%s<%s, %d>", message, temp->file_path, 0);

		temp = temp->next;
	}

	struct sockaddr_in other_client;
	struct sockaddr *other_clientptr = (struct sockaddr*)&other_client;
	int sock;
	struct hostent *rem;

	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit1("socket");
	/* Find server address */
	if ((rem = gethostbyname(temp_job->ip)) == NULL) {	
		herror("gethostbyname"); exit(1);
	}
	int port = atoi(temp_job->port);

	other_client.sin_family = AF_INET;			/* Internet domain */
	memcpy(&other_client.sin_addr, rem->h_addr, rem->h_length);
	other_client.sin_port = htons(port);		/* Server port */
	/* Initiate connection */
	while (connect(sock, other_clientptr, sizeof(other_client)) < 0){
		sleep(1);
	}

	int len = strlen(message);
	len++;
	if (write(sock, &len, sizeof(int)) < 0)
		perror_exit1("write");
	if (write(sock, message, len) < 0){
		perror_exit1("write");
	}
	close(sock);	

	free_file_list(&file_list);
}

int send_file_job(struct job* temp_job){
	char message[512];
	char *text;
	char name[200];
	snprintf(name, 200, "%s/%s", temp_job->my_dir, temp_job->pathname);
	FILE *fp = NULL;
	fp = fopen(name, "rb");
	if( fp == NULL ){
		return 0;
	}

	fseek(fp, 0, SEEK_END);
	int sz = ftell(fp);	
	fclose(fp);
	fp = fopen(name, "rb");

	char *buffer = NULL;
	buffer = (char*)calloc(sz+1, sizeof(char));
	if( buffer == NULL ){
		struct sockaddr_in other_client;
		struct sockaddr *other_clientptr = (struct sockaddr*)&other_client;
		int sock;
		struct hostent *rem;

		/* Create socket */
		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
			perror_exit1("socket");
		/* Find server address */
		if ((rem = gethostbyname(temp_job->ip)) == NULL) {	
			herror("gethostbyname"); exit(1);
		}
		int port = atoi(temp_job->port);

		other_client.sin_family = AF_INET;			/* Internet domain */
		memcpy(&other_client.sin_addr, rem->h_addr, rem->h_length);
		other_client.sin_port = htons(port);		/* Server port */
		/* Initiate connection */
		while (connect(sock, other_clientptr, sizeof(other_client)) < 0){
			sleep(1);
		}
		char message[15] = "FILE_NOT_FOUND";
		int len = strlen(message);
		len++;
		if (write(sock, &len, sizeof(int)) < 0)
			perror_exit1("write");
		if (write(sock, message, len) < 0){
			perror_exit1("write");
		}
		close(sock);
		return 0;
	}
	fread(buffer, sz+1, sizeof(char), fp);

// printf("flag 1\n");
	sprintf(message, "FILE_SIZE <%s, %s, %d>", temp_job->my_ip, temp_job->my_port, temp_job->my_id);
// printf("flag 2\n");
	sprintf(message, "%s<%s, %d, %d", message, temp_job->pathname, temp_job->version, sz);
// printf("flag 3\n");
	sprintf(message,"%s, %s>", message, buffer);
// printf("flag 4\n");

	
	fclose(fp);

	struct sockaddr_in other_client;
	struct sockaddr *other_clientptr = (struct sockaddr*)&other_client;
	int sock;
	struct hostent *rem;

	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit1("socket");
	/* Find server address */
	if ((rem = gethostbyname(temp_job->ip)) == NULL) {	
		herror("gethostbyname"); exit(1);
	}
	int port = atoi(temp_job->port);

	other_client.sin_family = AF_INET;			/* Internet domain */
	memcpy(&other_client.sin_addr, rem->h_addr, rem->h_length);
	other_client.sin_port = htons(port);		/* Server port */
	/* Initiate connection */
	while (connect(sock, other_clientptr, sizeof(other_client)) < 0){
		sleep(1);
	}

	int len = strlen(message);
	len++;
	if (write(sock, &len, sizeof(int)) < 0)
		perror_exit1("write");
	if (write(sock, message, len) < 0){
		perror_exit1("write");
	}
	close(sock);

	free(buffer);
	buffer= NULL;
}