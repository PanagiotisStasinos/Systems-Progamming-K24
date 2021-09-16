#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "transactions.h"
#include "menu.h"
#include "bitCoins.h"
#include "users.h"

int main (int argc, char **argv){
	///////////////////////
	/*     Arguments     */
	///////////////////////
	printf("\t-----\t      ARGUMENTS     \t-----\n");
	char balance[3] = "-a\0";
	char trasaction[3] = "-t\0";
	char value[3] = "-v\0";
	char sender[4] = "-h1\0";
	char receiver[4] = "-h2\0";
	char bucket[3] = "-b\0";

	int bitcoin_value;
	int h1;
	int h2;
	int bucket_size;

	char* balance_file = NULL;   	// .txt given
	char* transaction_file = NULL;  // .txt e3odou

	if( argc == 13 ){
		for(int i=1; i<=11; i+=2){
			if( strcmp(argv[i],balance)==0 ){
				balance_file = argv[i+1];
			}
			else if( strcmp(argv[i],trasaction)==0 ){
				transaction_file = argv[i+1];
			}
			else if(strcmp(argv[i],value)==0 ){
				bitcoin_value = atoi(argv[i+1]);
			}
			else if(strcmp(argv[i],sender)==0 ){
				h1 = atoi(argv[i+1]);
			}
			else if(strcmp(argv[i],receiver)==0 ){
				h2 = atoi(argv[i+1]);
			}
			else if(strcmp(argv[i],bucket)==0 ){
				bucket_size = atoi(argv[i+1]);
				if(bucket_size < MAX_WalletID_LENGTH + sizeof(void*)){
					printf("bucket size must be equal or greater than 58 bytes\n");
					bucket_size = MAX_WalletID_LENGTH + sizeof(void*);
				}
			}
			else{
				printf("Error in arguments given!\nProgram Terminated\n");
				return(0);  //terminating program
			}
		}
	}
	else{
		printf("Error in arguments given!\nProgram Terminated\n");
		return(0);  //terminating program
	}

	printf(" balance_file \t\t%s\n transaction_file \t%s\n bitcoin_value \t\t%d\n sender \t\t%d\n receiver \t\t%d\n bucket_size \t\t%d\n\n",balance_file,
		transaction_file,bitcoin_value ,h1,h2,bucket_size);
	printf("---------------------------------------------\n\n");	
	////////////////////////////////////////////////////////
	//    READ BALANCES FILE AND INITIALIZE HASH TABLE    //
	////////////////////////////////////////////////////////
	printf("\t-----\tREAD BALANCES FILE\t-----\n");
	FILE *fpb=NULL;
	fpb = fopen(balance_file,"r");
	if(fpb==NULL){
		printf("Cannot open balances file\n");
		return 0;
	}

	char line[MAX_LINE_SIZE];
	const char s[2] = " ";
	char *token;

	int temp_bitCoinID;
	char temp_owner[MAX_WalletID_LENGTH];
	
	struct user_hash_table users_ht;
	create_user_hash_table(&users_ht);

	struct bitCoin_hash_table coin_ht;
	init_coin_ht(&coin_ht);
	
	struct bitCoin_list_head *temp_bCoins_list = NULL;
	struct tree_node* temp_root = NULL;

	while (fgets(line, MAX_LINE_SIZE, fpb) != NULL) {
		temp_bCoins_list = (struct bitCoin_list_head*)malloc(sizeof(struct bitCoin_list_head));  /* - 4 - */
		init_coin_list(temp_bCoins_list);

		/* get owner's name first */
		token = strtok(line, s); if(token==NULL){printf("Invalid line in Balances file\n"); free(temp_bCoins_list); temp_bCoins_list = NULL; continue;}
		strcpy(temp_owner,token);

		if(exists_user_hash_table(&users_ht,temp_owner)!=NULL){		//checks if user exists
			printf("%s allready exists\n", temp_owner);
			free(temp_bCoins_list);
			temp_bCoins_list = NULL;
			continue;
		}

		/* get bitCoins ID of that owner */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in Balances file\n"); free(temp_bCoins_list); temp_bCoins_list = NULL; continue;}
		while( token != NULL ) {
			temp_bitCoinID = atoi(token);

			if( exist_coin_ht(&coin_ht, temp_bitCoinID) != NULL ){	//checks if coin exists
				printf("%d allready exists\n",temp_bitCoinID);
				token = strtok(NULL, s);
				continue;
			}
			temp_root = insert_coin_ht(&coin_ht, temp_bitCoinID, temp_owner, bitcoin_value);

			insert_coin_list(temp_bCoins_list, temp_bitCoinID, temp_root);
			token = strtok(NULL, s); 
			temp_root = NULL;
		}
		insert_user_ht(temp_owner, temp_bCoins_list, &users_ht, bitcoin_value);

		temp_bCoins_list = NULL;
	}
// 	print_users(&users_ht);
// printf("---------------------------------------------\n\n");
// 	print_coin_ht(&coin_ht);
	fclose(fpb);
	printf("---------------------------------------------\n\n");
	//////////////////////////////////////////////////////////////////
	//	CREATE TRANSACTIONS HASH TABLES	& READ TRANSACTION FILE     //
	//////////////////////////////////////////////////////////////////
	struct hash_table_head sended_ht;	init_transactions_ht(&sended_ht, h1, bucket_size);
	struct hash_table_head received_ht;	init_transactions_ht(&received_ht, h2, bucket_size);


	//////////////////////////////////////////////
	//		CREATE TRANSACTIONS HASH TABLES		//
	//////////////////////////////////////////////
	printf("\t-----\tREAD TRANSACTIONS FILE\t-----\n");
	FILE *fpt=NULL;
	fpt = fopen(transaction_file,"r");

	if(fpt==NULL){
		printf("Cannot open transaction file\n");
		free_user_hash_table(&users_ht);
		free_coin_ht(&coin_ht);
		free_transactions_ht(&sended_ht);
		free_transactions_ht(&received_ht);
		return 0;
	}

	const char p[2] = "-";
	const char r[2] = ":";
	const char q[2] = ";";

	char temp_tran_id[MAX_tranID_LENGTH];
	char temp_sender[MAX_WalletID_LENGTH];
	char temp_receiver[MAX_WalletID_LENGTH];
	int amount;

struct date latest_day;
	latest_day.day = 0;
	latest_day.month = 0;
	latest_day.year = 0;
struct time latest_time;
	latest_time.hour = 0;
	latest_time.min = 0;

	struct date tmp_day;
	struct time tmp_time;

	while (fgets(line, MAX_LINE_SIZE, fpt) != NULL) {
		/* get transactions's id */
		token = strtok(line, s); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		strcpy(temp_tran_id,token);
		/* get sender's name */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		strcpy(temp_sender,token);
		/* get receiver's name */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		strcpy(temp_receiver,token);
		/* get amount */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		amount = atoi(token);
		/* get day */
		token = strtok(NULL, p); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		tmp_day.day = atoi(token);
		/* get month */
		token = strtok(NULL, p); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		tmp_day.month = atoi(token);
		/* get year */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		tmp_day.year = atoi(token);
		/* get hour */
		token = strtok(NULL, r); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		tmp_time.hour = atoi(token);
		/* get minute */
		token = strtok(NULL, s); if(token==NULL){printf("Invalid line in transactions file\n"); continue;}
		tmp_time.min = atoi(token);

		if(compare_date_time(tmp_day, tmp_time, latest_day, latest_time)==1){
			insert_transactions_ht(&users_ht, &sended_ht, &received_ht, temp_sender, temp_receiver, amount, temp_tran_id, tmp_day, tmp_time);
			latest_day.day = tmp_day.day;
			latest_day.month = tmp_day.month;
			latest_day.year = tmp_day.year;
			latest_time.hour = tmp_time.hour;
			latest_time.min = tmp_time.min;
		}
		else{
			printf("Invalid date\n");
		}
	}

	fclose(fpt);
// printf("---------------------------------------------\n\n");
// 	print_users(&users_ht);
// 	// print_transactions_ht(&sended_ht);
// 	// print_transactions_ht(&received_ht);
// printf("---------------------------------------------\n\n");
	
	////////////////
	//    MENU    //
	////////////////
    char *buffer;
   	size_t bufsize = 16;
   	size_t characters;

   	buffer = (char *)malloc(bufsize * sizeof(char));

	char *entolh;

	char *walletID_1;	int ID_1;
	char *walletID_2;	int ID_2;
	char *amount_str;	// int amount;
	char *day_str;	char* month_str;	char *year_str;		struct date dt1;	struct date dt2;
	char *hour_str;	char *min_str;							struct time tm1;	struct time tm2;
	char* more_arguments_flag;
	char* input_file;
	char* bitCoinID;

	int tran_ID = atoi(temp_tran_id);

	char *temp_buffer;  //krataei autousia thn entolh

	char* search_buffer[10];

while(1){
	printf(">> Type something: ");
	characters = getline(&buffer,&bufsize,stdin);
/// gia na dexetai kai to sketo enter
	if( characters == 1 ){
		buffer[characters] = '\0';
	}
	else{
		buffer[characters-1] = '\0';
	}

	temp_buffer = (char *)malloc((strlen(buffer)+1) * sizeof(char));
	strcpy(temp_buffer, buffer);

	entolh = strtok( buffer , s );
//  - 1 - requestTransaction senderWalletID receiverWalletID amount date time
	if( strcmp( entolh , "requestTransaction\0" ) == 0 ){
		walletID_1 = strtok( NULL , s );
		if( walletID_1 == NULL ){ printf("Less argument given\n"); }
		else{
			walletID_2 = strtok( NULL, s );
			if( walletID_2 == NULL ){ printf("Less argument given\n"); }
			else{
				amount_str = strtok( NULL, s );
				if( amount_str == NULL ){ printf("Less argument given\n"); }
				else{
					amount = atoi(amount_str);
					day_str = strtok( NULL, p );
					if( day_str == NULL ){ printf("Less argument given\n"); }
					else{
						dt1.day = atoi(day_str);
						month_str = strtok( NULL, p );
						if( month_str == NULL ){ printf("Less argument given\n"); }
						else{
							dt1.month = atoi(month_str);
							year_str = strtok(NULL,s);
							if( year_str == NULL ){ printf("Less argument given\n"); }
							else{
								dt1.year = atoi(year_str);
								hour_str = strtok(NULL,r);
								if( hour_str == NULL ){ printf("Less argument given\n"); }
								else{
									tm1.hour = atoi(hour_str);
									min_str = strtok(NULL,s);
									if( min_str == NULL ){ printf("Less argument given\n"); }
									else{
										tm1.min = atoi(min_str);
										more_arguments_flag = strtok( NULL, s );
										if( more_arguments_flag == NULL ){
											tran_ID++;
											sprintf(temp_tran_id, "%d", tran_ID);

											if(compare_date_time(dt1, tm1, latest_day, latest_time)==1){
												requestTransaction(&users_ht, &sended_ht, &received_ht, walletID_1, walletID_2, amount, temp_tran_id, dt1, tm1);
												// insert_transactions_ht(&users_ht, &sended_ht, &received_ht, temp_sender, temp_receiver, amount, temp_tran_id, tmp_day, tmp_time);
												latest_day.day = dt1.day;
												latest_day.month = dt1.month;
												latest_day.year = dt1.year;
												latest_time.hour = tm1.hour;
												latest_time.min = tm1.min;
											}
											else{
												printf("Invalid date\n");
											}
										}
										else{ printf("More arguments given\n"); }
									}
								}
							}
						}
					}
				}
			}
		}
	}
//  - 2 - requestTransactions  senderWalletID receiverWalletID amount date time;
//                     senderWalletID2 receiverWalletID2 amount2 date2 time2;
//                       ...
//                     senderWalletIDn receiverWalletIDn amountn daten timen;
//  - 3 - requestTransactions inputFile
	else if( strcmp( entolh , "requestTransactions\0" ) == 0 ){
		int i=0;
		while(1){
			if(i==0){
				walletID_1 = strtok( NULL , s );
			}
			else{
				walletID_1 = more_arguments_flag;
			}
			if( walletID_1 == NULL ){ printf("Less argument given\n"); }
			else{
				walletID_2 = strtok( NULL, s );
				if( walletID_2 == NULL ){ 
					if(i==0){
						char* file_name = walletID_1;
						requestTransactions_file(file_name, &users_ht, &sended_ht, &received_ht, &latest_day, &latest_time, &tran_ID);
						break;
					}
					else{
						printf("Less argument given\n");
						break;
					}
				}
				else{
					amount_str = strtok( NULL, s );
					if( amount_str == NULL ){ printf("Less argument given\n"); break;}
					else{
						amount = atoi(amount_str);
						day_str = strtok( NULL, p );
						if( day_str == NULL ){ printf("Less argument given\n"); break;}
						else{
							dt1.day = atoi(day_str);
							month_str = strtok( NULL, p );
							if( month_str == NULL ){ printf("Less argument given\n"); break;}
							else{
								dt1.month = atoi(month_str);
								year_str = strtok(NULL,s);
								if( year_str == NULL ){ printf("Less argument given\n"); break;}
								else{
									dt1.year = atoi(year_str);
									hour_str = strtok(NULL,r);
									if( hour_str == NULL ){ printf("Less argument given\n"); break;}
									else{
										tm1.hour = atoi(hour_str);
										min_str = strtok(NULL,q);
										if( min_str == NULL ){ printf("Less argument given\n"); break;}
										else{
											tran_ID++;
											sprintf(temp_tran_id, "%d", tran_ID);

											if(compare_date_time(dt1, tm1, latest_day, latest_time)==1){
												requestTransaction(&users_ht, &sended_ht, &received_ht, walletID_1, walletID_2, amount, temp_tran_id, dt1, tm1);
												latest_day.day = dt1.day;
												latest_day.month = dt1.month;
												latest_day.year = dt1.year;
												latest_time.hour = tm1.hour;
												latest_time.min = tm1.min;
											}
											else{ printf("Invalid date\n"); }

											more_arguments_flag = strtok( NULL, s );
											if(more_arguments_flag == NULL){ break; }
											i++;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
//  - 4 - findEarnings walletID [time1][year1][time2][year2]
	else if( strcmp( entolh , "findEarnings\0" ) == 0 ){
		walletID_1 = strtok( NULL , s );
		if( walletID_1 == NULL ){ printf("Less argument given\n"); }
		else{
			hour_str = strtok(NULL,r);
			if(hour_str == NULL){		//in case no time and year arguments given
				findEarnings(&received_ht, walletID_1, NULL, NULL, NULL, NULL);
			}
			else{
				tm1.hour = atoi(hour_str);
				min_str = strtok(NULL,s);
				if( min_str == NULL){ printf("Less argument given\n"); }
				else{
					tm1.min = atoi(min_str);
					day_str = strtok(NULL,p);
					if( day_str == NULL ){ printf("Less argument given\n"); }
					else{
						dt1.day = atoi(day_str);
						month_str = strtok(NULL,p);
						if( month_str == NULL ){ printf("Less argument given\n"); }
						else{
							dt1.month = atoi(month_str);
							year_str  = strtok(NULL,s);
							if( year_str == NULL ){ printf("Less argument given\n"); }
							else{
								dt1.year = atoi(year_str);
								hour_str = strtok(NULL,r);
								if(hour_str == NULL){ printf("Less argument given\n"); }
								else{
									tm2.hour = atoi(hour_str);
									min_str = strtok(NULL,s);
									if( min_str == NULL){ printf("Less argument given\n"); }
									else{
										tm2.min = atoi(min_str);
										day_str = strtok(NULL,p);
										if( day_str == NULL ){ printf("Less argument given\n"); }
										else{
											dt2.day = atoi(day_str);
											month_str = strtok(NULL,p);
											if( month_str == NULL ){ printf("Less argument given\n"); }
											else{
												dt2.month = atoi(month_str);
												year_str  = strtok(NULL,s);
												if( year_str == NULL ){ printf("Less argument given\n"); }
												else{
													dt2.year = atoi(year_str);
													more_arguments_flag = strtok( NULL, s );
													if( more_arguments_flag == NULL ){
														findEarnings(&received_ht, walletID_1, &tm1, &dt1, &tm2, &dt2);
													}
													else{ printf("More arguments given\n"); }
												}
											}
										}
									}
								}
							}
						}
					}	
				}
			}
		}
	}
// //  - 5 - findPayments walletID [time1][year1][time2][year2]
	else if( strcmp( entolh , "findPayments\0" ) == 0 ){
		walletID_1 = strtok( NULL , s );
		if( walletID_1 == NULL ){ printf("Less argument given\n"); }
		else{
			hour_str = strtok(NULL,r);
			if(hour_str == NULL){		//in case no time and year arguments given
				findPayments(&sended_ht, walletID_1, NULL, NULL, NULL, NULL);
			}
			else{
				tm1.hour = atoi(hour_str);
				min_str = strtok(NULL,s);
				if( min_str == NULL){ printf("Less argument given\n"); }
				else{
					tm1.min = atoi(min_str);
					day_str = strtok(NULL,p);
					if( day_str == NULL ){ printf("Less argument given\n"); }
					else{
						dt1.day = atoi(day_str);
						month_str = strtok(NULL,p);
						if( month_str == NULL ){ printf("Less argument given\n"); }
						else{
							dt1.month = atoi(month_str);
							year_str  = strtok(NULL,s);
							if( year_str == NULL ){ printf("Less argument given\n"); }
							else{
								dt1.year = atoi(year_str);
								hour_str = strtok(NULL,r);
								if(hour_str == NULL){ printf("Less argument given\n"); }
								else{
									tm2.hour = atoi(hour_str);
									min_str = strtok(NULL,s);
									if( min_str == NULL){ printf("Less argument given\n"); }
									else{
										tm2.min = atoi(min_str);
										day_str = strtok(NULL,p);
										if( day_str == NULL ){ printf("Less argument given\n"); }
										else{
											dt2.day = atoi(day_str);
											month_str = strtok(NULL,p);
											if( month_str == NULL ){ printf("Less argument given\n"); }
											else{
												dt2.month = atoi(month_str);
												year_str  = strtok(NULL,s);
												if( year_str == NULL ){ printf("Less argument given\n"); }
												else{
													dt2.year = atoi(year_str);
													more_arguments_flag = strtok( NULL, s );
													if( more_arguments_flag == NULL ){
														findPayments(&sended_ht, walletID_1, &tm1, &dt1, &tm2, &dt2);
													}
													else{ printf("More arguments given\n"); }
												}
											}
										}
									}
								}
							}
						}
					}	
				}
			}
		}
	}	
//  - 6 - walletStatus walletID
	else if( strcmp( entolh, "walletStatus\0" ) == 0 ){
		walletID_1 = strtok( NULL , s );
		if( walletID_1 == NULL ){ printf("Less argument given\n"); }
		else{
			more_arguments_flag = strtok( NULL, s );
			if( more_arguments_flag == NULL ){	
				walletStatus(&users_ht, walletID_1);
			}
			else{ printf("More arguments given\n"); }
		}
	}
//  - 7 - bitCoinStatus bitCoinID
	else if( strcmp( entolh , "bitCoinStatus\0" ) == 0 ){
		bitCoinID = strtok( NULL , s );
		if( bitCoinID == NULL ){ printf("Less argument given\n"); }
		else{
			more_arguments_flag = strtok( NULL, s );
			if( more_arguments_flag == NULL ){	
				bitCoinStatus(&coin_ht, bitCoinID);
			}
			else{ printf("More arguments given\n"); }
		}
	}
//  - 8 - traceCoin bitCoinID
	else if( strcmp( entolh , "traceCoin\0" ) == 0 ){
		bitCoinID = strtok( NULL , s );
		if( bitCoinID == NULL ){ printf("Less argument given\n"); }
		else{
			more_arguments_flag = strtok( NULL, s );
			if( more_arguments_flag == NULL ){	
				traceCoin(&coin_ht, bitCoinID);
			}
			else{ printf("More arguments given\n"); }
		}
	}
//  - 9 -  e(xit)
	else if( strcmp( entolh , "exit\0" ) == 0 ){
		more_arguments_flag = strtok( NULL, s );
		if( more_arguments_flag == NULL ){	
			printf("exit program\n");
			break;
		}
		else{
			printf("More arguments given\n");
		}
	}
//  -  prints hash table (just for debbuging)
	else if( strcmp( entolh , "p\0" ) == 0 ){
		more_arguments_flag = strtok( NULL, s );
		if( more_arguments_flag == NULL ){	
			printf("print hash table\n");
			print_users(&users_ht);
		}
		else{
			printf("More arguments given\n");
		}
	}
	else if( strcmp(temp_buffer,"\0") == 0 ){
		printf("No command given\n");
	}
	else{
		printf("There is no command '%s'\n",temp_buffer);
	}
	free(temp_buffer);
}
free(buffer);
free(temp_buffer);

// printf("---------------------------------------------\n\n");
// print_users(&users_ht);
// printf("---------------------------------------------\n\n");
// print_coin_list(&(coin_ht.lists[0]));

free_user_hash_table(&users_ht);
free_coin_ht(&coin_ht);
free_transactions_ht(&sended_ht);
free_transactions_ht(&received_ht);

}