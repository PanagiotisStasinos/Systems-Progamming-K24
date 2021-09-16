bitcoin : main.o transactions.o menu.o bitCoins.o users.o
	cc -o bitcoin main.o transactions.o menu.o bitCoins.o users.o -lm

main.o : main.c menu.h
	cc -c main.c
transactions.o : transactions.c transactions.h
	cc -c transactions.c
menu.o : menu.c menu.h
	cc -c menu.c
bitCoins.o : bitCoins.c bitCoins.h
	cc -c bitCoins.c
users.o : users.c users.h transactions.h
	cc -c users.c
clean :
	rm -f *.o bitcoin
