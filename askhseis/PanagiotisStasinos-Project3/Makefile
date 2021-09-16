all: 	\
	dropbox_server dropbox_client

dropbox_server : dropbox_server.c server_utils.c
	gcc -g dropbox_server.c server_utils.c -o dropbox_server

dropbox_client : dropbox_client.c client_utils.c thread_pool.c file_utils.c
	gcc -o dropbox_client dropbox_client.c client_utils.c thread_pool.c file_utils.c -pthread

clean :
	rm -f *.o dropbox_server dropbox_client