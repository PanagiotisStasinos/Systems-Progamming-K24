#include "server_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */


void perror_exit(char *message) {
	perror(message);
	exit(EXIT_FAILURE);
}


int create_list(struct list_head* head){
	head->count = 0;
	head->total = 0;
	head->first = NULL;
	head->last = NULL;
}

int add_client(struct list_head* head, char* client_log_on_info){
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
	port = strtok(NULL, s4);
	port++;	// get rid of ' '


	if( client_exists(head, ip, port)==-1 ){
		add_node(head, ip, port);
	}
	else{
		printf("Client [%s, %s] exists\n", ip,port);
	}

	free(temp);
}

int add_node(struct list_head* head, char *ip, char *port){
	struct list_node* temp = head->first;
	while(temp!=NULL){
			/* Create socket */
		char buf[256];
		struct sockaddr_in client;
		struct sockaddr *clientptr = (struct sockaddr*)&client;
		int sock;
		struct hostent *rem;
		char* client_ip = temp->ip;
		if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
			perror_exit("socket");
		/* Find server address */
		if ((rem = gethostbyname(client_ip)) == NULL) {	
			herror("gethostbyname"); exit(1);
		}
		int client_port = atoi(temp->port);			
		client.sin_family = AF_INET;		/* Internet domain */
		memcpy(&client.sin_addr, rem->h_addr, rem->h_length);
		client.sin_port = htons(client_port);		/* Server port */
		/* Initiate connection */
		if (connect(sock, clientptr, sizeof(client)) < 0){
			perror_exit("connect");
		}

		snprintf(buf,256,"USER_ON <%s, %s, %d>",ip, port, head->total+1);
		int len = strlen(buf);
		len++;
		if (write(sock, &len, sizeof(int)) < 0)
			perror_exit("write");
		if (write(sock, buf, len) < 0){
			perror_exit("write");
		}
		close(sock);

		temp = temp->next;
	}


	temp= NULL;
	temp = (struct list_node*)malloc(sizeof(struct list_node));
	temp->ip = (char*)malloc((strlen(ip)+1)*sizeof(char));
	temp->port = (char*)malloc((strlen(port)+1)*sizeof(char));

	strcpy(temp->ip, ip);
	strcpy(temp->port, port);
	temp->id = head->total+1;
	temp->next = NULL;

	if(head->count==0){	// first clinet in list
		head->first  = temp;
	}
	else{
		head->last->next = temp;
	}
	head->last = temp;
	head->count++;
	head->total++;
}

int client_exists(struct list_head* head, char *ip, char *port){
	struct list_node* temp = head->first;
	while(temp!=NULL){
		if(strcmp(ip,temp->ip)==0 && strcmp(port,temp->port)==0){
			return 0;
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
	printf("-------- LIST (%d clients connected) -----\n",head->count);
	struct list_node* node = head->first;

	while(node!=NULL){
		printf("[%s,%s,%d]\t", node->ip, node->port, node->id);

		node = node->next;
	}
	if( head->first != NULL )
		printf("\n");
}

int request_type(char* buf){
	char* temp = (char*)malloc((strlen(buf)+1)*sizeof(char));
	strcpy(temp, buf);

	const char s1[2] = " ";

	char *request = strtok(temp, s1);
	if( strcmp(request, "LOG_ON")==0 ){
		// printf("LOG_ON\n");
		free(temp);
		return 1;
	}
	else if( strcmp(request, "GET_CLIENTS")==0 ){
		// printf("GET_CLIENTS\n");
		free(temp);
		return 2;
	}
	else if( strcmp(request, "LOG_OFF")==0 ){
		// printf("LOG_OF\n");
		free(temp);
		return 3;
	}

	free(temp);
	return 0;
}

int get_clients(struct list_head* head,char* client_log_on_info, int sock){
	char* temp;
	temp = (char*)malloc((strlen(client_log_on_info)+1)*sizeof(char));
	strcpy(temp, client_log_on_info);

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

	//create CLENT_LIST message
	char message1[512], message2[512];
	snprintf(message1,512,"CLENT_LIST %d ",head->count);
	char *temp1, *temp2, *temp3;
	temp1 = message1;
	temp2 = message2;

	struct list_node* node = head->first;
	while(node!=NULL){
		snprintf(temp2,512,"%s<%s, %s, %d>", temp1, node->ip, node->port, node->id);
		temp3 = temp1;
		temp1 = temp2;
		temp2 = temp3;

		node = node->next;
	}

	int len = strlen(temp1);
	len++;
	if (write(sock, &len, sizeof(int)) < 0)
		perror_exit("write");
	if (write(sock, temp1, len) < 0){
		perror_exit("write");
	}

	free(temp);
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

	temp = head->first;
	if(temp==NULL){
		head->last=NULL;
	}
	else{
		while(temp->next!=NULL){
			temp = temp->next;
		}
		head->last=temp;
	}
	head->count--;
}

int log_off_client(struct list_head* head,char* client_log_on_info){
	char* temp;
	temp = (char*)malloc((strlen(client_log_on_info)+1)*sizeof(char));
	strcpy(temp, client_log_on_info);

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

	if( client_exists(head, ip, port)==0 ){
		delete_node(head, ip, port);

		struct list_node* temp = head->first;
		while(temp!=NULL){
				/* Create socket */
			char buf[256];
			struct sockaddr_in client;
			struct sockaddr *clientptr = (struct sockaddr*)&client;
			int sock;
			struct hostent *rem;
			char* client_ip = temp->ip;
			if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
				perror_exit("socket");
			/* Find server address */
			if ((rem = gethostbyname(client_ip)) == NULL) {	
				herror("gethostbyname"); exit(1);
			}
			int client_port = atoi(temp->port);			
			client.sin_family = AF_INET;       /* Internet domain */
			memcpy(&client.sin_addr, rem->h_addr, rem->h_length);
			client.sin_port = htons(client_port);         /* Server port */
		/* Initiate connection */
			if (connect(sock, clientptr, sizeof(client)) < 0){
				perror_exit("connect");
			}

			snprintf(buf,256,"USER_OFF <%s, %s>",ip, port);
			int len = strlen(buf);
			len++;
			if (write(sock, &len, sizeof(int)) < 0)
				perror_exit("write");
			if (write(sock, buf, len) < 0){
				perror_exit("write");
			}
			close(sock);

			temp = temp->next;
		}
	}
	else{
		printf("Client [%s, %s] doesn t exist\n", ip,port);
		printf("ERROR_IP_PORT_NOT_FOUND_IN_LIST\n");
	}

	free(temp);	
}