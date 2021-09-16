#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "bitCoins.h"

int init_coin_list(struct bitCoin_list_head* list){
	list->count = 0;
	list->first = NULL;
}

int insert_coin_list(struct bitCoin_list_head* list, int ID, struct tree_node* root){
	struct bitCoin_list_node* temp = (struct bitCoin_list_node*)malloc(sizeof(struct bitCoin_list_node)); /* - 3 - */
	temp->bitCoinID = ID;
	temp->next = NULL;
	temp->root = root;
	if(list->first==NULL){
		list->first = temp;
	}
	else{
		struct bitCoin_list_node* temp1;
		temp1 = list->first;
		while(temp1->next!=NULL){
			temp1 = temp1->next;
		}
		temp1->next = temp;
	}
	
	list->count++;
}

int free_coin_list(struct bitCoin_list_node* node){
	if(node->next!=NULL){
		free_coin_list(node->next);
	}
	if(node->root != NULL){
		free_tree_node(node->root);
		node->root = NULL;
	}
	free(node); /* - 3 - */
	node = NULL;
}

int free_tree_node(struct tree_node* nd){
	if(nd->left_child != NULL){
		free_tree_node(nd->left_child);
		nd->left_child = NULL;
	}
	if(nd->right_child != NULL){
		free_tree_node(nd->right_child);
		nd->right_child = NULL;
	}
	free(nd);
}

int print_coin_list(struct bitCoin_list_head* list){
	printf("\t\tcount %d\n\t\t", list->count);
	struct bitCoin_list_node* temp = list->first;
	while(temp!=NULL){
		printf("%d", temp->bitCoinID);
		if(temp->root!=NULL){
			printf("(%s[%d])", temp->root->user, temp->root->sum);
			if(temp->root->sended_tran_nd!=NULL && temp->root->received_tran_nd!=NULL){
				printf("<S%s,R%s>", temp->root->sended_tran_nd->transactionID, temp->root->sended_tran_nd->transactionID);
			}
		}
		printf(" ");
		temp = temp->next;
	}
	printf("\n");
}

int init_coin_ht(struct bitCoin_hash_table* ht){
	ht->count = 0;
	ht->hash_k = COIN_HT_KEY;

	int num_of_lists = pow(2,ht->hash_k);
	ht->lists = (struct bitCoin_list_head*)malloc(num_of_lists*sizeof(struct bitCoin_list_head));

	for (int i = 0; i < num_of_lists; ++i){
		ht->lists[i].count = 0;
		ht->lists[i].first = NULL;
	}
}

struct tree_node* insert_coin_ht(struct bitCoin_hash_table* ht , int ID, char* user, int value){
	int hash_value = coin_hash_function(ht, ID);

	struct bitCoin_list_node* temp;
	temp = (struct bitCoin_list_node*)malloc(sizeof(struct bitCoin_list_node));
	temp->bitCoinID = ID;
	temp->next = NULL;
	temp->root = (struct tree_node*)malloc(sizeof(struct tree_node));
	temp->root->sum = value;
	strcpy(temp->root->user, user);

	temp->root->sended_tran_nd = NULL;
	temp->root->received_tran_nd = NULL;

	temp->root->left_child = NULL;
	temp->root->right_child = NULL;

	if(ht->lists[hash_value].first == NULL){
		ht->lists[hash_value].first = temp;
	}
	else{
		struct bitCoin_list_node* temp1 = ht->lists[hash_value].first;
		while(temp1->next!=NULL){
			temp1 = temp1->next;
		}
		temp1->next = temp;
	}

	ht->lists[hash_value].count++;
	ht->count++;

	return temp->root;
}

int coin_hash_function(struct bitCoin_hash_table* ht, int ID){
	return ID%(int)pow(2,ht->hash_k);
}

int print_coin_ht(struct bitCoin_hash_table* ht){
	int num_of_lists = pow(2,ht->hash_k);
	printf("coin hash table\n");
	for (int i = 0; i < num_of_lists; ++i){
		if(ht->lists[i].count!=0){
			printf("list: %d\n", i);
			print_coin_list(&(ht->lists[i]));
		}
	}
}

int free_coin_ht(struct bitCoin_hash_table* ht){
	int num_of_lists = pow(2,ht->hash_k);
	for (int i = 0; i < num_of_lists; ++i){
		if(ht->lists[i].count!=0){
			free_coin_list(ht->lists[i].first);
			// free_coin_list_1(ht->lists[i].first);
		}
	}
	free(ht->lists);
}

int free_coin_list_1(struct bitCoin_list_node* node){
	if(node->next!=NULL){
		free_coin_list_1(node->next);
	}
	free(node); /* - 3 - */
	node = NULL;
}

// struct tree_node* create_tree_nodes(void* sender, void* receiver, int amount){
// 	int rest = amount;

// 	// struct bitCoin_list_node temp_coin = sender->
// }

int list_delete_coin(struct bitCoin_list_head* list, int ID){
	struct bitCoin_list_node* temp_coin = list->first;
	struct bitCoin_list_node* prev_coin;

	if(temp_coin != NULL){
		if(temp_coin->bitCoinID == ID){
			list->first = temp_coin->next;
			list->count--;

			free(temp_coin);	//nomizw den xreiazetai kapoia allh free
			return 0;
		}

		prev_coin = temp_coin;
		temp_coin = temp_coin->next;
		while(temp_coin != NULL){
			if(temp_coin->bitCoinID == ID){
				prev_coin->next= temp_coin->next;
				list->count--;

				free(temp_coin);
				return 0;
			}

			prev_coin = temp_coin;
			temp_coin = temp_coin->next;
		}
	}
	printf("den uparxei tetoio coinID\n");
	return -1;	//den uparxei
}

struct bitCoin_list_node* exist_coin_list(struct bitCoin_list_head* list, int ID){
	struct bitCoin_list_node* temp_coin = list->first;

	while(temp_coin != NULL){
		if(temp_coin->bitCoinID == ID){
			return temp_coin;
		}

		temp_coin = temp_coin->next;
	}

	return NULL;	// den vrisketai sthn lista
}

struct bitCoin_list_node* exist_coin_ht(struct bitCoin_hash_table* ht, int ID){
	int hash_value = coin_hash_function(ht, ID);

	return exist_coin_list(&(ht->lists[hash_value]), ID);
}