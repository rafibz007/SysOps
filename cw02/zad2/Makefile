CC=gcc
CFlags=-Wall

all: main_static


main_static: char_counter_static
	$(CC) $(CFlags) -c main.c
	$(CC) $(CFlags) main.o libcharcounter.a -o main

char_counter_static:
	$(CC) $(CFlags) -c char_counter.c
	ar rcs libcharcounter.a char_counter.o

clean:
	rm -f *.o *.a