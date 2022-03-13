CC=gcc
CFlags=-Wall -Werror -g

all: main_static main_shared

static:
	$(CC) $(CFlags) -c counter.c
	ar rcs libcounter.a counter.o

shared:
	$(CC) $(CFlags) -c -fPIC counter.c
	$(CC) -shared -o libcounter.so counter.o -lc

main_static: static
	$(CC) $(CFlags) -c main.c
	$(CC) $(CFlags) main.o libcounter.a -o main_static

main_shared: shared
	$(CC) $(CFlags) main.c -lcounter -L ./ -o main_shared

clean:
	rm -f *.o *.so *.a
