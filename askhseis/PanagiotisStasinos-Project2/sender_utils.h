#ifndef SENDERUTILS_H_
#define SENDERUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

struct list_node{
	unsigned short length;
	int size;
	char *file_path;

	struct list_node* next;
};

struct list_head{
	int count;
	struct list_node* first;
};

int create_file_list(char* dir, struct list_head* head);	//anadromikh, create a list of all files in dir, and those in subdirs too
int insert_file(char* file_path, int size, struct list_head* head);	//adds given file to list
int print_file_list(struct list_head* head);
int free_file_list(struct list_head* head);
int free_node(struct list_node* node);

#endif