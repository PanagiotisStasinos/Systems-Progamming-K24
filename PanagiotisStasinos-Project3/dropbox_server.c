#include <stdio.h>
#include <string.h>
#include <sys/wait.h>	     /* sockets */
#include <sys/types.h>	     /* sockets */
#include <sys/socket.h>	     /* sockets */
#include <netinet/in.h>	     /* internet sockets */
#include <netdb.h>	         /* gethostbyaddr */
#include <unistd.h>	
#include <stdlib.h>	         /* exit */
#include <ctype.h>	         /* toupper */
#include <signal.h>          /* signal */

#include <arpa/inet.h>

#include "server_utils.h"


#define MAX_CONS 20		// maximum length to which the queue of pending connections


void sighandler(int signum);

struct list_head clients_list;
int newsock;

int main(int argc, char **argv){
	///////////////////////
	/*     Arguments     */
	///////////////////////

	// flags
	char portNum_flag[3] = "-p\0";
	int portNum;

	if( argc == 3 ){
		if( strcmp(argv[1],portNum_flag)==0 ){
			portNum = atoi(argv[2]);
		}
		else{
			printf("Error in arguments given!\nProgram Terminated\n");
			return(0);  //terminating program
		}
	}
	else{
		printf("Error in arguments given!\nProgram Terminated\n");
		return(0);  //terminating program
	}

	/////////////////////////////
	/*    Get my IP and Port   */
	/////////////////////////////
	char hostbuffer[256]; 
	char *IPbuffer; 
	struct hostent *host_entry; 
	int hostname;
	// To retrieve hostname 
	hostname = gethostname(hostbuffer, sizeof(hostbuffer));

	// To retrieve host information 
	host_entry = gethostbyname(hostbuffer);

	// To convert an Internet network 
	// address into ASCII string 
	IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0])); 

	printf("Hostname: %s\n", hostbuffer);
	printf("Host IP: %s\n", IPbuffer);

	///////////////////////
	/*     .........     */
	/////////////////////// 
	int port, sock;
	struct sockaddr_in server, client;
	socklen_t clientlen;
	struct sockaddr *serverptr=(struct sockaddr *)&server;
	struct sockaddr *clientptr=(struct sockaddr *)&client;
	struct hostent *rem;
	int opt = 1;

	port = portNum;

	signal(SIGINT, sighandler);
	signal(SIGQUIT, sighandler);

	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror_exit("socket");
	}
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}

	server.sin_family = AF_INET;		/* Internet domain */
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);		/* The given port */

	/* Bind socket to address */
	if (bind(sock, serverptr, sizeof(server)) < 0){
		perror_exit("bind");
	}

	if (listen(sock, MAX_CONS) < 0){	//max connections
		perror_exit("listen");
	}
	printf("Listening for connections to port %d\n", port);
	clientlen = sizeof(client);

	create_list(&clients_list);
	int loop = 0;
	char* buf;
	int len;
	while (1) {
		printf("\nLOOP %d ", loop);
		loop++;
		print_list(&clients_list);

		/* accept connection */
		if ((newsock = accept(sock, clientptr, &clientlen)) < 0){
			perror_exit("accept");
		}

		read(newsock, &len, sizeof(int));	// read length of request
		printf("request : (%d)", len);
		buf = (char*)malloc(len*sizeof(char));
		read(newsock, buf, len);	// read request
		printf("%s\n", buf);


		int type = request_type(buf);	// LOG_ON or GET_CLIENTS or LOG_OFF

		if( type == 1 ){
			add_client(&clients_list, buf);
		}
		else if( type == 2 ){
			get_clients(&clients_list, buf, newsock);
		}
		else if( type == 3 ){
			log_off_client(&clients_list, buf);
		}


		close(newsock);	  /* Close socket */
		free(buf);
		printf("-------------------------------------------------\n");
	}
}

void sighandler(int signum){
	printf("\nCaught signal %d, coming out...\n", signum);
	close(newsock);	  /* Close socket */
	destroy_list(&clients_list);
	printf("END!\n");
	exit(0);
}