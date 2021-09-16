#include "client_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>			/* sockets */
#include <sys/socket.h>			/* sockets */
#include <netinet/in.h>			/* internet sockets */
#include <unistd.h>				/* read, write, close */
#include <netdb.h>				/* gethostbyaddr */
#include <arpa/inet.h>


void perror_exit(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}


int create_list(struct list_head* head){
	head->count = 0;
	head->first = NULL;
	head->last = NULL;
}

int add_client(struct list_head* head, char* client_log_on_info, int my_id){
	char* temp;
	temp = (char*)malloc((strlen(client_log_on_info)+1)*sizeof(char));
	strcpy(temp, client_log_on_info);


	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *ip;
	char* port;
	char* id;
	ip = strtok(temp, s1);
	ip = strtok(NULL, s3);
	ip++;	// get rid of '<'
	port = strtok(NULL, s3);
	port++;	// get rid of ' '
	id = strtok(NULL, s4);
	id++;

	if( client_exists(head, ip, port)==-1 ){
		add_node(head, ip, port, atoi(id));
		int ID = atoi(id);
		char dir[100];
		snprintf(dir, 100, "dir%d_copyof%d", my_id, ID);
		mkdir(dir, 0777);
	}
	else{
		printf("Client [%s, %s] exists\n", ip,port);
	}

	free(temp);
}

int add_node(struct list_head* head, char *ip, char *port, int id){
	struct list_node* temp = (struct list_node*)malloc(sizeof(struct list_node));
	temp->ip = (char*)malloc((strlen(ip)+1)*sizeof(char));
	temp->port = (char*)malloc((strlen(port)+1)*sizeof(char));

	strcpy(temp->ip, ip);
	strcpy(temp->port, port);
	temp->id = id;
	temp->next = NULL;

	if(head->count==0){	// first clinet in list
		head->first  = temp;
	}
	else{
		head->last->next = temp;
	}
	head->last = temp;
	head->count++;
}

int client_exists(struct list_head* head, char *ip, char *port){	// returns node's id
	struct list_node* temp = head->first;
	while(temp!=NULL){
		if(strcmp(ip,temp->ip)==0 && strcmp(port,temp->port)==0){
			return temp->id;
		}

		temp = temp->next;
	}
	return -1;
}

int destroy_list(struct list_head* head){
	if(head->first != NULL){
		free_node(head->first);
	}

	head->count = 0;
	head->first = NULL;
}

int free_node(struct list_node* node){
	if(node->next != NULL){
		free_node(node->next);
	}
	free(node->ip);
	free(node->port);
	free(node);
}

int print_list(struct list_head* head){
	printf("-------- LIST (%d clients connected) ------\n",head->count);
	struct list_node* node = head->first;

	while(node!=NULL){
		printf("[%s,%s,%d]\t", node->ip, node->port, node->id);

		node = node->next;
	}
	if( head->first != NULL )
		printf("\n");
}

int delete_node(struct list_head* head, char *ip, char *port){
	struct list_node* temp = head->first;
	struct list_node* prev = NULL;
	while(temp!=NULL){
		if(strcmp(ip,temp->ip)==0 && strcmp(port,temp->port)==0){
			break;
		}
		prev = temp;
		temp = temp->next;
	}

	if( prev == NULL && head->count == 1){
		free(head->first->ip);
		free(head->first->port);
		free(head->first);
		head->first = NULL;
	}
	else if(prev == NULL && head->count != 1){
		head->first = head->first->next;
		free(temp->ip);
		free(temp->port);
		free(temp);
	}
	else{
		prev->next = temp->next;
		free(temp->ip);
		free(temp->port);
		free(temp);
	}

	head->count--;
}

int get_clients(struct list_head* head, char* list){
	char* temp;
	temp = (char*)malloc((strlen(list)+1)*sizeof(char));
	strcpy(temp, list);


	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *ip;
	char* port;
	char* id;
	char* N;

	N = strtok(temp, s1);
	N = strtok(NULL, s1);
	int count = atoi(N);

	for(int i = 1 ; i <= count ; i++ ){
		ip = strtok(NULL, s3);
		ip++;
		port = strtok(NULL, s3);
		port++;
		id = strtok(NULL, s4);
		id++;

		add_node(head, ip, port, atoi(id));
	}

	free(temp);
}

int send_get_file_list_requests(struct list_head* head, char* my_ip, char* my_port, int my_id){
	struct list_node* temp_node;
	temp_node = head->first;

	while(temp_node != NULL){
		if( my_id != temp_node->id){
			int ID = temp_node->id;
			char dir[100];
			snprintf(dir, 100, "dir%d_copyof%d", my_id, ID);
			mkdir(dir, 0777);


			struct sockaddr_in other_client;
			struct sockaddr *other_clientptr = (struct sockaddr*)&other_client;
			int sock;
			struct hostent *rem;

			/* Create socket */
			if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
				perror_exit("socket");
			/* Find server address */
			if ((rem = gethostbyname(temp_node->ip)) == NULL) {	
				herror("gethostbyname"); exit(1);
			}
			int port = atoi(temp_node->port);

			other_client.sin_family = AF_INET;			/* Internet domain */
			memcpy(&other_client.sin_addr, rem->h_addr, rem->h_length);
			other_client.sin_port = htons(port);		/* Server port */
				/* Initiate connection */
		    while (connect(sock, other_clientptr, sizeof(other_client)) < 0){
				sleep(1);
		    }



			char buf1[256];
			snprintf(buf1,256,"GET_FILE_LIST <%s, %s, %d>",my_ip, my_port, my_id);
			int len = strlen(buf1);
			len++;
			if (write(sock, &len, sizeof(int)) < 0)
				perror_exit("write");
			if (write(sock, buf1, len) < 0){
				perror_exit("write");
			}
			close(sock);
		}

		temp_node = temp_node->next;
	}	
}

int get_request_type(char* buf){
	char* temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);

	const char s1[2] = " ";

	char *request = strtok(temp, s1);
	if( strcmp(request, "GET_FILE_LIST")==0 ){
		free(temp);
		return 1;
	}
	else if( strcmp(request, "GET_FILE")==0 ){
		free(temp);
		return 2;
	}
	else if( strcmp(request, "USER_OFF")==0 ){
		free(temp);
		return 3;
	}
	else if( strcmp(request, "USER_ON")==0 ){
		free(temp);
		return 4;
	}
	else if( strcmp(request, "FILE_LIST")==0 ){
		free(temp);
		return 5;
	}
	else if( strcmp(request, "FILE_SIZE")==0 ){
		free(temp);
		return 6;
	}

	free(temp);
	return 0;
}

int send_file_list(struct thread_pool* pool, char* buf, char* dir, char* my_port, char* my_ip, int my_id, char* my_dir){
	printf("send_file_list\n");
	char* temp;
	temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);


	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *ip;
	char* port;
	ip = strtok(temp, s1);
	ip = strtok(NULL, s3);
	ip++;	// get rid of '<'
	port = strtok(NULL, s4);
	port++;	// get rid of ' '


	struct job temp_job;
	strcpy(temp_job.pathname, dir);
	temp_job.version = -1;
	strcpy(temp_job.ip, ip);
	strcpy(temp_job.port, port);
	temp_job.my_id = my_id;
	strcpy(temp_job.my_dir, my_dir);

	push_job(pool, &temp_job, my_port, my_ip);

	free(temp);
}

int send_file(struct thread_pool* pool, char* buf, char* my_port, char* my_ip, int my_id, char* my_dir){
	printf("send_file\n");
	char* temp;
	temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);

	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *ip;
	char* port;
	char* pathname;
	char* version;
	ip = strtok(temp, s1);
	ip = strtok(NULL, s3);
	ip++;	// get rid of '<'
	port = strtok(NULL, s4);
	port++;	// get rid of ' '
	pathname = strtok(NULL, s3);
	pathname++;
	version = strtok(NULL, s4);
	version++;

	struct job temp_job;
	strcpy(temp_job.pathname, pathname);
	temp_job.version = atoi(version);
	strcpy(temp_job.ip, ip);
	strcpy(temp_job.port, port);
	strcpy(temp_job.my_ip, my_ip);
	strcpy(temp_job.my_port, my_port);
	temp_job.my_id = my_id;
	strcpy(temp_job.my_dir, my_dir);

	push_job(pool, &temp_job, my_port, my_ip);

	free(temp);
}

int remove_client_from_list(struct list_head* list, char* buf){
	printf("remove_client_from_list\n");
	char* temp;
	temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);

	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *ip;
	char* port;
	ip = strtok(temp, s1);
	ip = strtok(NULL, s3);
	ip++;	// get rid of '<'
	port = strtok(NULL, s4);
	port++;	// get rid of ' '

	if( client_exists(list, ip, port) != -1 ){
			delete_node(list, ip, port);
	}
	else{
		printf("Client [%s, %s] doesn t exist\n", ip,port);
		printf("ERROR_IP_PORT_NOT_FOUND_IN_LIST\n");
	}

	free(temp);
}

int add_client_to_list(struct list_head* list, char* buf, int my_id){
	printf("add_client_to_list\n");
	add_client(list, buf, my_id);
}

int request_file_list(char* buf, char* port_str, char* my_ip, int id){
	char* temp;
	temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);

	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *other_clients_ip;
	char* other_clients_port;
	char* other_clients_id;
	other_clients_ip = strtok(temp, s1);
	other_clients_ip = strtok(NULL, s3);
	other_clients_ip++;	// get rid of '<'
	other_clients_port = strtok(NULL, s4);
	other_clients_port++;	// get rid of ' '
	other_clients_id = strtok(NULL, s4);
	other_clients_id++;	// get rid of '<'


	struct sockaddr_in other_client;
	struct sockaddr *other_clientptr = (struct sockaddr*)&other_client;
	int sock;
	struct hostent *rem;

	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit("socket");
	/* Find server address */
	if ((rem = gethostbyname(other_clients_ip)) == NULL) {	
		herror("gethostbyname"); exit(1);
	}
	int port = atoi(other_clients_port);

	other_client.sin_family = AF_INET;			/* Internet domain */
	memcpy(&other_client.sin_addr, rem->h_addr, rem->h_length);
	other_client.sin_port = htons(port);		/* Server port */
	/* Initiate connection */
	while (connect(sock, other_clientptr, sizeof(other_client)) < 0){
		sleep(1);
	}

	char buf1[256];
	snprintf(buf1,256,"GET_FILE_LIST <%s, %s, %d>",my_ip, port_str, id);
	int len = strlen(buf1);
	len++;
	if (write(sock, &len, sizeof(int)) < 0)
		perror_exit("write");
	if (write(sock, buf1, len) < 0){
		perror_exit("write");
	}
	close(sock);

	free(temp);
}

int request_files(char* buf, char* my_port, char* my_ip){
	char* temp;
	temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);

	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *other_clients_ip;
	char* other_clients_port;
	char *num_of_files;
	other_clients_ip = strtok(temp, s1);
	other_clients_ip = strtok(NULL, s3);
	other_clients_ip++;	// get rid of '<'
	other_clients_port = strtok(NULL, s4);
	other_clients_port++;	// get rid of ' '
	num_of_files = strtok(NULL, s1);
	int N = atoi(num_of_files);

	char* file;
	char* version;
	for (int i = 0; i < N; ++i){
		file = strtok(NULL, s3);
		file++;
		version = strtok(NULL, s4);
		version++;

		// printf("%s - %s\n", file, version);

		struct sockaddr_in other_client;
		struct sockaddr *other_clientptr = (struct sockaddr*)&other_client;
		int sock;
		struct hostent *rem;

		/* Create socket */
		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
			perror_exit("socket");
		/* Find server address */
		if ((rem = gethostbyname(other_clients_ip)) == NULL) {	
			herror("gethostbyname"); exit(1);
		}
		int port = atoi(other_clients_port);

		other_client.sin_family = AF_INET;			/* Internet domain */
		memcpy(&other_client.sin_addr, rem->h_addr, rem->h_length);
		other_client.sin_port = htons(port);		/* Server port */
		/* Initiate connection */
		while (connect(sock, other_clientptr, sizeof(other_client)) < 0){
			sleep(1);
		}

		char buf1[256];
		snprintf(buf1,256,"GET_FILE <%s, %s><%s, %s>",my_ip, my_port, file, version);
		int len = strlen(buf1);
		len++;
		if (write(sock, &len, sizeof(int)) < 0)
			perror_exit("write");
		if (write(sock, buf1, len) < 0){
			perror_exit("write");
		}
		close(sock);
	}

	free(temp);
}

int store_file(char* buf, char* my_port, char* my_ip, int my_id){
	char* temp;
	temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);


	const char s1[2] = " ";
	const char s2[2] = "<";
	const char s3[2] = ",";
	const char s4[2] = ">";

	char *ip;
	char* port;
	char* id;
	char* file;
	char* version;
	char* size;
	char* buffer;
	ip = strtok(temp, s1);
	ip = strtok(NULL, s3);
	ip++;	// get rid of '<'
	port = strtok(NULL, s3);
	port++;	// get rid of ' '
	id = strtok(NULL, s4);
	id++;
	file = strtok(NULL, s3);
	file++;
	version = strtok(NULL, s3);
	version++;
	size = strtok(NULL, s3);
	size++;
	buffer = strtok(NULL, s4);
	buffer++;

	char new_file[200];
	snprintf(new_file, 200, "dir%d_copyof%s/%s", my_id, id, file);	// path/filename
	printf("%s\n", new_file);


	FILE *fp;
	fp = fopen(new_file, "w");		// create copy file
	if(fp == NULL){	// subdir case
		add_subdir(new_file);
			
		fp = fopen(new_file, "w+");
	}
	fwrite(buffer, sizeof(char), atoi(size)+1, fp);
	fclose(fp);

	free(temp);
}

int add_subdir(char* new_dir){
	char *temp = (char*)malloc(strlen(new_dir)+1);
	strcpy(temp, new_dir);
	for (int i = strlen(temp); i >0 ; i--){
		if(temp[i] == '/'){
			temp[i] = '\0';
			break;
		}
	}

// create dir
	if( mkdir(temp,0700) == -1){
		add_subdir(temp);
		if( mkdir(temp,0700) == -1){
			printf("cannot create dir\n");
			//terminate
		}	
	}
	
	free(temp);
}