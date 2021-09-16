#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sender_utils.h"

void  ALARMhandler(int sig);
// argv[1] client's id
// argv[2] other client's id
// argv[3] common_dir
// argv[4] input_dir
// argv[5] buffer_size

int main(int argc, char **argv){
	signal(SIGALRM, ALARMhandler);
	alarm(30);

	char * input_dir = argv[4];
	int buffer_size = atoi(argv[5]);
	char *buffer = (char*)malloc(sizeof(char)*buffer_size);

	char other_client_file_name[10];
	strcpy(other_client_file_name, argv[2]);
	const char s[2] = ".";
	char *other_client_id_str;
	other_client_id_str = strtok(other_client_file_name, s);

// fifo for sending the files
	char fifo_send[100];
	sprintf(fifo_send, "%s/id%s_to_id%s.fifo", argv[3], argv[1], other_client_id_str);
	struct stat statb_send;
	int rs = stat(fifo_send, &statb_send);
	if(rs!=0){
		mkfifo(fifo_send, 0666);

	}
// fifo for receiving back acks
	char fifo_send_ACK[100];
	sprintf(fifo_send_ACK, "%s/id%s_to_id%s_ACK.fifo", argv[3], argv[1], other_client_id_str);
	struct stat statb_send_ACK;
	rs = stat(fifo_send_ACK, &statb_send_ACK);
	if(rs!=0){
		mkfifo(fifo_send_ACK, 0666);
	}

// find all files and store their path in a list
	struct list_head head;
	head.count = 0;
	head.first = NULL;
	create_file_list(input_dir, &head);


// loop for writing all the files
	int fd, fd_ACK;
	char ack[4];
	unsigned short length;
	int size;
	struct list_node *node = head.first;
	while (node != NULL) {	//gia kathe kainourgio id sto common dir
		length = node->length;
		size = node->size;
		char *file = node->file_path;
		// printf("%s-%hu-%d\n", file, length, size);

// name length
		fd = open(fifo_send, O_WRONLY);
		write(fd, &length, sizeof(length));
		close(fd);
		fd_ACK = open(fifo_send_ACK,O_RDONLY);
		read(fd_ACK, ack, 4);
		close(fd_ACK);
// name
		fd = open(fifo_send, O_WRONLY);
		write(fd, file, strlen(file)+1);
		close(fd);
		fd_ACK = open(fifo_send_ACK,O_RDONLY);
		read(fd_ACK, ack, 4);
		close(fd_ACK);
// file size
		fd = open(fifo_send, O_WRONLY);
		write(fd, &size, sizeof(size));
		close(fd);
		fd_ACK = open(fifo_send_ACK,O_RDONLY);
		read(fd_ACK, ack, 4);
		close(fd_ACK);
// send file
		FILE *fp;
		char *file_1 = (char*)malloc(strlen(input_dir)+strlen(file)+2);
		sprintf(file_1, "%s/%s", input_dir, file);
		fp = fopen(file_1, "r");
		int offset = 0;
		fd = open(fifo_send, O_WRONLY);
		while(offset < size){
			if(offset+buffer_size > size){
				fread(buffer, size-offset, 1, fp);
				int n = write(fd, buffer, size-offset);
				offset = size;
			}
			else{
				fread(buffer, buffer_size, 1, fp);
				int n = write(fd, buffer, buffer_size);
				offset += buffer_size;
			}
		}
		close(fd);
		fclose(fp);
		fd_ACK = open(fifo_send_ACK,O_RDONLY);
		read(fd_ACK, ack, 4);
		close(fd_ACK);

		free(file_1);	//line 90
		node = node->next;
	}

	length = 0;
	fd = open(fifo_send, O_WRONLY);
	write(fd, &length, sizeof(length));
	close(fd);

	fd_ACK = open(fifo_send_ACK,O_RDONLY);
	read(fd_ACK, ack, 4);
	close(fd_ACK);

// free fifo ack
	struct stat statb1;
	rs = stat(fifo_send_ACK, &statb1);
	if(rs==0){
		char rm[100];
		sprintf(rm, "rm %s", fifo_send_ACK);
		system(rm);
		// printf("%s\n", rm);
	}


	free_file_list(&head);
	free(buffer);
}

void  ALARMhandler(int sig){
  printf("Sender time expired\n");
  exit(-1);
}