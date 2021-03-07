#include <stdio.h>
#include <stdlib.h>

char *hellostr = "hello class\n";

int
main()
{
  int rc;
  rc = printf("%s", hellostr);
  exit(rc);
} 
