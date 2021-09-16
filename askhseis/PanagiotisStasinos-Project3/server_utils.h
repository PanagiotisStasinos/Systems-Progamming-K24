#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

struct list_node{
	char* ip;
	char* port;
	int id;

	struct list_node *next;
};

struct list_head{
	int count;
	int total;
	struct list_node *first;
	struct list_node *last;
};

void perror_exit(char *message);

int create_list(struct list_head* head);
int destroy_list(struct list_head* head);
int free_node(struct list_node* node);

int add_client(struct list_head* head,char* client_log_on_info);	// adds a client to list (calls add_node)
int add_node(struct list_head* head, char *ip, char *port);
int delete_node(struct list_head* head, char *ip, char *port);		// deletes certain client from list

int client_exists(struct list_head* head, char *ip, char *port);	// return 0 if exists, -1 if not
int print_list(struct list_head* head);

int request_type(char* buf);
int get_clients(struct list_head* head,char* client_log_on_info, int sock);
int log_off_client(struct list_head* head,char* client_log_on_info);

#endif