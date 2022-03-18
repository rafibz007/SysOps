CC=gcc
CFlags=-Wall

all: main_static

main_static: file_joiner_static
	$(CC) $(CFlags) -c main.c
	$(CC) $(CFlags) main.o libfilejoiner.a -o main

file_joiner_static:
	$(CC) $(CFlags) -c file_joiner.c
	ar rcs libfilejoiner.a file_joiner.o

clean:
	rm -f *.a *.o