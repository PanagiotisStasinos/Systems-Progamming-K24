#include "utils.h"

int is_idfile(char* f_name){
	if(f_name[strlen(f_name)-1] == 'd'){
		if(f_name[strlen(f_name)-2] == 'i'){
			if(f_name[strlen(f_name)-3] == '.'){
				return 0;
			}
		}
	}
	return -1;
}


int get_id(char* f_name){	//returns id
	char *temp_name;
	temp_name = (char*)malloc(strlen(f_name)+1);
	strcpy(temp_name,f_name);
	const char s[2] = ".";
	char *other_client_id_str;
	other_client_id_str = strtok(temp_name, s);
	int other_client_id = atoi(other_client_id_str);
	free(temp_name);
	return other_client_id;
}




//list functions
int init_list(struct list_head* head){
	head->count = 0;
	head->first = NULL;

	return 0;
}

int get_len(struct list_head* head){
	return head->count;
}

int push(struct list_head* head, int id){	//return -1 if allready exists
	struct list_node* temp = head->first;
	if(head->first == NULL){
		head->first = (struct list_node*)malloc(sizeof(struct list_node));
		head->first->id = id;
		head->first->next = NULL;
		head->count = 1;

		return 0;
	}
	while(1){
		if(temp->id == id){
			return -1;
		}
		if(temp->next == NULL){break;}
		temp = temp->next;
	}
	temp->next = (struct list_node*)malloc(sizeof(struct list_node));
	temp->next->id = id;
	temp->next->next = NULL;

	head->count++;
	return 0;
}

int pull(struct list_head* head, int id){	//return 0 if it finds it, -1 elsewhere
	struct list_node* temp = head->first;
	struct list_node* temp_1;
	if(temp == NULL){
		return -1;
	}
	else if(temp->id == id){
		head->first = head->first->next;
		head->count--;

		free(temp);
	}
	while(temp->next!=NULL){
		if(temp->next->id == id){
			temp_1 = temp->next;
			temp->next = temp->next->next;
			free(temp_1);
			head->count--;

			return 0;
		}
		temp = temp->next;
	}
	return -1;
}

int free_list(struct list_head* head){
	if(head->first!=NULL){
		free_node(head->first);
	}
	head->first = NULL;
	head->count = 0;
	return 0;
}

int free_node(struct list_node* node){
	if(node->next!=NULL){
		free_node(node->next);
	}
	node->next = NULL;
	free(node);
	return 0;
}

int print(struct list_head* head){
	struct list_node* temp = head->first;
	printf("(%d) : ",head->count);
	while(temp!=NULL){
		printf("%d ", temp->id);
		temp = temp->next;
	}
	printf("\n");
}

int exists(struct list_head* head, int id){	//returns 0 if exists, -1 elsewhere
	struct list_node* temp = head->first;
	while(temp!=NULL){
		if(temp->id==id){
			return 0;
		}
		temp = temp->next;
	}
	return -1;
}