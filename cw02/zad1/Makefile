CC=gcc
CFlags=-Wall -g

TEST_FILE1=test_file.for.lib.tests0
TEST_FILE2=test_file.for.lib.tests1

REPORT_FILENAME=pomiar_zad_1.txt

all: main_static

test: gen main_static
	./gen 2 100000
	./main $(TEST_FILE1) $(TEST_FILE2)
	make clean_test_gen

	./gen 2 1000000
	./main $(TEST_FILE1) $(TEST_FILE2)
	make clean_test_gen

	./gen 2 10000000
	./main $(TEST_FILE1) $(TEST_FILE2)
	make clean_test_gen


main_static: file_joiner_static
	$(CC) $(CFlags) -c main.c
	$(CC) $(CFlags) main.o libfilejoiner.a -o main

file_joiner_static:
	$(CC) $(CFlags) -c file_joiner.c
	ar rcs libfilejoiner.a file_joiner.o

gen:
	$(CC) -o gen gen_test.c

clean:
	rm -f *.a *.o

clean_report:
	rm -f $(REPORT_FILENAME)

clean_test_gen:
	ls | grep -P "^test_file\.for\.lib\.tests[0-9]{1,}$$" | xargs -d"\n" rm -f
