/* 
 * EC440 Project 4
 * Thread Local Storage and Copy-on-Write
 * 
 * File:   test_write_read_multipage_2.c
 * Author: Tony Faller
 *
 * Created on March 30, 2021, 11:54 PM
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
    char buffer[10] = {0};
    char *msg = "Overlap";
        
    printf("Main: Trying to obtain TLS of size 10000...\n");
    ret = tls_create(10000);
    assert(ret == 0);
    printf("...Main: Obtained\n\n");
    
    printf("Main: Trying to write to two different pages in the TLS...\n");
    ret = tls_write(4094, 8, msg);
    assert(ret == 0);
    printf("...Main: Write successful\n\n");
    
    printf("Main: Trying to read back the zeroth byte of the message...\n");
    ret = tls_read(4094, 1, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, "O") == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "O"
    
    printf("Main: Trying to read back the first byte of the message...\n");
    ret = tls_read(4095, 1, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, "v") == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "v"
    
    memset(buffer, 0, 10);
    printf("Main: Trying to read back all bytes of the message...\n");
    ret = tls_read(4094, 8, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, msg) == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "Overlap"

    memset(buffer, 0, 10);
    printf("Main: Trying to read back last byte of the message...\n");
    ret = tls_read(4101, 1, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, "") == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect nothing, since string is null-terminated
    
    memset(buffer, 0, 10);
    printf("Main: Trying to read back the second-to-last byte of the message...\n");
    ret = tls_read(4100, 1, buffer);
    assert(ret == 0);
    assert( strcmp(buffer, "p") == 0 );
    printf("...Main: Read successful. Received message: %s\n\n", buffer);   // Expect "p"
    
    printf("Main: Trying to destroy my TLS...\n");
    ret = tls_destroy();
    assert(ret == 0);
    printf("...Main: Destroy successful\n");

    return (EXIT_SUCCESS);
}

