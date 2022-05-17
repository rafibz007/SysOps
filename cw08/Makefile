CC=gcc
CFlags=-Wall

all: clean main

test: main
	./main 5 block barbara.ascii.pgm block-barbara.ascii.pgm
	./main 5 numbers barbara.ascii.pgm numbers-barbara.ascii.pgm

	./main 8 block snap.ascii.pgm block-snap.ascii.pgm
	./main 8 numbers snap.ascii.pgm numbers-snap.ascii.pgm

main:
	$(CC) $(CFlags) -o main main.c -lpthread -lm

clean:
	rm -f main block-barbara.ascii.pgm numbers-barbara.ascii.pgm block-snap.ascii.pgm numbers-snap.ascii.pgm
