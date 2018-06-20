#include "shared.h"

char * locksName[4] = {"clientLock", "serverLock", "deleteLock", "criptedLock"};

void *getSharedMem(char* name, int size, int create){
	int fd;
	int mode = O_RDWR;
	void * m = NULL;
	if(create){
		mode = mode | O_CREAT;
	}
	fd = shm_open(name, mode, S_IRUSR | S_IWUSR);
	if(fd==-1){
		return NULL;
	}
	ftruncate(fd, size);

	m = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	return m;
}

void removeSharedMem(char * name,void * m,int size){
	munmap(m, size);
	shm_unlink(name);
}

mutex mutexCreate(char * name){
	mutex m = getSharedMem(name,sizeof(mutex),1);
	pthread_mutexattr_t attr;
	pthread_mutexattr_init (&attr);
	pthread_mutexattr_setpshared (&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init (m, &attr);
	return m;
}

mutex mutexGet(char * name){
	mutex m = getSharedMem(name,sizeof(mutex),0);
	return m;
}

int mutexLock(pthread_mutex_t * m, int timeoutSec){
	struct timespec timeoutTime;
    clock_gettime(CLOCK_REALTIME, &timeoutTime);
    timeoutTime.tv_sec += timeoutSec;
    return pthread_mutex_timedlock(m, &timeoutTime);
}
void mutexUnlock(pthread_mutex_t * m){
	pthread_mutex_unlock (m);
}