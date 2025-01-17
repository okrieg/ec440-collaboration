#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>
#include "ec440threads.h"

/* Extracted from private libc headers. These are not part of the public
 * interface for jmp_buf.
 */
#define JB_RBX 0
#define JB_RBP 1
#define JB_R12 2
#define JB_R13 3
#define JB_R14 4
#define JB_R15 5
#define JB_RSP 6
#define JB_PC 7

static jmp_buf buf;


void
foo()
{
  printf("you called foo\n");
  //exit(0);
}

int
main()
{
  setjmp(buf);
  // hey lets play with this...
  buf[0].__jmpbuf[JB_PC] = ptr_mangle((long unsigned int)foo);
  longjmp(buf, 1);
  return 0;
}
