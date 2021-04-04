/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_clone_read.c
 * Author: Tony Faller
 *
 * Created on April 2, 2021, 2:12 PM
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>
#include <string.h>

int tls_create(unsigned int size);
int tls_destroy();
int tls_read(unsigned int offset, unsigned int length, char *buffer);
int tls_write(unsigned int offset, unsigned int length, const char *buffer);
int tls_clone(pthread_t tid);

int ret;
char *msg = "Hello";
int pthread_id;

void *threadFn(void *id_ptr){
    int thread_id = *(int*)id_ptr;
    
    pthread_id = (int)pthread_self();
    printf("Thread %d: pthread_self() returns %d\n\n", thread_id, (int)pthread_self());
    
    printf("Thread %d: Trying to obtain TLS of size 1000...\n", thread_id);
    ret = tls_create(1000);
    assert(ret == 0);
    printf("...Thread %d: Obtained\n\n", thread_id);
    
    printf("Thread %d: Trying to write string %s to TLS...\n", thread_id, msg);
    ret = tls_write(0, 6, msg);
    assert(ret == 0);
    printf("...Thread %d: Write successful\n\n", thread_id);
    
    return NULL;
}

/*
 * 
 */
int main(int argc, char** argv) {
    pthread_t myThread;
    int myInt = 1;
    char buffer[10] = {0};
    
    pthread_create(&myThread, NULL, threadFn, &myInt);
    
    sleep(1);
    
    printf("Main: Trying to clone thread %d...\n", myInt);
    ret = tls_clone((pthread_t)pthread_id);
    assert(ret == 0);
    printf("...Main: Clone successful\n\n");
    
    printf("Main: Trying to read from cloned TLS...\n");
    ret = tls_read(0, 6, buffer);
    assert(ret == 0);
    assert(strcmp(buffer, msg) == 0);
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "Hello"

    return (EXIT_SUCCESS);
}

