all: 	\
	mirror_client sender receiver deleter

sender : sender.c sender_utils.c
	gcc -o sender sender.c sender_utils.c

receiver : receiver.c
	gcc -g receiver.c -o receiver

deleter : deleter.c
	gcc -g deleter.c -o deleter

mirror_client : main.c utils.c
	gcc -o mirror_client main.c utils.c

clean :
	rm -f *.o mirror_client sender receiver deleter