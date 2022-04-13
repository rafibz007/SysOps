CC=gcc
CFlags=-Wall

all: clean main

main:
	$(CC) $(CFlags) -o main main.c

test: main
	@echo "./main user@example.com NewMail TestTest x3"
	@./main "user@example.com" "NewMail1" "TestTest"
	@./main "user@example.com" "NewMail2" "TestTest"
	@./main "user@example.com" "NewMail3" "TestTest"

	@echo "./main date"
	@./main "date"

	@echo "./main sender"
	@./main "sender"

clean:
	rm -rf main