CC=gcc
CFlags=-g -Wall

all: clean main

test: clean main
	@echo "\n"
	./main 1000 KILL
	@echo "\n"
	./main 1000 SIGRT
	@echo "\n"
	./main 1000 SIGQUEUE

main: sender catcher
	$(CC) $(CFlags) -o main main.c

sender:
	$(CC) $(CFlags) -o sender sender.c

catcher:
	$(CC) $(CFlags) -o catcher catcher.c

clean:
	rm -f main sender catcher