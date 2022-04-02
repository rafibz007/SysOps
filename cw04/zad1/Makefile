CC=gcc
CFlags=-Wall

FILENAME=raport2.txt

all: clean main

main: child
	$(CC) $(CFlags) -o main main.c

child:
	$(CC) $(CFlags) -o child child.c

test: main
	@echo "=== IGNORE ===" > $(FILENAME)
	@./main ignore >> $(FILENAME)
	@echo "\n" >> $(FILENAME)

	@echo "=== HANDLER ===" >> $(FILENAME)
	@./main handler >> $(FILENAME)
	@echo "\n" >> $(FILENAME)

	@echo "=== MASK ===" >> $(FILENAME)
	@./main mask >> $(FILENAME)
	@echo "\n" >> $(FILENAME)

	@echo "=== PENDING ===" >> $(FILENAME)
	@./main pending >> $(FILENAME)


clean:
	rm -f child main