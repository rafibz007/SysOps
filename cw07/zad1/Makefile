CC=gcc
CFLAGS=-Wall

all:
	$(CC) $(FLAGS) -o supplier supplier.c utils.c
	$(CC) $(FLAGS) -o cook cook.c utils.c
	$(CC) $(FLAGS) -o main main.c utils.c

clean:
	rm -f main cook supplier