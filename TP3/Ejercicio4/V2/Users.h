#ifndef _USERS_H
#define _USERS_H

#define USER_STATUS_OK 1
#define USER_STATUS_CONNECTIONERROR -1
#define USER_STATUS_GAMEALREADYSTARTED 2
#define USER_STATUS_QUIT 3


#include <stdio.h>
#include <string.h>
#include<pthread.h>

#include "Protocol.h"

typedef struct user{
    char name[30];
}tUser;

typedef struct statistics{
    int count;
    int correct;
    int incorrect;
}tStatistics;

typedef struct userNode{
    tUser user;
    tStatistics statistics;
    tConnection connection;
    pthread_t threadId;
    int status;
    struct userNode * prev;
}tUserNode;

tUserNode * getUser(int);

tUserNode * addUser(tConnection);

void freeUserList();

void userQuit(int,int);

int send_all(tCommand *,void *);

void initController(tMessageInit, tConnection *);

void acceptController(tMessageAccept, tConnection * );

int getUserCount();

#endif