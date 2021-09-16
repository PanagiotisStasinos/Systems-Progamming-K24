#ifndef CLIENT_UTILS_H_
#define CLIENT_UTILS_H_

#include "thread_pool.h"

struct list_node{
	char* ip;
	char* port;
	int id;

	struct list_node *next;
};

struct list_head{
	int count;
	struct list_node *first;
	struct list_node *last;
};

void perror_exit(char *message);

int create_list(struct list_head* head);
int add_client(struct list_head* head,char* client_log_on_info, int my_id);
int add_node(struct list_head* head, char *ip, char *port, int id);
int client_exists(struct list_head* head, char *ip, char *port);	// return 0 if exists, -1 if not
int destroy_list(struct list_head* head);
int free_node(struct list_node* node);
int print_list(struct list_head* head);
int delete_node(struct list_head* head, char *ip, char *port);

int get_clients(struct list_head* head, char* list);
int send_get_file_list_requests(struct list_head* head, char* my_ip, char* my_port, int my_id);
int get_request_type(char* buf);

int send_file_list(struct thread_pool* pool, char* buf, char* dir, char* my_port, char* my_ip, int my_id, char* my_dir);
int send_file(struct thread_pool* pool, char* buf, char* my_port, char* my_ip, int my_id, char* my_dir);
int remove_client_from_list(struct list_head* list, char* buf);
int add_client_to_list(struct list_head* list, char* buf, int my_id);

int request_file_list(char* buf, char* port_str, char* my_ip, int id);
int request_files(char* buf, char* my_port, char* my_ip);		// takes file list and ask for files

int store_file(char* buf, char* my_port, char* my_ip, int my_id);
int add_subdir(char* new_dir);

#endif