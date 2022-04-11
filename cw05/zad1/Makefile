CC=gcc
CFlags=-Wall

all: main

main:
	$(CC) $(CFlags) -o main main.c

test: main
	@echo "./main 'skladnik1 | skladnik3'"
	@./main "skladnik1 | skladnik3"

	@echo "./main 'skladnik2 | skladnik4'"
	@./main "skladnik2 | skladnik4"

clean:
	rm -rf main