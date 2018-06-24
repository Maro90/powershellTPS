all : Servidor Cliente

Servidor: Servidor.c Socket.o
	gcc -g Servidor.c -o Servidor Socket.o -lpthread

Cliente: Cliente.c Socket.o
	gcc -g Cliente.c -o Cliente Socket.o -lpthread

Socket.o:Socket.h Socket.c
	gcc -c Socket.c

clean :
	rm *.o Cliente Servidor