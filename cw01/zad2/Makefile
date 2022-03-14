CC=gcc
CFlags=-Wall -Werror -g


all: counter_test_static counter_test_shared clean


# zdecydowałem się na generowanie plików z powodu małej ilości miejsca na UPeLu
# dla nauki postanowilem napisac generacje w bashu, w nastepnym zad upieksze kod uzywajac generatora napisanego w c

# test files generation constants
LOREM1="Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum."
LARGE_LOREM_REPETITION=5000
LARGE_LOREM_FILES=4
LARGE_BLOCK_ALLOCATIONS=100000

MEDIUM_LOREM_REPETITION=3000
MEDIUM_LOREM_FILES=7
MEDIUM_BLOCK_ALLOCATIONS=10000

SMALL_LOREM_REPETITION=1000
SMALL_LOREM_FILES=10
SMALL_BLOCK_ALLOCATIONS=1000

REPORT_FILENAME="raport2.txt"

# compiling libraries
static:
	$(CC) $(CFlags) -c counter.c
	ar rcs libcounter.a counter.o

shared:
	$(CC) $(CFlags) -c -fPIC counter.c
	$(CC) -shared -o libcounter.so counter.o -lc


# test
test: clean clean_report clean
	@touch $(REPORT_FILENAME)
	@echo "====== STATIC - SMALL TEST  ======" >> $(REPORT_FILENAME)
	make small_test_static
	@echo "====== STATIC - MEDIUM TEST ======" >> $(REPORT_FILENAME)
	make medium_test_static
	@echo "====== STATIC - LARGE TEST  ======" >> $(REPORT_FILENAME)
	make large_test_static
	@echo "====== SHARED - SMALL TEST  ======" >> $(REPORT_FILENAME)
	make small_test_shared
	@echo "====== SHARED - MEDIUM TEST ======" >> $(REPORT_FILENAME)
	make medium_test_shared
	@echo "====== SHARED - LARGE TEST  ======" >> $(REPORT_FILENAME)
	make large_test_shared


# testing static
counter_test_static: static
	$(CC) $(CFlags) -c counter_test.c
	$(CC) $(CFlags) counter_test.o libcounter.a -o test_static


small_test_static: SHELL := /bin/bash
small_test_static: counter_test_static
	@ \
 	files=() \
	i=1; \
	while [ "$$i" -le $(SMALL_LOREM_FILES) ]; do \
        files+=(`mktemp`); i=$$((i + 1));\
    done; \
    for key in "$${!files[@]}" ; do \
		j=1; \
		limit=$$RANDOM; \
		limit=$$((limit%3)); \
		limit+=$(SMALL_LOREM_REPETITION); \
		while [ "$$j" -le "$$limit" ]; do \
			echo $(LOREM1) >> "$${files[$${key}]}"; j=$$((j + 1));\
		done; \
    done; \
    \
    ./test_static 1 wc_files $${files[@]} remove_block 0 create_table $(SMALL_BLOCK_ALLOCATIONS) add_remove_blocks $(SMALL_BLOCK_ALLOCATIONS);\
    \
    for key in "$${!files[@]}" ; do \
	  rm "$${files[$${key}]}"; \
	done;

medium_test_static: SHELL := /bin/bash
medium_test_static: counter_test_static
	@ \
	files=() \
	i=1; \
	while [ "$$i" -le $(MEDIUM_LOREM_FILES) ]; do \
		files+=(`mktemp`); i=$$((i + 1));\
	done; \
	for key in "$${!files[@]}" ; do \
		j=1; \
		limit=$$RANDOM; \
		limit=$$((limit%3)); \
		limit+=$(MEDIUM_LOREM_REPETITION); \
		while [ "$$j" -le "$$limit" ]; do \
			echo $(LOREM1) >> "$${files[$${key}]}"; j=$$((j + 1));\
		done; \
	done; \
	\
	./test_static 1 wc_files $${files[@]} remove_block 0 create_table $(MEDIUM_BLOCK_ALLOCATIONS) add_remove_blocks $(MEDIUM_BLOCK_ALLOCATIONS);\
	\
	for key in "$${!files[@]}" ; do \
	  rm "$${files[$${key}]}"; \
	done;

large_test_static: SHELL := /bin/bash
large_test_static: counter_test_static
	@ \
	files=() \
	i=1; \
	while [ "$$i" -le $(LARGE_LOREM_FILES) ]; do \
		files+=(`mktemp`); i=$$((i + 1));\
	done; \
	for key in "$${!files[@]}" ; do \
		j=1; \
		limit=$$RANDOM; \
		limit=$$((limit%3)); \
		limit+=$(LARGE_LOREM_REPETITION); \
		while [ "$$j" -le "$$limit" ]; do \
			echo $(LOREM1) >> "$${files[$${key}]}"; j=$$((j + 1));\
		done; \
	done; \
	\
	./test_static 1 wc_files $${files[@]} remove_block 0 create_table $(LARGE_BLOCK_ALLOCATIONS) add_remove_blocks $(LARGE_BLOCK_ALLOCATIONS);\
	\
	for key in "$${!files[@]}" ; do \
	  rm "$${files[$${key}]}"; \
	done;


# testing shared
counter_test_shared: shared
	$(CC) $(CFlags) counter_test.c -lcounter -L ./ -o test_shared


small_test_shared: SHELL := /bin/bash
small_test_shared: counter_test_shared
	@ \
 	files=() \
	i=1; \
	while [ "$$i" -le $(SMALL_LOREM_FILES) ]; do \
        files+=(`mktemp`); i=$$((i + 1));\
    done; \
    for key in "$${!files[@]}" ; do \
		j=1; \
		limit=$$RANDOM; \
		limit=$$((limit%3)); \
		limit+=$(SMALL_LOREM_REPETITION); \
		while [ "$$j" -le "$$limit" ]; do \
			echo $(LOREM1) >> "$${files[$${key}]}"; j=$$((j + 1));\
		done; \
    done; \
    \
    ./test_shared 1 wc_files $${files[@]} remove_block 0 create_table $(SMALL_BLOCK_ALLOCATIONS) add_remove_blocks $(SMALL_BLOCK_ALLOCATIONS);\
    \
    for key in "$${!files[@]}" ; do \
	  rm "$${files[$${key}]}"; \
	done;

medium_test_shared: SHELL := /bin/bash
medium_test_shared: counter_test_shared
	@ \
	files=() \
	i=1; \
	while [ "$$i" -le $(MEDIUM_LOREM_FILES) ]; do \
		files+=(`mktemp`); i=$$((i + 1));\
	done; \
	for key in "$${!files[@]}" ; do \
		j=1; \
		limit=$$RANDOM; \
		limit=$$((limit%3)); \
		limit+=$(MEDIUM_LOREM_REPETITION); \
		while [ "$$j" -le "$$limit" ]; do \
			echo $(LOREM1) >> "$${files[$${key}]}"; j=$$((j + 1));\
		done; \
	done; \
	\
	./test_shared 1 wc_files $${files[@]} remove_block 0 create_table $(MEDIUM_BLOCK_ALLOCATIONS) add_remove_blocks $(MEDIUM_BLOCK_ALLOCATIONS);\
	\
	for key in "$${!files[@]}" ; do \
	  rm "$${files[$${key}]}"; \
	done;

large_test_shared: SHELL := /bin/bash
large_test_shared: counter_test_shared
	@ \
	files=() \
	i=1; \
	while [ "$$i" -le $(LARGE_LOREM_FILES) ]; do \
		files+=(`mktemp`); i=$$((i + 1));\
	done; \
	for key in "$${!files[@]}" ; do \
		j=1; \
		limit=$$RANDOM; \
		limit=$$((limit%3)); \
		limit+=$(LARGE_LOREM_REPETITION); \
		while [ "$$j" -le "$$limit" ]; do \
			echo $(LOREM1) >> "$${files[$${key}]}"; j=$$((j + 1));\
		done; \
	done; \
	\
	./test_shared 1 wc_files $${files[@]} remove_block 0 create_table $(LARGE_BLOCK_ALLOCATIONS) add_remove_blocks $(LARGE_BLOCK_ALLOCATIONS);\
	\
	for key in "$${!files[@]}" ; do \
	  rm "$${files[$${key}]}"; \
	done;

# cleaning
clean:
	rm -f *.o *.so *.a

clean_report:
	rm -f $(REPORT_FILENAME)
