#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char *hellostr = "hello class\n";

int
main()
{
  int rc;
  rc = write(STDOUT_FILENO, hellostr, strlen(hellostr)+1);
  exit(rc);
} 
