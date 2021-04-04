/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_EC_read_1.c
 * Author: Tony Faller
 *
 * Created on March 29, 2021, 3:39 PM
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

/*
 * 
 */
int main(int argc, char** argv) {
    char buffer[1000];
    
    printf("Main: Trying to read from nonexistent TLS...\n");
    ret = tls_read(0, 500, buffer);
    assert(ret == -1);
    printf("...Main: Nonexistent read failed\n\n");
    
    printf("Main: Trying to obtain TLS of size 1000...\n");
    ret = tls_create(1000);
    assert(ret == 0);
    printf("...Main: Obtained\n\n");
    
    printf("Main: Trying to read out-of-bounds...\n");
    ret = tls_read(1000, 500, buffer);
    assert(ret == -1);
    printf("...Main: Out-of-bounds read failed\n\n");

    printf("Main: Trying to destroy my TLS...\n");
    ret = tls_destroy();
    assert(ret == 0);
    printf("...Main: Destroy successful\n");

    return (EXIT_SUCCESS);
}

