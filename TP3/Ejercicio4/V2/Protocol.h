#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define COMMAND_INIT 1000
#define COMMAND_ACCEPT 1001
#define COMMAND_QUIT 1002
#define COMMAND_QUESTION 1003
#define COMMAND_ANSWER 1004
#define COMMAND_STATISTICS 1005

//command
typedef struct command{
	int commandId;
}tCommand;


//messages
typedef struct messageInit{
	char user[30];
}tMessageInit;

typedef struct messageAccept{
	int id;
	char message[100];
}tMessageAccept;

typedef struct messageQuit{
	char message[100];
}tMessageQuit;

typedef struct messageQuestion{
	char title[100];
	char responses[4][100];
}tMessageQuestion;

typedef struct messageAnswer{
	int id;
}tMessageAnswer;

typedef struct messageStatistics{
	char message[100];
}tMessageStatistics;

//main loop thread
void * handler_run(void *args);


typedef struct connection{
	int id;
	int descriptor;
	void (*initController)(tMessageInit, struct connection *);
	void (*acceptController)(tMessageAccept, struct connection *);
	void (*quitController)(tMessageQuit, struct connection *);
	void (*questionController)(tMessageQuestion, struct connection *);
	void (*answerController)(tMessageAnswer, struct connection *);
	void (*statisticsController)(tMessageStatistics, struct connection *);
}tConnection;

int send_command(tCommand *,void *, tConnection *);
int read_socket (void *, int, tConnection *);
int send_socket (void * ,int  , tConnection * );

#endif