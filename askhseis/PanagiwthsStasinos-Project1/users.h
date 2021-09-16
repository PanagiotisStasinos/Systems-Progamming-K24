#ifndef USERS_H_
#define USERS_H_

#include "transactions.h"
#include "bitCoins.h"

#define USER_HT_KEY 1

struct users{
	char WalletID[MAX_WalletID_LENGTH];

	int current_amount;

	int num_of_coins;
	struct bitCoin_list_head* bitCoins;

	struct transactions_list_head sended;
	struct transactions_list_head received;
};

struct user_list_node{
	struct users user;
	struct user_list_node* next;
};

struct user_list_head{
	int count;
	struct user_list_node* first;
};

struct user_hash_table{
	int count;
	int hash_k;

	struct user_list_head* lists;
};

int users_hash_function(char* name, struct user_hash_table* ht);
int create_user_hash_table(struct user_hash_table* ht);
struct users* exists_user_hash_table(struct user_hash_table* ht, char* name);
int insert_user_ht(char* name, struct bitCoin_list_head* bCoins, struct user_hash_table* ht, int value);
int free_user_hash_table(struct user_hash_table* ht);
int free_user_list(struct user_list_node* node);

int print_users(struct user_hash_table* ht);

#endif