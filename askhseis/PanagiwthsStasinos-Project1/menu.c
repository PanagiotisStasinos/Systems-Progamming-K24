#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "transactions.h"
#include "users.h"
#include "menu.h"

int requestTransaction(void* user_ht, void* sended_ht, void* received_ht, char* walletID1, char* walletID2, int amount, char* tran_id, struct date dt, struct time tm){
	// printf("requestTransaction [ %s %s %s %d %d-%d-%d %d:%d ]\n", tran_id, walletID1, walletID2, amount, dt.day, dt.month, dt.year, tm.hour, tm.min);
	struct user_hash_table* usr_ht = (struct user_hash_table*)user_ht;
	struct hash_table_head* send_ht = (struct hash_table_head*)sended_ht;
	struct hash_table_head* receive_ht = (struct hash_table_head*)received_ht;

	if(amount <= 0){
		printf("Ivalid transfer amount\n");
		return 0;
	}

	insert_transactions_ht(usr_ht, send_ht, receive_ht, walletID1, walletID2, amount, tran_id, dt, tm);
}

// int requestTransactions(){
// 	printf("requestTransaction_2\n");
// }

int requestTransactions_file(char* file_name, void* usr_ht, void* send_ht, void* rec_ht, struct date* latest_day, struct time* latest_time, int* tran_ID){
	struct hash_table_head* sended_ht = (struct hash_table_head*)send_ht;
	struct hash_table_head* received_ht = (struct hash_table_head*)rec_ht;
	struct user_hash_table* users_ht = (struct user_hash_table*)usr_ht;

	// printf("requestTransactions_file\n");
	FILE *fpt=NULL;
	fpt = fopen(file_name,"r");

	if(fpt==NULL){
		printf("Cannot open transaction file\n");
		return 0;
	}

	char line[MAX_LINE_SIZE];
	const char s[2] = " ";
	char *token;
	const char p[2] = "-";
	const char r[2] = ":";
	const char q[2] = ";";

	char temp_tran_id[MAX_tranID_LENGTH];
	char temp_sender[MAX_WalletID_LENGTH];
	char temp_receiver[MAX_WalletID_LENGTH];
	int amount;

	struct date tmp_day;
	struct time tmp_time;

	while (fgets(line, MAX_LINE_SIZE, fpt) != NULL) {
		(*tran_ID)++;
		sprintf(temp_tran_id, "%d", *tran_ID);
		/* get sender's name */
		token = strtok(line, s); if(token==NULL){printf("Invalid line in transactions file 1\n"); continue;}
		strcpy(temp_sender,token);
		/* get receiver's name */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file 2\n"); continue;}
		strcpy(temp_receiver,token);
		/* get amount */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file 3\n"); continue;}
		amount = atoi(token);
		/* get day */
		token = strtok(NULL, p); if(token==NULL){printf("Invalid line in transactions file 4\n"); continue;}
		tmp_day.day = atoi(token);
		/* get month */
		token = strtok(NULL, p); if(token==NULL){printf("Invalid line in transactions file 5\n"); continue;}
		tmp_day.month = atoi(token);
		/* get year */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file 6\n"); continue;}
		tmp_day.year = atoi(token);
		/* get hour */
		token = strtok(NULL, r); if(token==NULL){printf("Invalid line in transactions file 7\n"); continue;}
		tmp_time.hour = atoi(token);
		/* get minute */
		token = strtok(NULL, q); if(token==NULL){printf("Invalid line in transactions file 8\n"); continue;}
		tmp_time.min = atoi(token);

		if(compare_date_time(tmp_day, tmp_time, *latest_day, *latest_time)==1){
			insert_transactions_ht(users_ht, sended_ht, received_ht, temp_sender, temp_receiver, amount, temp_tran_id, tmp_day, tmp_time);
			latest_day->day = tmp_day.day;
			latest_day->month = tmp_day.month;
			latest_day->year = tmp_day.year;
			latest_time->hour = tmp_time.hour;
			latest_time->min = tmp_time.min;
		}
		else{
			printf("Invalid date\n");
		}
	}

	fclose(fpt);
}

int findEarnings(void* ht, char* walletID, struct time* tm1, struct date* dt1, struct time* tm2, struct date* dt2){
	if(tm1!=NULL){
		if( compare_date_time(*dt1, *tm1, *dt2, *tm2) > 0 ){
			printf("Invalid dates given\n");
			return 0;
		}
	}

	struct hash_table_head* received_ht = (struct hash_table_head*)ht;
	struct transactions_list_head* list = user_exists_transactions_ht(walletID, received_ht);
	if(list == NULL){
		printf("No receiver with name %s\n", walletID);
		return 0;
	}

	int sum = 0;
	struct transactions_node* temp_node = list->first;
	while(temp_node != NULL){
		if(tm1!=NULL){
			if(compare_date_time(*dt2, *tm2, temp_node->dt, temp_node->tm)==1 && compare_date_time(temp_node->dt, temp_node->tm, *dt1, *tm1)==1){
				printf("%s %s %s %d %d-%d-%d %d:%d\n", temp_node->transactionID, temp_node->WalletID, walletID, temp_node->value, temp_node->dt.day, temp_node->dt.month, temp_node->dt.year, temp_node->tm.hour, temp_node->tm.min);
				sum += temp_node->value;
			}
		}
		else{
			printf("%s %s %s %d %d-%d-%d %d:%d\n", temp_node->transactionID, temp_node->WalletID, walletID, temp_node->value, temp_node->dt.day, temp_node->dt.month, temp_node->dt.year, temp_node->tm.hour, temp_node->tm.min);
			sum += temp_node->value;
		}
		temp_node = temp_node->next;
	}
	printf("Total Sum %d\n", sum);
	return 0;
}

int findPayments(void* ht, char* walletID, struct time* tm1, struct date* dt1, struct time* tm2, struct date* dt2){
	if(tm1!=NULL){
		if( compare_date_time(*dt1, *tm1, *dt2, *tm2) > 0 ){
			printf("Invalid dates given\n");
			return 0;
		}
	}

	struct hash_table_head* sended_ht = (struct hash_table_head*)ht;
	struct transactions_list_head* list = user_exists_transactions_ht(walletID, sended_ht);
	if(list == NULL){
		printf("No sender with name %s\n", walletID);
		return 0;
	}

	int sum = 0;
	struct transactions_node* temp_node = list->first;
	while(temp_node != NULL){
		if(tm1!=NULL){
			if(compare_date_time(*dt2, *tm2, temp_node->dt, temp_node->tm)==1 && compare_date_time(temp_node->dt, temp_node->tm, *dt1, *tm1)==1){
				printf("%s %s %s %d %d-%d-%d %d:%d\n", temp_node->transactionID, walletID, temp_node->WalletID, temp_node->value, temp_node->dt.day, temp_node->dt.month, temp_node->dt.year, temp_node->tm.hour, temp_node->tm.min);
				sum += temp_node->value;
			}
		}
		else{
			printf("%s %s %s %d %d-%d-%d %d:%d\n", temp_node->transactionID, walletID, temp_node->WalletID, temp_node->value, temp_node->dt.day, temp_node->dt.month, temp_node->dt.year, temp_node->tm.hour, temp_node->tm.min);
			sum += temp_node->value;
		}


		temp_node = temp_node->next;
	}
	printf("Total Sum %d\n", sum);
	return 0;
}

int walletStatus(void* ht, char* name){
	struct user_hash_table* user_ht = (struct user_hash_table*)ht;
	struct users* usr = exists_user_hash_table(user_ht,name);

	if(usr != NULL){
		printf("%d\n", usr->current_amount);
	}
	else{
		printf("there is no walletID %s\n", name);
	}
}

int bitCoinStatus(void* ht, char* coin){
	struct bitCoin_hash_table* coins_ht = (struct bitCoin_hash_table*)ht;

	int coinID = atoi(coin);
	int hash_value = coin_hash_function(coins_ht, coinID);
	struct bitCoin_list_node* nd = exist_coin_list(&(coins_ht->lists[hash_value]), coinID);
	if(nd==NULL){
		printf("Coin %s does not exist\n", coin);
	}
	else{
		int num = num_of_transactions(nd->root)/2;
		int unspent=0;
		struct tree_node* temp = nd->root;
		while(temp->right_child!=NULL){
			temp = temp->right_child;
		}
		unspent = temp->sum;
		printf("%d %d %d\n", nd->root->sum, num, unspent);
	}
}

int traceCoin(void* ht, char* coin){
	struct bitCoin_hash_table* coins_ht = (struct bitCoin_hash_table*)ht;

	int coinID = atoi(coin);
	int hash_value = coin_hash_function(coins_ht, coinID);
	struct bitCoin_list_node* nd = exist_coin_list(&(coins_ht->lists[hash_value]), coinID);
	if(nd==NULL){
		printf("Coin %s does not exist\n", coin);
	}
	else{
		print_coins_trans(nd->root);
	}	
}

int e(){
	printf("exit\n");
}

int num_of_transactions(struct tree_node* nd){
	int sum1 =0;
	int sum2 = 0;
	if(nd->left_child!=NULL){
		sum1 = num_of_transactions(nd->left_child);
	}
	if(nd->right_child!=NULL){
		sum2 = num_of_transactions(nd->right_child);
	}
	return sum1+sum2+1;
}

int print_coins_trans(struct tree_node* nd){
	if(nd->sended_tran_nd!=NULL && nd->received_tran_nd!=NULL){
		char* tran_id = nd->sended_tran_nd->transactionID;
		char* sender = nd->received_tran_nd->WalletID;
		char* receiver = nd->sended_tran_nd->WalletID;
		int value = nd->sended_tran_nd->value;
		struct date dt;
		dt.day = nd->sended_tran_nd->dt.day;
		dt.month = nd->sended_tran_nd->dt.month;
		dt.year = nd->sended_tran_nd->dt.year;
		struct time tm;
		tm.hour = nd->sended_tran_nd->tm.hour;
		tm.min = nd->sended_tran_nd->tm.min;
		printf("%s %s %s %d %d-%d-%d %d:%d\n", tran_id, sender, receiver, value, dt.day, dt.month, dt.year, tm.hour, tm.min);
	}
	if(nd->left_child!=NULL){
		print_coins_trans(nd->left_child);
	}
	if(nd->right_child!=NULL){
		print_coins_trans(nd->right_child);
	}
}