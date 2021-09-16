#ifndef MENU_H_
#define MENU_H_

#define MAX_LINE_SIZE 100

//  - 1 - requestTransaction senderWalletID receiverWalletID amount date time
int requestTransaction(void* user_ht, void* sended_ht, void* received_ht, char* walletID1, char* walletID2, int amount, char* tran_id, struct date dt, struct time tm);
//  - 2 - requestTransactions  senderWalletID receiverWalletID amount date time;
//                     senderWalletID2 receiverWalletID2 amount2 date2 time2;
//                       ...
//                     senderWalletIDn receiverWalletIDn amountn daten timen;
// int requestTransactions();
//  - 3 - requestTransactions inputFile
int requestTransactions_file(char* file_name, void* usr_ht, void* send_ht, void* rec_ht, struct date* latest_day, struct time* latest_time,  int* tran_ID);
//  - 4 - findEarnings walletID [time1][year1][time2][year2]
int findEarnings(void* ht, char* walletID, struct time* tm1, struct date* dt1, struct time* tm2, struct date* dt2);
//  - 5 - findPayments walletID [time1][year1][time2][year2]
int findPayments(void* ht, char* walletID, struct time* tm1, struct date* dt1, struct time* tm2, struct date* dt2);
//  - 6 - walletStatus walletID
int walletStatus(void* ht, char* name);
//  - 7 - bitCoinStatus bitCoinID
int bitCoinStatus(void* ht, char* coin);
//  - 8 - traceCoin bitCoinID
int traceCoin(void* ht, char* coin);
//  - 9 -  e(xit)
int e();


int num_of_transactions(struct tree_node* nd);
int print_coins_trans(struct tree_node* nd);

#endif
