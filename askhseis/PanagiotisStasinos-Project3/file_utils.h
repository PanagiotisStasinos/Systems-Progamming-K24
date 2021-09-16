#ifndef FILEUTILS_H_
#define FILEUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

struct file_node{
	unsigned short length;
	int size;
	char *file_path;

	struct file_node* next;
};

struct file_list_head{
	int count;
	struct file_node* first;
};

int create_file_list(char* dir, struct  file_list_head* head);	//anadromikh, create a list of all files in dir, and those in subdirs too
int insert_file(char* file_path, int size, struct  file_list_head* head);	//adds given file to list
int print_file_list(struct  file_list_head* head);
int free_file_list(struct  file_list_head* head);
int free_file_node(struct  file_node* node);

#endif