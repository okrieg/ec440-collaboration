#include <pthread.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef SUCCESS_EXIT_CODE
#error SUCCESS_EXIT_CODE must be defined to an integer value
#endif

static void *launched_function(void *arg)
{
	exit(SUCCESS_EXIT_CODE);
}

int main(int argc, char **argv)
{
	pthread_t tid;
	/*
	 * Normally we'd care to test the return value of pthread_create. But
	 * this is meant to be one of the most simple test cases, so let's not
	 * over-define it.
	 */
	(void)pthread_create(&tid, NULL, launched_function, NULL);

	for (int i = 0; i < 1000000000; ++i) {
		/* Just wasting time */
	}

	/* We should not reach here if the test succeeds. */
	exit(SUCCESS_EXIT_CODE - 1);
}
