CC=gcc
CFlags=-Wall -Werror -g

all: static shared

static:
	$(CC) $(CFlags) -c counter.c
	ar rcs libcounter.a counter.o

shared:
	$(CC) $(CFlags) -c -fPIC counter.c
	$(CC) -shared -o libcounter.so counter.o -lc

clean:
	rm -f *.o *.so *.a
