/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_write_read_multipage_1.c
 * Author: Tony Faller
 *
 * Created on March 31, 2021, 12:45 AM
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
    char *msg = "Hello World";
        
    printf("Main: Trying to obtain TLS of size 10000...\n");
    ret = tls_create(10000);
    assert(ret == 0);
    printf("...Main: Obtained\n\n");
    
    printf("Main: Trying to write to multiple pages in TLS...\n");
    ret = tls_write(4094, 12, msg);
    assert(ret == 0);
    printf("...Main: Write successful\n\n");
    
    printf("Main: Trying to read back the zeroth byte of the message...\n");
    ret = tls_read(4094, 1, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, "H") == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "H"
    
    memset(buffer, 0, 15);
    printf("Main: Trying to read back all bytes of the message...\n");
    ret = tls_read(4094, 12, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, msg) == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "Hello World"

    memset(buffer, 0, 15);
    printf("Main: Trying to read back all but two bytes of the message...\n");
    ret = tls_read(4094, 10, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, "Hello Worl") == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "Hello Worl"
    
    printf("Main: Trying to destroy my TLS...\n");
    ret = tls_destroy();
    assert(ret == 0);
    printf("...Main: Destroy successful\n");

    return (EXIT_SUCCESS);
}
