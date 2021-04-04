/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_EC_destroy_1.c
 * Author: Tony Faller
 *
 * Created on March 28, 2021, 8:22 PM
 * 
 * Note: This code is loosely based on code found at 
 *          https://github.com/angrave/SystemProgramming/wiki/Sample-program-using-pthread-barriers
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
    
    printf("Thread %d: Trying to obtain TLS of size 0...\n", thread_id);
    ret = tls_create(0);
    assert(ret == -1);
    printf("...Thread %d: Unable to obtain\n\n", thread_id);
    
    printf("Thread %d: Trying to destroy a nonexistent TLS...\n", thread_id);
    ret = tls_destroy();
    assert(ret == -1);
    printf("...Thread %d: Destroy failed\n\n", thread_id);
    
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
    
    printf("Main: Trying to obtain TLS of size 1000...\n");
    ret = tls_create(1000);
    assert(ret == 0);
    printf("...Main: Obtained\n\n");

    printf("Main: Trying to obtain another TLS...\n");
    ret = tls_create(100);
    assert(ret == -1);
    printf("...Main: Unable to obtain\n\n");
    
    printf("Main: Trying to destroy my TLS...\n");
    ret = tls_destroy();
    assert(ret == 0);
    printf("...Main: Destroy successful\n");
    
    return (EXIT_SUCCESS);
}

