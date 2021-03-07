#include <stdio.h>
#include <setjmp.h>

static jmp_buf buf;

enum {
      RETURN_FROM_LONGJ = 5
};

void second(void) {
  printf("second\n");
  longjmp(buf, RETURN_FROM_LONGJ);
}

void first(void) {
  printf("first- before calling second\n");
  second();
  printf("first- after calling second\n");
}


int
main()
{
  if(setjmp(buf) == RETURN_FROM_LONGJ) {
    printf("main\n");
  } else {
    first();
  }
  return 0;
}
