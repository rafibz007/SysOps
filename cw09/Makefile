CC=gcc
CFlags=-Wall

all: main

test: clean main
	./main

main:
	$(CC) $(CFlags) -o main main.c -lpthread

clean:
	rm -rf main