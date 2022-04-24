CC=gcc
CFlags=-Wall

all: client server


que_utils:
	$(CC) $(CFlags) -c que_utils.c
	ar rcs libqueue.a que_utils.o


client: que_utils
	$(CC) $(CFlags) -o client client.c libqueue.a


server: que_utils
	$(CC) $(CFlags) -o server server.c libqueue.a


clean:
	rm -rf server client libqueue.a que_utils.o