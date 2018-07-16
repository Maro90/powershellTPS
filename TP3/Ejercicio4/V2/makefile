all : Server Client

Server: Server.c Protocol.o Game.o Users.o Questions.o
	gcc -g Server.c -o Server Protocol.o Game.o Users.o Questions.o -lpthread

Client: Client.c Protocol.o
	gcc -g Client.c -o Client Protocol.o -lpthread

Game.o:Game.h Game.c Protocol.o Users.o Questions.o
	gcc -c Game.c

Users.o:Users.h Users.c Protocol.o
	gcc -c Users.c

Questions.o:Questions.h Questions.c Protocol.o
	gcc -c Questions.c

Protocol.o:Protocol.h Protocol.c
	gcc -c Protocol.c

clean :
	rm *.o Client Server