#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "char_counter.h"


int main(int argc, char** argv){

    if (argc < 3){
        perror("Provide: char filename");
        exit(1);
    }

    char seekChar;
    char* filename = calloc(sizeof(char ), strlen(argv[2]));
    if (filename == NULL){
        perror("Not enough memory");
        exit(1);
    }

    seekChar = argv[1][0];
    strcpy(filename, argv[2]);

    printf("%c %s\n", seekChar, filename);

    if (access(filename, R_OK)!=0){
        fprintf(stderr, "Could not open file or file do not exists: %s", filename);
        exit(1);
    }


    results res;
    res = countWithFd(seekChar, filename);
    printf("Chars:%lu, Lines:%lu\n", res.chars, res.lines);

    res = countWithStream(seekChar, filename);
    printf("Chars:%lu, Lines:%lu\n", res.chars, res.lines);

    free(filename);
    filename=NULL;


    return 0;
}

