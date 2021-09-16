#include "users.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int users_hash_function(char* name, struct user_hash_table* ht){
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
	return value%( (int)pow(2,ht->hash_k) ) ;
}

int create_user_hash_table(struct user_hash_table* ht){
	ht->count = 0;
	ht->hash_k = USER_HT_KEY;

	int num_of_lists = pow(2,ht->hash_k);
	ht->lists = (struct user_list_head*)malloc(num_of_lists*sizeof(struct user_list_head));  /* - 1 - */

	for (int i = 0; i < num_of_lists; ++i){
		ht->lists[i].count = 0;
		ht->lists[i].first = NULL;
	}
}

int insert_user_ht(char* str, struct bitCoin_list_head* bCoins, struct user_hash_table* ht, int value){
	int hash_value = users_hash_function(str, ht);

	struct user_list_node* temp;
	temp = (struct user_list_node*)malloc(sizeof(struct user_list_node)); /* - 2 - */
	temp->next = NULL;
	strcpy(temp->user.WalletID, str);
	temp->user.bitCoins = bCoins;
	temp->user.num_of_coins = bCoins->count;
	temp->user.current_amount = bCoins->count * value;

	temp->user.sended.num_of_transactions = 0;
	temp->user.sended.first = NULL;
	temp->user.sended.user = &(temp->user);

	temp->user.received.num_of_transactions = 0;
	temp->user.received.first = NULL;
	temp->user.received.user = &(temp->user);

	if(ht->lists[hash_value].first == NULL){
		ht->lists[hash_value].first = temp;
	}
	else{
		struct user_list_node* temp1 = ht->lists[hash_value].first;
		while(temp1->next != NULL){
			temp1 = temp1->next;
		}
		temp1->next = temp;
	}

	ht->count++;
	ht->lists[hash_value].count++;
}

struct users* exists_user_hash_table(struct user_hash_table* ht, char* name){
	int hash_value = users_hash_function(name, ht);

	struct user_list_node* temp = ht->lists[hash_value].first;
	while(temp != NULL){
		if(strcmp(temp->user.WalletID, name) == 0){
			return &(temp->user);
		}

		temp = temp->next;
	}

	return NULL;
}

int free_user_hash_table(struct user_hash_table* ht){
	for (int i = 0; i < pow(2,ht->hash_k); ++i){
		if(ht->lists[i].count != 0){
			free_user_list(ht->lists[i].first);
		}
	}
	free(ht->lists); /* - 1 - */
}

int free_user_list(struct user_list_node* node){
	if(node->next!= NULL){
		free_user_list(node->next);
	}

	if(node->user.num_of_coins != 0){
		free_coin_list_1(node->user.bitCoins->first);
		// free_coin_list(node->user.bitCoins->first);
	}

	free(node->user.bitCoins); /* - 4 - */

	if(node->user.sended.first != NULL){
		free_transactions_lists(node->user.sended.first);
	}
	if(node->user.received.first != NULL){
		free_transactions_lists(node->user.received.first);
	}
	free(node); /* - 2 - */
}

int print_users(struct user_hash_table* ht){
	printf("Users : %d\n",ht->count);
	struct user_list_node* temp;
	if(ht->count != 0){
		int num_of_lists = pow(2,ht->hash_k);
		for (int i = 0; i < num_of_lists; ++i){
			printf(" -> list %d users %d\n", i, ht->lists[i].count);
			temp = ht->lists[i].first;
			for (int j = 0; j < ht->lists[i].count; ++j){
				printf("\tName: %s | amount: %d\n", temp->user.WalletID, temp->user.current_amount);
				print_coin_list(temp->user.bitCoins);
				printf("\t\tsended ");
				print_transactions_list(&(temp->user.sended));
				printf("\t\treceived ");
				print_transactions_list(&(temp->user.received));

				temp = temp->next;
			}
		}
	}
}

int compare_date_time(struct date date1, struct time time1, struct date date2, struct time time2){
	if(date2.year < date1.year){
		return 1;
	}
	else if(date2.year > date1.year){
		return -1;
	}
	else{	// same year
		if(date2.month < date1.month){
			return 1;
		}
		else if(date2.month > date1.month){
			return -1;
		}
		else{	// same month
			if(date2.day < date1.day){
				return 1;
			}
			else if(date2.day > date1.day){
				return -1;
			}
			else{	// same day
				if(time2.hour < time1.hour){
					return 1;
				}
				else if(time2.hour > time1.hour){
					return -1;
				}
				else{	// same hour
					if(time2.min < time1.min){
						return 1;
					}
					else if(time2.min > time1.min){
						return -1;
					}
					else{	// same minute
						return 0;
					}
				}
			}
		}
	}
}