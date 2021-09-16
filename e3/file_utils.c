#include "file_utils.h"

int create_file_list(char* dir, struct  file_list_head* head){	//anadromikh, otan vriskei subdir
	char *path;
	path = (char*)malloc(strlen(dir)+1);
	strcpy(path, dir);

	unsigned short length;
	struct dirent *ent;
	DIR *input_dir_ptr = opendir(dir);
	struct stat temp_file_stat;

	char *file = NULL;
	while ((ent = readdir (input_dir_ptr)) != NULL) {	//gia kathe kainourgio id kai subdir sto dir
		if (!strcmp (ent->d_name, "."))
			continue;
		if (!strcmp (ent->d_name, ".."))    
			continue;

		file = (char*)malloc(strlen(path)+strlen(ent->d_name)+2);
		sprintf(file, "%s/%s", path, ent->d_name);

		if (ent->d_type == DT_DIR){	// case of sub dir, check it and add its files
			create_file_list(file, head);
			free(file);
			file=NULL;
			continue;
		}

		stat(file, &temp_file_stat);
		length = strlen(file);
		int size = (int)(temp_file_stat.st_size);
		// printf("%s - %hu - %d\n", file, length, size);

		insert_file(file, size, head);

		free(file);
		file=NULL;
	}
	closedir(input_dir_ptr);
	free(path);
}

int insert_file(char* file_path, int size, struct  file_list_head* head){
	struct file_node* nd = head->first;
	char *c;
	c = file_path;
	while(*c != '/'){	//skip input dir
		c++;
	}
	c++;
	if( head->first == NULL){	// first file in list
		head->first = (struct file_node*)malloc(sizeof(struct file_node));
		head->first->size = size;
		head->first->file_path = (char*)malloc((int)strlen(c)+1);
		strcpy(head->first->file_path, c);
		head->first->length = (int)strlen(c);

		head->first->next = NULL;
		head->count = 1;
		return 0;
	}
	while(nd->next != NULL){
		nd = nd->next;
	}
	nd->next = (struct file_node*)malloc(sizeof(struct file_node));
	nd->next->size = size;
	nd->next->file_path = (char*)malloc((int)strlen(c)+1);
	strcpy(nd->next->file_path, c);
	nd->next->length = (int)strlen(c);

	nd->next->next = NULL;
	head->count++;
	return 0;

}

int print_file_list(struct  file_list_head* head){
	printf("[%d]\n", head->count);
	struct file_node* nd = head->first;
	while(nd!=NULL){
		printf("%s - %hu - %d\n", nd->file_path, nd->length, nd->size);

		nd = nd->next;
	}
}

int free_file_list(struct  file_list_head* head){
	if(head->first!=NULL){
		free_file_node(head->first);
	}
	// head->first = NULL;
	head->count = 0;
	return 0;
}

int free_file_node(struct  file_node* node){
	if(node->next!=NULL){
		free_file_node(node->next);
	}
	free(node->file_path);
	free(node);
	node = NULL;
	return 0;
}