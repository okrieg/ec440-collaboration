// Benjamin Li, HW4 test
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include "tls.c"

#define TEST_ASSERT(x) do { \
	if (!(x)) { \
		fprintf(stderr, "%s:%d: Assertion (%s) failed!\n", \
				__FILE__, __LINE__, #x); \
	       	abort(); \
	} \
} while(0)

#define SIZE0 1000 // bytes

void *clone_fail(void *arg)
{
	pthread_t target_thread_id = (pthread_t) arg;
	int status = tls_clone(target_thread_id);
	TEST_ASSERT(status == -1);
	printf("Cloning from a target thread that does not have a TLS results in error!\n\n");
	pthread_exit(NULL);
	return NULL;
}

int main(int argc, char **argv)
{
	int status;
	// char reference_buffer[SIZE0];
	char buffer1[SIZE0];
	// char buffer2[SIZE0];
	pthread_t threads[1];

	printf("Testing that calling tls_destroy() with a thread that does not have a TLS errors.\n");
	status = tls_destroy();
	TEST_ASSERT(status == -1);
	printf("Calling tls_destroy() with a threads that does not have a TLS errors.\n");

	printf("Testing if creating a TLS with a non-positive size results in error.\n");
	status = tls_create(0);
	TEST_ASSERT(status == -1);
	printf("Creating a TLS with a non-positive size results in error!\n\n");

	printf("Creating TLS for further testing.\n");
	status = tls_create(SIZE0);
	TEST_ASSERT(status == 0);
	printf("TLS ready for further testing!\n\n");

	printf("Testing if creating a TLS for a thread that already has a TLS results in error.\n");
	status = tls_create(SIZE0);
	TEST_ASSERT(status == -1);
	printf("Creating a TLS for a thread that already has a TLS results in error!\n\n");

	printf("Deleting TLS for further testing.\n");
	status = tls_destroy();
	TEST_ASSERT(status == 0);
	printf("TLS ready for further testing!\n\n");

	printf("Testing if calling tls_destroy() on a thread without a TLS results in error.\n");
	status = tls_destroy();
	TEST_ASSERT(status == -1);
	printf("Calling tls_destroy() on a thread without a TLS results in error!\n\n");

	printf("Testing if calling tls_read() on a thread without a TLS results in error.\n");
	status = tls_read(0, SIZE0, buffer1);
	TEST_ASSERT(status == -1);
	printf("Calling tls_read() on a thread without a TLS results in error!\n\n");

	printf("Testing if calling tls_write() on a thread without a TLS results in error.\n");
	status = tls_write(0, SIZE0, buffer1);
	TEST_ASSERT(status == -1);
	printf("Calling tls_write() on a thread without a TLS results in error!\n\n");

	printf("Creating TLS for further testing.\n");
	status = tls_create(SIZE0);
	TEST_ASSERT(status == 0);
	printf("TLS ready for further testing!\n\n");

	printf("Testing if calling tls_read() that overflows the allocated pages results in error.\n");
	status = tls_read(0, 2 * SIZE0, buffer1);
	TEST_ASSERT(status == -1);
	status = tls_read(SIZE0 / 2, SIZE0, buffer1);
	TEST_ASSERT(status == -1);
	printf("Calling tls_read() that overflows the allocated pages results in error!\n\n");

	printf("Testing if calling tls_write() that overflows the allocated pages results in error.\n");
	status = tls_write(0, 2 * SIZE0, buffer1);
	TEST_ASSERT(status == -1);
	status = tls_write(SIZE0 / 2, SIZE0, buffer1);
	TEST_ASSERT(status == -1);
	printf("Calling tls_write() that overflows the allocated pages results in error!\n\n");

	printf("Testing if attempting to clone a thread that does not have a TLS results in error.\n");
	status = tls_destroy();
	TEST_ASSERT(status == 0);
	pthread_create(&threads[0], NULL, clone_fail, (void *)(pthread_self()));
	pthread_join(threads[0], NULL);

	printf("Finished with error checking!\n");

	pthread_exit(0);
	return 0;
}
