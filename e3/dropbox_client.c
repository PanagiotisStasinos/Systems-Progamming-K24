#include <stdio.h>
#include <sys/types.h>			/* sockets */
#include <sys/socket.h>			/* sockets */
#include <netinet/in.h>			/* internet sockets */
#include <unistd.h>				/* read, write, close */
#include <netdb.h>				/* gethostbyaddr */
#include <stdlib.h>				/* exit */
#include <string.h>				/* strlen */
#include <signal.h>				/* signal */

#include <arpa/inet.h> 

#include "client_utils.h"
#include "thread_pool.h"
#include "file_utils.h"

#define MAX_CONS 20		// maximum length to which the queue of pending connections

void sighandler(int signum);

int newsock1;

int port, sock, i;
char buf[256];
struct sockaddr_in server;
struct sockaddr *serverptr = (struct sockaddr*)&server;
struct hostent *rem;

struct thread_pool pool;
struct list_head clients;

// arguments
char *dirName = NULL;
int portNum;	char* port_str;
int workerThreads;
int bufferSize;
int serverPort;
char* serverIP = NULL;

//	get my ip and port
char hostbuffer[256]; 
char *IPbuffer; 
struct hostent *host_entry; 
int hostname; 

int main(int argc, char **argv){
	///////////////////////
	/*     Arguments     */
	///////////////////////

	// flags
	char dirName_flag[3] = "-d\0";
	char portNum_flag[3] = "-p\0";
	char workerThreads_flag[3] = "-w\0";
	char bufferSize_flag[3] = "-b\0";
	char serverPort_flag[4] = "-sp\0";
	char serverIP_flag[5] = "-sip\0";

	if( argc == 13 ){
		for(int i=1; i<=11; i+=2){
			if( strcmp(argv[i],dirName_flag)==0 ){
				dirName = argv[i+1];
			}
			else if( strcmp(argv[i],portNum_flag)==0 ){
				port_str = argv[i+1];
				portNum = atoi(argv[i+1]);
			}
			else if( strcmp(argv[i],workerThreads_flag)==0 ){
				workerThreads = atoi(argv[i+1]);
			}
			else if( strcmp(argv[i],bufferSize_flag)==0 ){
				bufferSize = atoi(argv[i+1]);
			}
			else if( strcmp(argv[i],serverPort_flag)==0 ){
				serverPort = atoi(argv[i+1]);
			}
			else if( strcmp(argv[i],serverIP_flag)==0 ){
				serverIP = argv[i+1];
			}
			else{
				printf("Error in arguments given!\nProgram Terminated\n");
				return(0);  //terminating program
			}
		}
	}
	else{
		printf("Error in arguments given!\nProgram Terminated\n");
		return(0);  //terminating program
	}

	// printf("dirName: %s\nportNum: %d\nworkerThreads: %d\nbufferSize: %d\nserverPort %d\nserverIP %s\n", dirName, portNum, workerThreads,bufferSize, serverPort, serverIP);


	/////////////////////////////
	/*    Get my IP and Port   */
	/////////////////////////////
	// To retrieve hostname 
	hostname = gethostname(hostbuffer, sizeof(hostbuffer));

	// To retrieve host information 
	host_entry = gethostbyname(hostbuffer);

	// To convert an Internet network 
	// address into ASCII string 
	IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0])); 

	printf("Hostname: %s\n", hostbuffer); 
	printf("Host IP: %s\n", IPbuffer);

	//////////////////////////
	/*      ...........     */
	//////////////////////////

// LOG ON
	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit("socket");
	/* Find server address */
	if ((rem = gethostbyname(serverIP)) == NULL) {	
		herror("gethostbyname"); exit(1);
	}
	port = serverPort;

	server.sin_family = AF_INET;       /* Internet domain */
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);         /* Server port */
	/* Initiate connection */
	if (connect(sock, serverptr, sizeof(server)) < 0){
		perror_exit("connect");
    }
//	printf("Connecting to %s port %d\n", serverIP, port);
	snprintf(buf,256,"LOG_ON <%s, %s>",IPbuffer, port_str);
	int len = strlen(buf);
	len++;
	if (write(sock, &len, sizeof(int)) < 0)
		perror_exit("write");
	if (write(sock, buf, len) < 0){
		perror_exit("write");
	}
	close(sock);

	printf("\nLOGGED IN\n");



// GET CLIENTS
	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit("socket");
	/* Find server address */
	if ((rem = gethostbyname(serverIP)) == NULL) {	
		herror("gethostbyname"); exit(1);
	}
	port = serverPort;

	server.sin_family = AF_INET;       /* Internet domain */
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port);
	/* Initiate connection */
	if (connect(sock, serverptr, sizeof(server)) < 0){
		perror_exit("connect");
	}
	snprintf(buf,256,"GET_CLIENTS <%s, %s>",IPbuffer, port_str);
	len = strlen(buf);
	len++;
	if (write(sock, &len, sizeof(int)) < 0)
		perror_exit("write");
	if (write(sock, buf, len) < 0){
		perror_exit("write");
	}

	char* list;
	read(sock, &len, sizeof(int));
	list = (char*)malloc(len*sizeof(char));
	read(sock, list, len);
	close(sock);                 /* Close socket and exit */
printf("%s\n", list);

		//////////////////////////
		/*      thread pool     */
		//////////////////////////
	create_thread_pool(&pool, workerThreads, bufferSize);

	create_list(&clients);

	get_clients(&clients, list);
	send_get_file_list_requests(&clients, IPbuffer, port_str, client_exists(&clients, IPbuffer, port_str));
	free(list);
	printf("\nGET_CLIENTS ");
	print_list(&clients);
	printf("------------------------------------------------------\n\n");


// server part
	int port1, sock1;
	struct sockaddr_in server1;
	struct sockaddr_in client1;
	socklen_t clientlen1;
	struct sockaddr *serverptr1=(struct sockaddr *)&server1;
	struct sockaddr *clientptr1=(struct sockaddr *)&client1;
	struct hostent *rem1;
	int opt = 1;

	port1 = portNum;

	signal(SIGINT, sighandler);
	signal(SIGQUIT, sighandler);

	/* Create socket */
	if ((sock1 = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror_exit("socket");
	}
		// Forcefully attaching socket to the port 8080 
	if (setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	}
	
	server1.sin_family = AF_INET;		/* Internet domain */
	server1.sin_addr.s_addr = htonl(INADDR_ANY);
	server1.sin_port = htons(port1);		/* The given port */

	/* Bind socket to address */
	if (bind(sock1, serverptr1, sizeof(server1)) < 0){
		perror_exit("bind");
	}

	if (listen(sock1, MAX_CONS) < 0){	//max connections
		perror_exit("listen");
	}
	printf("Listening for connections to port %d\n", port1);
	clientlen1 = sizeof(client1);

	int loop = 0;
	char* buf1;
	int len1;
	while (1) {
		printf("\nLOOP %d ", loop);
		loop++;
		print_list(&clients);

		/* accept connection */
		if ((newsock1 = accept(sock1, clientptr1, &clientlen1)) < 0){
			perror_exit("accept");
		}

		read(newsock1, &len1, sizeof(int));	// read length of request
		printf("(%d)", len1);
		buf1 = (char*)malloc(len1*sizeof(char));
		read(newsock1, buf1, len1);	// read request
		printf("%s\n", buf1);

		int type = get_request_type(buf1);

		if( type == 1 ){			// GET_FILE_LIST
			send_file_list(&pool, buf1, dirName, port_str, IPbuffer, client_exists(&clients, IPbuffer, port_str), dirName);
		}
		else if( type == 2 ){		// GET_FILE
			send_file(&pool, buf1, port_str, IPbuffer, client_exists(&clients, IPbuffer, port_str), dirName);
		}
		else if( type == 3 ){		// USER_OFF
			remove_client_from_list(&clients, buf1);
		}
		else if( type == 4 ){		// USER_ON
			add_client_to_list(&clients, buf1, client_exists(&clients, IPbuffer, port_str));

			request_file_list(buf1, port_str, IPbuffer, client_exists(&clients, IPbuffer, port_str));
		}
		else if( type == 5 ){		// FILE_LIST
			request_files(buf1, port_str, IPbuffer);
		}
		else if( type == 6 ){		// FILE_SIZE
			store_file(buf1, port_str, IPbuffer, client_exists(&clients, IPbuffer, port_str));
		}

		close(newsock1);	/* Close socket */
		free(buf1);

		printf("-------------------------------------------------\n");
	}
}


void sighandler(int signum){
	printf("\nCaught signal %d, coming out...\n", signum);

	destroy_list(&clients);
	thread_pool_destroy(&pool);
	
// LOG_OFF
	/* Create socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		perror_exit("socket");
	/* Find server address */
	if ((rem = gethostbyname(serverIP)) == NULL) {	
		herror("gethostbyname"); exit(1);
	}
	port = serverPort;

	server.sin_family = AF_INET;		/* Internet domain */
	memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
	server.sin_port = htons(port); 
	/* Initiate connection */
	if (connect(sock, serverptr, sizeof(server)) < 0){
		perror_exit("connect");
	}

	snprintf(buf,256,"LOG_OFF <%s, %s>",IPbuffer, port_str);
	int len = strlen(buf);
	len++;
	if (write(sock, &len, sizeof(int)) < 0)
		perror_exit("write");
	if (write(sock, buf, len) < 0){
		perror_exit("write");
	}
	close(sock);				/* Close socket and exit */

	printf("END!\n");
	exit(0);
}