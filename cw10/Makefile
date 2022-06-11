CC=gcc
CFLAGS=-Wall

all: client server

run_server: server
	./server 1234 socket

run_client1: client
	./client client1 inet localhost:1234

run_client2: client
	./client client2 unix socket

client:
	$(CC) $(FLAGS) client.c -o client -lpthread -lm

server:
	$(CC) $(FLAGS) server.c -o server -lpthread -lm

clean:
	rm -f server client socket