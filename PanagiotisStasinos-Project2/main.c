#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h> 

#include "utils.h"

#define PRINTS

void sighandler(int signum);

char *m_dir;
char *c_dir;
char path[100];
DIR *mirror_dir_ptr;
DIR *common_dir_ptr;
struct list_head* l_1;
struct list_head* l_2;

int main(int argc, char **argv){
	///////////////////////
	/*     Arguments     */
	///////////////////////

	// flags
	char id_flag[3] = "-n\0";
	char common_dir_flag[3] = "-c\0";
	char input_dir_flag[3] = "-i\0";
	char mirror_dir_flag[3] = "-m\0";
	char buffer_size_flag[3] = "-b\0";
	char log_file_flag[3] = "-l\0";

	// arguments
	int id;	char *str_id;
	char* common_dir = NULL;
	char* input_dir = NULL;
	char* mirror_dir = NULL;
	int buffer_size;	char *b_size;
	char* log_file = NULL;

	if( argc == 13 ){
		for(int i=1; i<=11; i+=2){
			if( strcmp(argv[i],id_flag)==0 ){
				str_id = argv[i+1];
				id = atoi(argv[i+1]);
			}
			else if( strcmp(argv[i],common_dir_flag)==0 ){
				common_dir = argv[i+1];
				c_dir = argv[i+1];
			}
			else if( strcmp(argv[i],input_dir_flag)==0 ){
				input_dir = argv[i+1];
			}
			else if( strcmp(argv[i],mirror_dir_flag)==0 ){
				mirror_dir = argv[i+1];
				m_dir = argv[i+1];
			}
			else if( strcmp(argv[i],buffer_size_flag)==0 ){
				buffer_size = atoi(argv[i+1]);
				b_size = argv[i+1];
			}
			else if( strcmp(argv[i],log_file_flag)==0 ){
				log_file = argv[i+1];
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

#ifdef PRINTS
	printf("Id : %d\ncommon dir : %s\ninput dir : %s\nmirror dir : %s\nbuffer size %d\nlog file : %s\n", id, common_dir, input_dir, mirror_dir, buffer_size, log_file);
#endif

	///////////////////////
	/*     1             */
	///////////////////////
	// check if input dir exists
	DIR *input_dir_ptr = opendir(input_dir);
	if(input_dir_ptr){
		printf("dir %s exists\n",input_dir);
	}
	else if(ENOENT == errno){
		printf("dir %s does not exist\n",input_dir);
		return -1;
	}
	else{
		printf("cannot open dir %s\n",input_dir);
		return -1;
	}
	closedir(input_dir_ptr);

	// check if mirror dir exists, if not create it
	mirror_dir_ptr = opendir(mirror_dir);
	if(mirror_dir_ptr){
		printf("dir %s exists\n",mirror_dir);
		return -1;
	}
	else if(ENOENT == errno){
		printf("dir %s does not exist\n",mirror_dir);
		int status = mkdir(mirror_dir,0700);
		printf("%d\n", status);
		mirror_dir_ptr = opendir(mirror_dir);
	}

	// check if common dir exists, if not create it
	common_dir_ptr = opendir(common_dir);
	if(common_dir_ptr){
		printf("dir %s exists\n",common_dir);
	}
	else if(ENOENT == errno){
		printf("dir %s does not exist\n",common_dir);
		int status = mkdir(common_dir,0700);
		printf("%d\n", status);
		common_dir_ptr = opendir(common_dir);
	}

	///////////////////////
	/*        2          */
	///////////////////////
	char id_file[10];
	sprintf(id_file, "%d.id", id);
	pid_t my_pid = getpid();
	printf("file %s %d\n", id_file, my_pid);

	FILE *fptr;
	sprintf(path, "%s/%s", common_dir, id_file);
	if( access( path, F_OK ) != -1 ) {
    	printf("file %s allready exists\n", id_file);
    	return -1;
	}
	fptr = fopen(path, "w+");
	if(fptr==NULL){
		printf("Unable to create file %s\n", id_file);
		return -1;
	}


	char pid[10];
	sprintf(pid, "%d", (int)my_pid);
	int r = fprintf(fptr, "%s\n", pid);
	printf("%d\n", r);
	fclose(fptr);


	///////////////////////
	/*        3-4        */
	///////////////////////
	struct dirent *ent;
	FILE *fp;
	char file[280];
	char c;
	signal(SIGINT, sighandler);
	signal(SIGQUIT, sighandler);

	pid_t sender;
	pid_t receiver;
	pid_t deleter;

	struct list_head list_1;	init_list(&list_1);	l_1 = &list_1;
	struct list_head list_2;	init_list(&list_2);	l_2 = &list_2;
	struct list_head* current_list = &list_1;
	struct list_head* prev_list = &list_2;
	struct list_head* temp_list = NULL;

	int loop=0;
	while(1){
		printf("-------- loop %d\n", loop);
		// to reset dir pointer
		closedir(common_dir_ptr);
		common_dir_ptr = opendir(common_dir);
		//////////////////////

		while ((ent = readdir (common_dir_ptr)) != NULL) {	//gia kathe kainourgio id sto common dir
			if (!strcmp (ent->d_name, "."))
				continue;
			if (!strcmp (ent->d_name, ".."))    
				continue;
			if (!strcmp (ent->d_name, id_file))
				continue;
			if (is_idfile(ent->d_name)!=0)
				continue;

			sprintf(file,"%s/%s", common_dir, ent->d_name);
			fp = fopen(file, "r");
			if(fp!=NULL){
				int other_client_id = get_id(ent->d_name);
				push(current_list, other_client_id);
				if( (exists(prev_list, other_client_id)) == 0){
					fclose(fp);
					continue;
				}

				sender = fork();
				if(sender==-1){
					printf("failed to fork\n");
				}
				else if(sender==0){
					execl("./sender", "./sender", str_id, ent->d_name, common_dir, input_dir, b_size, NULL);
				}
				
				receiver = fork();
				if(receiver==-1){
					printf("failed to fork\n");
				}
				else if(receiver==0){
					execl("./receiver","./receiver", str_id, ent->d_name, common_dir, mirror_dir, b_size, log_file, NULL);
				}
				

				fclose(fp);
				int status1, status2;
				waitpid(sender, &status1, 0);
				waitpid(receiver, &status2, 0);
				
				if(status2!=0 || status1!=0){
					printf("FATHER failed with client %d\n" , other_client_id);
					// pull(current_list,  other_client_id);	// for retrying, doesnt work
				}
				else{
					printf("FATHER ok with client %d\n" ,other_client_id);
				}
			}
		}
		printf("\nCurrent clients ");
		print(current_list);
		printf("Prev clients ");
		print(prev_list);


// delete mirror dir of a client that has terminated 
		struct list_node *tmp_node = prev_list->first;
		while(tmp_node != NULL){
			if( exists(current_list,tmp_node->id) != 0){
				deleter = fork();
				if(deleter==-1){
					printf("failed to fork\n");
				}
				else if(deleter==0){
					char other_client_id_str[10];
					sprintf(other_client_id_str, "%d", tmp_node->id);
					execl("./deleter", "./deleter", str_id, m_dir, other_client_id_str, NULL);
				}
				int status;
				waitpid(deleter, &status, 0);
			}
			tmp_node = tmp_node->next;
		}

// updating alive clients
		temp_list = prev_list;
		prev_list = current_list;
		free_list(temp_list);
		init_list(temp_list);
		current_list = temp_list;
		printf("-------- end loop %d\n\n", loop);

		sleep(5);
		loop++;
	}

	return 0;
}

void sighandler(int signum){
	printf("\nCaught signal %d, coming out...\n", signum);
	int ret_val = remove(path);
	if(ret_val!=0){
		printf("Error: unable to delete file %s ", path);
	}


	closedir(mirror_dir_ptr);
	closedir(common_dir_ptr);

	int status = rmdir(m_dir);
	// printf("%d\n", status);
	if(status == -1){
		char rm[20];
		sprintf(rm, "rm -rf %s", m_dir);
		system(rm);
	}
	else{
		printf("empty directory %s deleted\n", m_dir);
	}

	char rm[100];
	sprintf(rm, "rm %s/*.fifo", c_dir);
	system(rm);

	free_list(l_1);
	free_list(l_2);

	printf("END!\n");
	exit(0);
}