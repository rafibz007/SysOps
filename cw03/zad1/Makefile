CC=gcc
CFlags=-Wall


all: clean main child


main: child
	$(CC) $(CFlags) -o main main.c


child:
	$(CC) $(CFlags) -o child child.c

clean:
	rm -f main child