CC=gcc
CFlags=-Wall

TEST_FILE=test_file.for.lib.tests0

REPORT_FILENAME=pomiar_zad_2.txt

all: main_static

main_static: char_counter_static
	$(CC) $(CFlags) -c main.c
	$(CC) $(CFlags) main.o libcharcounter.a -o main

test: gen main_static
	./gen 1 1000000
	./main "H" $(TEST_FILE)
	make clean_test_gen

	./gen 1 10000000
	./main "H" $(TEST_FILE)
	make clean_test_gen

	./gen 1 10000000
	./main "H" $(TEST_FILE)
	make clean_test_gen


char_counter_static:
	$(CC) $(CFlags) -c char_counter.c
	ar rcs libcharcounter.a char_counter.o

gen:
	$(CC) -o gen gen_test.c

clean:
	rm -f *.o *.a

clean_report:
	rm -f $(REPORT_FILENAME)

clean_test_gen:
	ls | grep -P "^test_file\.for\.lib\.tests[0-9]{1,}$$" | xargs -d"\n" rm -f