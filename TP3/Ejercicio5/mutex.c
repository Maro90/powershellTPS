#include <sys/types.h>
#include <pthread.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main () {

	int fd = shm_open("miMemoria", O_RDONLY, 0);
	pthread_mutex_t *mutex = (pthread_mutex_t*) mmap(NULL, sizeof (pthread_mutex_t) + sizeof (long), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, fd, 0);
	close(fd);


	int stat;

    
    long *data = (long*)(&mutex[1]); /* map 'data' after mutex */
	int pid;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init (&attr);
	pthread_mutexattr_setpshared (&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init (mutex, &attr);
	*data = 0;

	pid = fork ();
	printf("waiting lock %d \n", getpid() );
	pthread_mutex_lock (mutex);
	printf("locked by %d \n", getpid() );
	sleep(1);
	(*data)++;
	printf("unlocked by %d \n", getpid() );
	sleep(1);
	pthread_mutex_unlock (mutex);
               if (!pid)        /* child exits */
	exit (0);
	else
		waitpid (pid, &stat, 0);
	printf ("data is %ld\n",*data);
}