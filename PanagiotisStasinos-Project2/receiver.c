#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>

void  ALARMhandler(int sig);
int add_subdir(char* new_dir);

// argv[1] client's id
// argv[2] other client's id
// argv[3] common_dir
// argv[4] mirror_dir
// argv[5] buffer_size
// argv[6] log_file

int main(int argc, char **argv){
	signal(SIGALRM, ALARMhandler);
	alarm(30);

	char * mirror_dir = argv[4];
	char *log_file = argv[6];
	int buffer_size = atoi(argv[5]);
	char *buffer = (char*)malloc(sizeof(char)*buffer_size);

// log file
	FILE *fpl = NULL;
	fpl = fopen(log_file, "a");

// create mirror dir
	char other_client_file_name[10];
	strcpy(other_client_file_name, argv[2]);
	const char s[2] = ".";
	char *other_client_id_str;
	other_client_id_str = strtok(other_client_file_name, s);
	char new_mirror_dir[100];
	sprintf(new_mirror_dir, "%s/%s",mirror_dir, other_client_id_str);
	struct stat statb_mir;
	int mir = stat(new_mirror_dir, &statb_mir);
	if(mir!=0){
		if ( mkdir(new_mirror_dir,0700) == -1){
			printf("cannot create dir\n");
		}
	}

// fifo for receiving the files
	char fifo_receive[100];
	sprintf(fifo_receive, "%s/id%s_to_id%s.fifo", argv[3], other_client_id_str, argv[1]);
	struct stat statb_receive;
	int rr = stat(fifo_receive, &statb_receive);
	if(rr!=0){
		mkfifo(fifo_receive, 0666);
	}
// fifo to send back acks to sender
	char fifo_receive_ACK[100];
	sprintf(fifo_receive_ACK, "%s/id%s_to_id%s_ACK.fifo", argv[3], other_client_id_str, argv[1]);
	struct stat statb_receive_ACK;
	rr = stat(fifo_receive_ACK, &statb_receive_ACK);
	if(rr!=0){
		mkfifo(fifo_receive_ACK, 0666);
	}


	int fd, fd_ACK;
	char *file = NULL;
	unsigned short length = 0;
	char *path = NULL;
	int size;
	int n;
	while(1){
// name length
		fd = open(fifo_receive,O_RDONLY);
		n = read(fd, &length, sizeof(length));
		close(fd);
		if(n==sizeof(length)){
			fd_ACK = open(fifo_receive_ACK, O_WRONLY);
			write(fd_ACK, "ACK", 4);
			close(fd_ACK);
		}
		else{
			printf("\nERROR\n");
			return -1;
		}

// no more files
		if(length==0){
			printf("No more files\n\n");
			break;
		}
file = (char*)malloc(length+1);
// file name
		fd = open(fifo_receive,O_RDONLY);
		n = read(fd, file, length+1);
		close(fd);
		if(n==length+1){
			fd_ACK = open(fifo_receive_ACK, O_WRONLY);
			write(fd_ACK, "ACK", 4);
			close(fd_ACK);
		}
		else{
			printf("\nERROR\n");
			return -1;
		}

// file size
		fd = open(fifo_receive,O_RDONLY);
		n = read(fd, &size, sizeof(size));
		close(fd);
		if(n==sizeof(size)){
			fd_ACK = open(fifo_receive_ACK, O_WRONLY);
			write(fd_ACK, "ACK", 4);
			close(fd_ACK);
		}
		else{
			printf("\nERROR\n");
			return -1;
		}

path = (char*)malloc(strlen(new_mirror_dir)+strlen(file)+2);
// read file
		sprintf(path, "%s/%s", new_mirror_dir, file);
		FILE *fp;
		fp = fopen(path, "w+");
		if(fp == NULL){	// subdir case
			add_subdir(path);
			
			fp = fopen(path, "w+");
		}
		int offset = 0;
		fd = open(fifo_receive,O_RDONLY);
		while(offset < size){
			if(offset+buffer_size > size){
				int n = read(fd, buffer, size-offset);
				fwrite(buffer, size-offset, 1, fp);
				offset = size;
				break;
			}
			else{
				int n = read(fd, buffer, buffer_size);
				fwrite(buffer, buffer_size, 1, fp);
				offset = offset + buffer_size;
			}
		}
		close(fd);
		fclose(fp);
		fd_ACK = open(fifo_receive_ACK, O_WRONLY);
		write(fd_ACK, "ACK", 4);
		close(fd_ACK);

		printf("Client %s : |%s|-|%hu|-|%d|\n", other_client_id_str, path, length, size);
		fprintf(fpl, "%d\t%d\t%s\t%d\n", atoi(argv[1]), atoi(other_client_id_str), path, size);

free(file);	file=NULL;
free(path);	path=NULL;
	}
// close log file
	fclose(fpl);

// free fifo
	struct stat statb;
	rr = stat(fifo_receive, &statb);
	if(rr==0){
		char rm[100];
		sprintf(rm, "rm %s", fifo_receive);
		system(rm);
		// printf("%s\n", rm);
	}

	free(buffer);
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

void  ALARMhandler(int sig){
  printf("Receiver time expired\n");
  exit(-1);
}