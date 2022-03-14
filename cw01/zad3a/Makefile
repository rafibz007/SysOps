CC=gcc
CFlags=-Wall -Werror -g

all: test

# zdecydowałem się na generowanie plików z powodu małej ilości miejsca na UPeLu

REPORT_FILENAME="raport3a.txt"

# test generator
gen:
	$(CC) test_gen.c -o gen

gen_small: gen clean_test_gen
	./gen 10 5000

gen_medium: gen clean_test_gen
	./gen 7 10000

gen_large: gen clean_test_gen
	./gen 4 15000


# compiling libraries
static:
	$(CC) $(CFlags) -c counter.c
	ar rcs libcounter.a counter.o

shared:
	$(CC) $(CFlags) -c -fPIC counter.c
	$(CC) -shared -o libcounter.so counter.o -lc


# test
test: clean clean_report gen counter_test_dynamic counter_test_shared counter_test_static
	@touch $(REPORT_FILENAME)

	make gen_small
	@echo "====== DYNAMIC - SMALL TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 test_file.for.lib.tests7 test_file.for.lib.tests8 test_file.for.lib.tests9 remove_block 0 create_table 1000 add_remove_blocks 1000
	@echo "====== SHARED - SMALL TEST  ======" >> $(REPORT_FILENAME)
	@./test_shared 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 test_file.for.lib.tests7 test_file.for.lib.tests8 test_file.for.lib.tests9 remove_block 0 create_table 1000 add_remove_blocks 1000
	@echo "====== STATIC - SMALL TEST  ======" >> $(REPORT_FILENAME)
	@./test_static 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 test_file.for.lib.tests7 test_file.for.lib.tests8 test_file.for.lib.tests9 remove_block 0 create_table 1000 add_remove_blocks 1000

	make gen_medium
	@echo "====== DYNAMIC - MEDIUM TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 remove_block 0 create_table 10000 add_remove_blocks 10000
	@echo "====== SHARED - MEDIUM TEST  ======" >> $(REPORT_FILENAME)
	@./test_shared 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 remove_block 0 create_table 10000 add_remove_blocks 10000
	@echo "====== STATIC - MEDIUM TEST  ======" >> $(REPORT_FILENAME)
	@./test_static 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 remove_block 0 create_table 10000 add_remove_blocks 10000

	make gen_large
	@echo "====== DYNAMIC - LARGE TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 remove_block 0 create_table 100000 add_remove_blocks 100000
	@echo "====== SHARED - LARGE TEST  ======" >> $(REPORT_FILENAME)
	@./test_shared 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 remove_block 0 create_table 100000 add_remove_blocks 100000
	@echo "====== STATIC - LARGE TEST  ======" >> $(REPORT_FILENAME)
	@./test_static 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 remove_block 0 create_table 100000 add_remove_blocks 100000

	make clean_test_gen

test_dynamic: counter_test_dynamic gen
	@touch $(REPORT_FILENAME)

	make gen_small
	@echo "====== DYNAMIC - SMALL TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 test_file.for.lib.tests7 test_file.for.lib.tests8 test_file.for.lib.tests9 remove_block 0 create_table 1000 add_remove_blocks 1000

	make gen_medium
	@echo "====== DYNAMIC - MEDIUM TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 remove_block 0 create_table 10000 add_remove_blocks 10000

	make gen_large
	@echo "====== DYNAMIC - LARGE TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 remove_block 0 create_table 100000 add_remove_blocks 100000

	make clean_test_gen

test_shared: counter_test_shared gen
	@touch $(REPORT_FILENAME)

	make gen_small
	@echo "====== DYNAMIC - SMALL TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 test_file.for.lib.tests7 test_file.for.lib.tests8 test_file.for.lib.tests9 remove_block 0 create_table 1000 add_remove_blocks 1000

	make gen_medium
	@echo "====== DYNAMIC - MEDIUM TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 remove_block 0 create_table 10000 add_remove_blocks 10000

	make gen_large
	@echo "====== DYNAMIC - LARGE TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 remove_block 0 create_table 100000 add_remove_blocks 100000

	make clean_test_gen

test_static: counter_test_static gen
	@touch $(REPORT_FILENAME)

	make gen_small
	@echo "====== DYNAMIC - SMALL TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 test_file.for.lib.tests7 test_file.for.lib.tests8 test_file.for.lib.tests9 remove_block 0 create_table 1000 add_remove_blocks 1000

	make gen_medium
	@echo "====== DYNAMIC - MEDIUM TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 test_file.for.lib.tests4 test_file.for.lib.tests5 test_file.for.lib.tests6 remove_block 0 create_table 10000 add_remove_blocks 10000

	make gen_large
	@echo "====== DYNAMIC - LARGE TEST  ======" >> $(REPORT_FILENAME)
	@./test_dynamic 1 wc_files test_file.for.lib.tests0 test_file.for.lib.tests1 test_file.for.lib.tests2 test_file.for.lib.tests3 remove_block 0 create_table 100000 add_remove_blocks 100000

	make clean_test_gen

# compiling testers
counter_test_dynamic: shared
	$(CC) $(CFlags) -o test_dynamic counter_test_dynamic.c -ldl

counter_test_shared: shared
	$(CC) $(CFlags) counter_test.c -lcounter -L ./ -o test_shared

counter_test_static: static
	$(CC) $(CFlags) -c counter_test.c
	$(CC) $(CFlags) counter_test.o libcounter.a -o test_static


# cleaning
clean:
	rm -f *.o *.so *.a

clean_report:
	rm -f $(REPORT_FILENAME)

clean_test_gen:
	ls | grep -P "^test_file\.for\.lib\.tests[0-9]{1,}$$" | xargs -d"\n" rm -f