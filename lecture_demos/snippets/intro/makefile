override CFLAGS :=  -Werror -Wall -std=gnu99 -O0 -g $(CFLAGS) -I.
CC = gcc

prog = hello_p hello_w 
c_files = hello_p.c hello_w.c 
h_files =


all: $(prog)

.PHONY: clean 

clean:
	rm -f *.o *~ $(prog)
