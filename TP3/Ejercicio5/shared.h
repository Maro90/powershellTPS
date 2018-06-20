#include <sys/types.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>

#define TIMEOUT_DEFAULT 1

#define CLIENTLOCK 0
#define SERVERLOCK 1
#define DELETELOCK 2
#define CRIPTEDLOCK 3

#define MESSAGE_NAME "message"

typedef pthread_mutex_t* mutex;

typedef struct{
	int operation;
	char text[100];
}messageData;

mutex locks  [4];

messageData * message;

extern char * locksName[4];

void *getSharedMem(char* name, int size, int create);
void removeSharedMem(char * name,void * m,int size);
mutex mutexCreate(char * name);
mutex mutexGet(char * name);
int mutexLock(pthread_mutex_t * m, int timeoutSec);
void mutexUnlock(pthread_mutex_t * m);
