#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "transactions.h"
#include "users.h"

int transactions_hash_function(struct hash_table_head* ht, char* name){
	int value=0;
	char* temp;
	int count=0;
	temp = (char*)malloc(strlen(name)+1);
	strcpy(temp, name);
	while( temp[count] != '\0'){
		value = value + temp[count];
		count++;
	}
	free(temp);
	return value%( ht->hash_key ) ;
}

int init_transactions_ht(struct hash_table_head* head, int key, int bucket_size){
	head->count = 0;
	head->hash_key = key;
	head->num_of_buckets = key;
	head->bucket_size = bucket_size;

	head->buckets = (struct bucket**)malloc((head->num_of_buckets)*sizeof(struct bucket*));
	for (int i = 0; i < head->num_of_buckets; ++i){
		head->buckets[i] = NULL;
	}
}

int free_transactions_ht(struct hash_table_head* head){
	for (int i = 0; i < head->num_of_buckets; ++i){
		if(head->buckets[i]!=NULL){
			free_bucket(head->buckets[i]);
		}
	}
	free(head->buckets);
}

int free_bucket(struct bucket* temp_b){
	if(temp_b->next_bucket != NULL){
		free_bucket(temp_b->next_bucket);
	}
	free(temp_b->buffer);
	free(temp_b);
}

int insert_transactions_ht(void* ht, 
							struct hash_table_head* ht_sender, 
							struct hash_table_head* ht_receiver, 
							char* name_sender, 
							char* name_receiver, 
							int value, 
							char* tran_id, 
							struct date tmp_day, struct time tmp_time){
	struct users* ptr;

	int hash_value_sender = transactions_hash_function(ht_sender, name_sender);
	int hash_value_receiver = transactions_hash_function(ht_receiver, name_receiver);

	struct users* sender_pointer;
	sender_pointer = exists_user_hash_table(ht, name_sender);
	if(sender_pointer == NULL){	// check if sender exists
		printf("sender %s doesn t exist, transaction %s\n", name_sender, tran_id);
		return 0;
	}
	struct users* receiver_pointer;
	receiver_pointer = exists_user_hash_table(ht, name_receiver);
	if(receiver_pointer == NULL){	// receiver does't exist
		printf("receiver %s doesn t exist, transaction %s\n", name_receiver, tran_id);
		return 0;
	}

	if( strcmp(name_sender,name_receiver) == 0){
		printf("tran can t be completed, receiver sender same person, transaction %s\n",tran_id);
		return 0;
	}
	if(value > sender_pointer->current_amount){
		printf("tran can t be completed, not enough money, transactions %s\n",tran_id);
		return 0;
	}


	struct transactions_list_head* sender_list_head = user_exists_transactions_ht(name_sender, ht_sender);
	struct transactions_list_head* receiver_list_head = user_exists_transactions_ht(name_receiver, ht_receiver);
	if(sender_list_head==NULL){
		// printf("add sender ------------------- %s\n", name_sender);
		sender_list_head = add_user_transactions_ht(sender_pointer, ht_sender, 0);	// flag 0 for sended
	}
	if(receiver_list_head==NULL){
		// printf("add receiver ------------------- %s\n", name_receiver);
		receiver_list_head = add_user_transactions_ht(receiver_pointer, ht_receiver, 1);	// flag 1 for received
	}

	add_tansaction_nd(sender_list_head, receiver_list_head, value, tmp_day, tmp_time, tran_id);

	printf("transaction %s completed\n", tran_id);
}





struct transactions_list_head* user_exists_transactions_ht(char* name, struct hash_table_head* ht){
	int hash_value = transactions_hash_function(ht, name);

	if(ht->buckets[hash_value] == NULL){	//empty bucket, doesn t exist, return NULL
		return NULL;
	}
	else{
		struct bucket* temp_bucket = ht->buckets[hash_value];
		void* offset;
		char temp_name[MAX_WalletID_LENGTH];
		struct transactions_list_head* returned_list;
		while(temp_bucket != NULL){
			offset = temp_bucket->buffer;

			while( (temp_bucket->end_of_buffer - offset) >= (MAX_WalletID_LENGTH + sizeof(void*)) ){
				// strcpy(temp_name, (char*)offset);
memcpy(temp_name, (char*)offset, MAX_WalletID_LENGTH);
				if( strcmp(name, temp_name) == 0){
					offset = offset + MAX_WalletID_LENGTH;
					memcpy(&returned_list, offset, sizeof(void*));
					return returned_list;
				}

				offset = offset + MAX_WalletID_LENGTH + sizeof(void*);
			}

			temp_bucket = temp_bucket->next_bucket;
		}
	}
	return NULL;	//it does not exist
}






struct transactions_list_head* add_user_transactions_ht(void* user, struct hash_table_head* ht, int flag){
	struct users* temp_user = (struct users*)user;

	struct transactions_list_head* temp_list;
	if(flag==0){
		// printf("add sender %s\n", temp_user->WalletID);
		temp_list = &(temp_user->sended);
	}
	else if(flag==1){
		// printf("add receiver %s\n", temp_user->WalletID);
		temp_list = &(temp_user->received);
	}
	else{
		printf("Wrong flag\n");
		return NULL;
	}

	int hash_value = transactions_hash_function(ht, temp_user->WalletID);
	struct bucket* temp_bucket = ht->buckets[hash_value];
	if(temp_bucket == NULL){	//empty bucket
		// if(flag==0){
		// 	printf("first sender\n");
		// }
		// else{
		// 	printf("first receiver\n");
		// }

		temp_bucket = (struct bucket*)malloc(sizeof(struct bucket));
		temp_bucket->next_bucket = NULL;

		temp_bucket->buffer = (void*)malloc(ht->bucket_size);
		temp_bucket->offset = temp_bucket->buffer;
		temp_bucket->end_of_buffer = temp_bucket->offset + ht->bucket_size;
		temp_bucket->count = 1;

		memcpy(temp_bucket->offset, temp_user->WalletID, MAX_WalletID_LENGTH);
		temp_bucket->offset += MAX_WalletID_LENGTH;
		memcpy(temp_bucket->offset, &temp_list, sizeof(void*));
		temp_bucket->offset += sizeof(void*);

		ht->buckets[hash_value] = temp_bucket;
	}
	else{
		while(temp_bucket->next_bucket != NULL){	//if next bucket != NULL temp bucket is full
			temp_bucket = temp_bucket->next_bucket;
		}

		if( (temp_bucket->end_of_buffer - temp_bucket->offset) < MAX_WalletID_LENGTH + sizeof(void*)){	//not enough space in temp buffer
			// if(flag==0){
			// 	printf("add sender in new buffer\n");
			// }
			// else{
			// 	printf("addd receiver in new buffer\n");
			// }
			temp_bucket->next_bucket = (struct bucket*)malloc(sizeof(struct bucket));
			temp_bucket = temp_bucket->next_bucket;
			temp_bucket->next_bucket = NULL;

			temp_bucket->buffer = (void*)malloc(ht->bucket_size);
			temp_bucket->offset = temp_bucket->buffer;
			temp_bucket->end_of_buffer = temp_bucket->offset + ht->bucket_size;
			temp_bucket->count = 1;

			memcpy(temp_bucket->offset, temp_user->WalletID, MAX_WalletID_LENGTH);
			temp_bucket->offset += MAX_WalletID_LENGTH;
			memcpy(temp_bucket->offset, &temp_list, sizeof(void*));
			temp_bucket->offset += sizeof(void*);
		}
		else{
			// if(flag==0){
			// 	printf("add sender in last buffer\n");
			// }
			// else{
			// 	printf("add receiver in last buffer\n");
			// }
			memcpy(temp_bucket->offset, temp_user->WalletID, MAX_WalletID_LENGTH);
			temp_bucket->offset += MAX_WalletID_LENGTH;
			memcpy(temp_bucket->offset, &temp_list, sizeof(void*));
			temp_bucket->offset += sizeof(void*);
		}
	}

	ht->count++;
	return temp_list;
}














int insert_transactions_list(struct transactions_list_head* head, struct transactions_node* nd){
	struct transactions_node* tmp_nd = head->first;
	if(head->first == NULL){
		head->first = nd;
		head->num_of_transactions++;
		return 0;
	}

	while(tmp_nd->next != NULL){
		tmp_nd = tmp_nd->next;
	}
	tmp_nd->next = nd;
	head->num_of_transactions++;
}

int print_transactions_ht(struct hash_table_head* head){
	printf("key %d, #buckets %d, count %d\n", head->hash_key, head->num_of_buckets, head->count);
}

int init_transactions_list(struct transactions_list_head* list){
	list->num_of_transactions = 0;
	list->first = NULL;
}

int print_transactions_list(struct transactions_list_head* list){
	printf("%d transactions [", list->num_of_transactions);
	
	struct transactions_node* temp = list->first;

	while(temp != NULL){
		printf("%d ", temp->value);
		temp = temp->next;
	}
	printf("]\n");
}

int free_transactions_lists(struct transactions_node* node){
	if(node->next!=NULL){
		free_transactions_lists(node->next);
	}
	free(node->tree_nds);
	free(node);
	node = NULL;
}

int add_tansaction_nd(struct transactions_list_head* sended, struct transactions_list_head* received, int amount, struct date dt, struct time tm, char* tran_id){
	int num_of_coins = sended->user->bitCoins->count;
	struct bitCoin_list_node* temp_coin = sended->user->bitCoins->first;	// first coin
	int rest = amount;

	// we allocate memory now beacuase we want the memory pointers for the tree node
	struct transactions_node* sended_nd = (struct transactions_node*)malloc(sizeof(struct transactions_node));
	struct transactions_node* received_nd = (struct transactions_node*)malloc(sizeof(struct transactions_node));

	int count=0;
	while(rest > 0){
		count++;
		rest = add_transaction_tree_nds(temp_coin->root, rest, received->user->WalletID, sended_nd, received_nd);

		if(temp_coin->next!=NULL){
			temp_coin = temp_coin->next;
		}
	}

	strcpy(sended_nd->transactionID, tran_id);
	strcpy(received_nd->transactionID, tran_id);

	strcpy(sended_nd->WalletID, received->user->WalletID);
	strcpy(received_nd->WalletID, sended->user->WalletID);

	sended_nd->value = amount;
	received_nd->value = amount;

	sended_nd->dt.day = dt.day;
	sended_nd->dt.month = dt.month;
	sended_nd->dt.year = dt.year;
	sended_nd->tm.hour = tm.hour;
	sended_nd->tm.min = tm.min;

	received_nd->dt.day = dt.day;
	received_nd->dt.month = dt.month;
	received_nd->dt.year = dt.year;
	received_nd->tm.hour = tm.hour;
	received_nd->tm.min = tm.min;

	sended_nd->number_of_tree_nds = count;
	received_nd->number_of_tree_nds = count;

	sended_nd->next = NULL;
	received_nd->next = NULL;

	sended_nd->tree_nds = (struct tree_node**)malloc(count*sizeof(void*));
	received_nd->tree_nds = (struct tree_node**)malloc(count*sizeof(void*));


	temp_coin = sended->user->bitCoins->first;
	for (int i = 0; i < count; ++i){
		sended_nd->tree_nds[i] = temp_coin->root->right_child;
		received_nd->tree_nds[i] = temp_coin->root->left_child;

		insert_coin_list(received->user->bitCoins, temp_coin->bitCoinID, temp_coin->root->left_child);

		temp_coin->root = temp_coin->root->right_child;		//deixnei sto neo upoloipo tou sender gi auto to bitcoin

		if(temp_coin->next!=NULL){
			temp_coin = temp_coin->next;
		}
	}

	// delete coins with no value
	for (int i = 0; i < count; ++i){
		if(sended->user->bitCoins->first->root->sum == 0){
			list_delete_coin(sended->user->bitCoins, sended->user->bitCoins->first->bitCoinID);
		}
	
	}

	insert_transactions_list(sended, sended_nd);
	insert_transactions_list(received, received_nd);

	sended->user->current_amount = sended->user->current_amount - amount;
	received->user->current_amount = received->user->current_amount + amount;
}

int add_transaction_tree_nds(struct tree_node* nd, int rest, char* receiver, struct transactions_node* sended, struct transactions_node* received){
	int new_rest = 0;
	if(nd->sum < rest){
		new_rest = rest - nd->sum;

		nd->sended_tran_nd = sended;
		nd->received_tran_nd = received;

		nd->left_child = (struct tree_node*)malloc(sizeof(struct tree_node));
		nd->right_child = (struct tree_node*)malloc(sizeof(struct tree_node));

		nd->left_child->sum = nd->sum;
		strcpy(nd->left_child->user, receiver);
		nd->left_child->sended_tran_nd = NULL;
		nd->left_child->received_tran_nd = NULL;
		nd->left_child->left_child = NULL;
		nd->left_child->right_child = NULL;

		nd->right_child->sum = 0;
		strcpy(nd->right_child->user, nd->user);
		nd->right_child->sended_tran_nd = NULL;
		nd->right_child->received_tran_nd = NULL;
		nd->right_child->left_child = NULL;
		nd->right_child->right_child = NULL;
	}
	else{
		nd->sended_tran_nd = sended;
		nd->received_tran_nd = received;
		
		nd->left_child = (struct tree_node*)malloc(sizeof(struct tree_node));
		nd->right_child = (struct tree_node*)malloc(sizeof(struct tree_node));

		nd->left_child->sum = rest;
		strcpy(nd->left_child->user, receiver);
		nd->left_child->left_child = NULL;
		nd->left_child->right_child = NULL;

		nd->right_child->sum = nd->sum-rest;
		strcpy(nd->right_child->user, nd->user);
		nd->right_child->left_child = NULL;
		nd->right_child->right_child = NULL;
	}
	return new_rest;
}