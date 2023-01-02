#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int g_value = 0;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;       //the lock and cond have to be initialized like this

void *thr_func(void *ptr) {
	// first step: wait until the value is positive
	while (!(g_value > 0)) {
		if (pthread_mutex_lock(&g_mutex) != 0)  // the mutex has to be locked beforehand
			exit(EXIT_FAILURE);

		pthread_cond_wait(&cond, &g_mutex);   //the wait unlocks if the time isn't right, and locks if it got the 
                                                                        //signal. So it does the same as before, but without holding the
                                                                        //lock.
	}

	// second step: change the value
	fprintf(stdout, "[thread #%ld] read value=%d, will increment\n", pthread_self(), g_value);
	g_value++;

	if (pthread_mutex_unlock(&g_mutex) != 0) exit(EXIT_FAILURE);

	return NULL;
}

int main() {
	pthread_t tid;
	pthread_cond_init(&cond, NULL);     //the cond has to be initialized like this in the parent thread

	// spin new thread
	int error_num = pthread_create(&tid, NULL, thr_func, NULL);
	if (error_num != 0) {
		fprintf(stderr, "error creating thread: strerror(%s)\n", strerror(error_num));
		return -1;
	}

	// artificial delay
	sleep(5);

	if (pthread_mutex_lock(&g_mutex) != 0) return -1;

	// increment the value
	g_value++;

	pthread_cond_signal(&cond);      //note that the signal is sent before the lock is lost

	if (pthread_mutex_unlock(&g_mutex) != 0) return -1;

	// wait for the other thread to finish
	if (pthread_join(tid, NULL) != 0) return -1;

	if (pthread_mutex_lock(&g_mutex) != 0) return -1;

	fprintf(stdout, "[main thread] value=%d\n", g_value);

	if (pthread_mutex_unlock(&g_mutex) != 0) return -1;

	return 0;
}
