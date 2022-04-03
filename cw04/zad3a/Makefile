CC=gcc
CFlags=-g -Wall

all: clean main

main: sender catcher
	$(CC) $(CFlags) -o main main.c

sender:
	$(CC) $(CFlags) -o sender sender.c

catcher:
	$(CC) $(CFlags) -o catcher catcher.c

clean:
	rm -f main sender catcher