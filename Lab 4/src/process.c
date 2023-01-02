/*
 * thread.c - simple example demonstrating the creation of threads
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include<sys/wait.h>

/* global value */
int g_value = 0;

void* thr_func(void* ptr) {
    sleep(1);
    g_value = 1;
    return NULL;
}

int main() {
    pid_t id1 = fork();         //the parent process gets a positive value and the child gets 0. If error, -1
    if (id1 == -1) {
        fprintf(stderr, "error creating thread.\n");
        return -1;
    }
    if (id1 == 0) {
        g_value = 1;
        wait(NULL);
    }
    else
        g_value = 2;
    printf("value = %d\n", g_value);
    return 0;
}
