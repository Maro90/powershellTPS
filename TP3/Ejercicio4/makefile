all : Servidor Cliente

Servidor: Servidor.c Socket.o Socket_Servidor.o
	gcc -g Servidor.c -o Servidor Socket.o Socket_Servidor.o -lpthread

Cliente: Cliente.c Socket.o Socket_Cliente.o
	gcc -g Cliente.c -o Cliente Socket.o Socket_Cliente.o -lpthread

Socket_Servidor.o:Socket_Servidor.h Socket_Servidor.c
	gcc -c Socket_Servidor.c

Socket_Cliente.o:Socket_Cliente.h Socket_Cliente.c
	gcc -c Socket_Cliente.c

Socket.o:Socket.h Socket.c
	gcc -c Socket.c

clean :
	rm *.o Cliente Servidor
