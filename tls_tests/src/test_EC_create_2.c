/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_EC_create_2.c
 * Author: Tony Faller
 *
 * Created on March 28, 2021, 8:29 PM
 * 
 * Note: This code tests for TLS_create() with no prior call to pthread_create().
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
    
    printf("Main: Trying to obtain TLS of size 1000...\n");
    ret = tls_create(1000);
    assert(ret == 0);
    printf("...Main: Obtained\n\n");

    printf("Main: Trying to obtain another TLS...\n");
    ret = tls_create(100);
    assert(ret == -1);
    printf("...Main: Unable to obtain\n");
    
    return (EXIT_SUCCESS);
}