CC=gcc
CFlags=-Wall


all: clean main child


main: child
	$(CC) $(CFlags) -o main main.c


child:
	$(CC) $(CFlags) -o child child.c

test: main
	@./main 0.0000001 10
	@./main 0.0000001 20
	@./main 0.0000001 30

	@./main 0.00000001 10
	@./main 0.00000001 20
	@./main 0.00000001 30

	@./main 0.000000001 10
	@./main 0.000000001 20
	@./main 0.000000001 30

	@./main 0.0000000005 10
	@./main 0.0000000005 20
	@./main 0.0000000005 30

	@./main 0.0000000001 10
	@./main 0.0000000001 20
	@./main 0.0000000001 30

	make clean


clean:
	rm -f main child
	ls | grep -P "^w[0-9]{1,}\.txt$$" | xargs -d "\n" rm -f