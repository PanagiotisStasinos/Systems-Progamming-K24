#ifndef BITCOINS_H_
#define BITCOINS_H_

#include "users.h"
#include "transactions.h"

#define COIN_HT_KEY 1

struct tree_node{
	int sum;
	char user[MAX_WalletID_LENGTH];

	struct transactions_node* sended_tran_nd;
	struct transactions_node* received_tran_nd;

	struct tree_node* left_child;  // init value NULL, goes to receiver
	struct tree_node* right_child;  // init value NULL, goes to sender
};

struct bitCoin_list_node{
	int bitCoinID;
	struct tree_node* root;

	struct bitCoin_list_node* next;
};

struct bitCoin_list_head{
	int count;

	struct bitCoin_list_node* first;
};

struct bitCoin_hash_table{
	int count;
	int hash_k;

	struct bitCoin_list_head* lists;
};

int coin_hash_function(struct bitCoin_hash_table* ht, int ID);
int init_coin_ht(struct bitCoin_hash_table* ht);
int free_coin_ht(struct bitCoin_hash_table* ht);
struct tree_node* insert_coin_ht(struct bitCoin_hash_table* ht, int ID, char* user, int value);
int print_coin_ht(struct bitCoin_hash_table* ht);
struct bitCoin_list_node* exist_coin_ht(struct bitCoin_hash_table* ht, int ID);


int init_coin_list(struct bitCoin_list_head* list);
int free_coin_list(struct bitCoin_list_node* node);
int free_tree_node(struct tree_node* nd);
int free_coin_list_1(struct bitCoin_list_node* node);  // does not free root, it is be used by free user list
int insert_coin_list(struct bitCoin_list_head* list, int ID, struct tree_node* root);
int list_delete_coin(struct bitCoin_list_head* list, int ID);
struct bitCoin_list_node* exist_coin_list(struct bitCoin_list_head* list, int ID);
int print_coin_list(struct bitCoin_list_head* list);

// struct tree_node* create_tree_nodes(void* sender, void* receiver, int amount);
#endif