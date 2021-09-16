#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

struct list_node{
	int id;
	struct list_node* next;
};

struct list_head{
	int count;
	struct list_node* first;
};

int is_idfile(char* f_name);
int get_id(char* f_name);	//returns id


//list functions
int init_list(struct list_head* head);
int get_len(struct list_head* head);
int push(struct list_head* head, int id);
int pull(struct list_head* head, int id);
int free_list(struct list_head* head);
int free_node(struct list_node* node);
int print(struct list_head* head);
int exists(struct list_head* head, int id);	//returns 0 if exists, -1 elsewhere
#endif