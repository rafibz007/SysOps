CC=gcc
CFLAGS=-Wall

all: test

test:
	@echo "Use following command in different terminals:"
	@echo "  make run_server"
	@echo "  make run_client_inet"
	@echo "  make run_client_unix"

run_server: server
	./server 1234 socket

run_client_inet: client
	./client client1 inet localhost:1234

run_client_unix: client
	./client client2 unix socket

client:
	$(CC) $(FLAGS) client.c -o client -lpthread -lm

server:
	$(CC) $(FLAGS) server.c -o server -lpthread -lm

clean:
	rm -f server client socket