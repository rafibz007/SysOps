CC=gcc
CFlags=-Wall

all: main_nftw main_open_dir

main_open_dir: tree_open_dir
	$(CC) $(CFlags) -c mainOpenDir.c
	$(CC) $(CFlags) mainOpenDir.o libtreeopendir.a -o mainOpenDir

tree_open_dir:
	$(CC) $(CFlags) -c tree_open_dir.c
	ar rcs libtreeopendir.a tree_open_dir.o

main_nftw:
	$(CC) $(CFlags) mainNftw.c -o mainNftw

clean:
	rm -f *.o *.a