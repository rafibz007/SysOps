CC=gcc
CFlags=-Wall

all: clean main

test: clean main
	./main test/ no 5
	./main test/ no 4
	./main test/ halo 4
	./main test/ halo 2

main:
	$(CC) $(CFlags) -o main main.c

clean:
	rm -f main