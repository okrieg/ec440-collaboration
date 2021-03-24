#include <assert.h>

#include "tls.h"

int main(int argc, char **argv)
{
	// We haven't created a TLS yet. Destroy should fail.
	assert(tls_destroy() == -1);

	return 0;
}
