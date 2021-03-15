#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define TEST_ASSERT(x) do { \
	if (!(x)) { \
		fprintf(stderr, "%s:%d: Assertion (%s) failed!\n", \
				__FILE__, __LINE__, #x); \
	       	abort(); \
	} \
} while(0)

pthread_barrier_t *barrier;

int main(int argc, char **argv) {

	printf("Testing that barrier with count of 0 results in EINVAL.\n");
	barrier = malloc(sizeof(*barrier));
	int init_status = pthread_barrier_init(barrier, NULL, 0);
	TEST_ASSERT(init_status == EINVAL);
	printf("Barrier with count of 0 results in error!\n\n");

	printf("Testing if barrier can be created.\n");
	init_status = pthread_barrier_init(barrier, NULL, 3);
	TEST_ASSERT(init_status == 0);
	printf("Barrier can be created!\n\n");

	printf("Testing if barrier can be destroyed.\n");
	int barrier_status = pthread_barrier_destroy(barrier);
	TEST_ASSERT(barrier_status == 0);
	printf("Barrier can be destroyed!\n\n");

	printf("Testing if barrier can be initialized after being destroyed.\n");
	init_status = pthread_barrier_init(barrier, NULL, 3);
	TEST_ASSERT(init_status == 0);
	printf("Barrier can be initialized after being destroyed!\n\n");

	printf("Finished Barrier testing.\n\n");

	return 0;
}
