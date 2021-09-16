#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#define MAX_WalletID_LENGTH 50
#define MAX_tranID_LENGTH 20

struct date{
	int day;
	int month;
	int year;
};

struct time{
	int hour;
	int min;
};

struct transactions_node{
	char transactionID[MAX_tranID_LENGTH];
	char WalletID[MAX_WalletID_LENGTH];		// sender or receiver, depends on type of list(receiving or sendind)
	int value;
	struct date dt;
	struct time tm;

	int number_of_tree_nds;
	struct tree_node** tree_nds;

	struct transactions_node* next;
};

struct transactions_list_head{
	struct users* user;
	int num_of_transactions;
	struct transactions_node* first;
};

struct bucket{
	void* buffer;
	void* offset;
	void* end_of_buffer;
	int count;

	struct bucket* next_bucket;
};

struct hash_table_head{
	int count;
	int hash_key;
	int num_of_buckets;
	int bucket_size;
	struct bucket** buckets;
};

int transactions_hash_function(struct hash_table_head* ht, char* name); 
int init_transactions_ht(struct hash_table_head* head, int key, int bucket_size);
int print_transactions_ht(struct hash_table_head* head);
int free_transactions_ht(struct hash_table_head* head);
int free_bucket(struct bucket* temp_b);
int insert_transactions_ht(void* ht, struct hash_table_head* head_sended, struct hash_table_head* head_received, char* name1, char* name2, 
	int value, char* tran_id, struct date tmp_day, struct time tmp_time);
struct transactions_list_head* user_exists_transactions_ht(char* name, struct hash_table_head* ht);		// return NULL if not
struct transactions_list_head* add_user_transactions_ht(void* user, struct hash_table_head* ht, int flag);	//flag arqument 0 sended, 1 received


int insert_transactions_list(struct transactions_list_head* head, struct transactions_node* nd);
int init_transactions_list(struct transactions_list_head* list);
int print_transactions_list(struct transactions_list_head* list);
int free_transactions_lists(struct transactions_node* node);

int compare_date_time(struct date date1, struct time time1, struct date date2, struct time time2); // 1 if first subsequent, 0 if equal, -1 if second subsequent

// prosthetei ena komvo sthn lista sended kai ena sthn received
int add_tansaction_nd(struct transactions_list_head* sended, struct transactions_list_head* received, int amount, struct date dt, struct time tm, char* tran_id);

//dhmiourgei kai arxikopoiei tous 2 neous tree nodes
int add_transaction_tree_nds(struct tree_node* nd, int rest, char* receiver, struct transactions_node* sended, struct transactions_node* received);
#endif