all: 	\
	test17_6 test16_4 process_mutex_deadlock

test17_6 : test17_6.c 
	gcc -g test17_6.c -o test17_6

test16_4 : test16_4.c
	gcc -o test16_4 test16_4.c -pthread

process_mutex_deadlock : process_mutex_deadlock.c
	gcc -o process_mutex_deadlock process_mutex_deadlock.c -pthread

clean :
	rm -f *.o test16_4 test17_6 process_mutex_deadlock