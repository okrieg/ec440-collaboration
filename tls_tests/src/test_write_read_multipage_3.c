/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_write_read_multipage_3.c
 * Author: Tony Faller
 *
 * Created on April 2, 2021, 1:26 PM
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

/*
 * 
 */
int main(int argc, char** argv) {
    
    char buffer[15] = {0};
    char *msg = "Three pages";

    printf("Main: Trying to obtain TLS of size 12288...\n");
    ret = tls_create(12288);
    assert(ret == 0);
    printf("...Main: Obtained\n\n");
    
    printf("Main: Trying to write at the end of the second page...\n");
    ret = tls_write(8180, 12, msg);
    assert(ret == 0);
    printf("...Main: Write successful\n\n");
    
    memset(buffer, 0, 15);
    printf("Main: Trying to read back all bytes of the message...\n");
    ret = tls_read(8180, 12, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, msg) == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "Three pages"
    
    return (EXIT_SUCCESS);
}

