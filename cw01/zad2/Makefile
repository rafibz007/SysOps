CC=gcc
CFlags=-Wall -Werror -g

all: test_static clean clean_report

static:
	$(CC) $(CFlags) -c counter.c
	ar rcs libcounter.a counter.o

shared:
	$(CC) $(CFlags) -c -fPIC counter.c
	$(CC) -shared -o libcounter.so counter.o -lc

test_static: static
	$(CC) $(CFlags) -c counter_test.c
	$(CC) $(CFlags) counter_test.o libcounter.a -o test_static

test_shared: shared
	$(CC) $(CFlags) counter_test.c -lcounter -L ./ -o test_shared

clean:
	rm -f *.o *.so *.a

clean_report:
	rm -f raport2.txt
