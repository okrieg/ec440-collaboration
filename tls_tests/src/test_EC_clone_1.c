/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_EC_clone_1.c
 * Author: Tony Faller
 *
 * Created on April 2, 2021, 9:43 AM
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

int tls_create(unsigned int size);
int tls_destroy();
int tls_read(unsigned int offset, unsigned int length, char *buffer);
int tls_write(unsigned int offset, unsigned int length, const char *buffer);
int tls_clone(pthread_t tid);

int ret;

void *threadFn(void *id_ptr){
    int thread_id = *(int*)id_ptr;
    
    printf("Thread %d: pthread_self() returns %d\n", thread_id, (int)pthread_self());
    
    printf("Thread %d: Trying to obtain TLS of size 0...\n", thread_id);
    ret = tls_create(0);
    assert(ret == -1);
    printf("...Thread %d: Unable to obtain\n\n", thread_id);
    return NULL;
}

/*
 * 
 */
int main(int argc, char** argv) {
    pthread_t myThread;
    int myInt = 1;
    
    pthread_create(&myThread, NULL, threadFn, &myInt);
    
    sleep(1);
    
    printf("Main: Trying to clone TLS from thread %d...\n", myInt);
    ret = tls_clone((pthread_t)myInt);
    assert(ret == -1);
    printf("...Main: Unable to obtain\n");
        
    return (EXIT_SUCCESS);
}

