CC=gcc
CFlags=-Wall

all: clean main

test: clean main
	./main test/ halo 5
	./main test/ halo 4

main:
	$(CC) $(CFlags) -o main main.c

clean:
	rm -f main