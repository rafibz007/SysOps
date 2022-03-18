#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "file_joiner.h"


int main(int argc, char** argv){
    char* fromFilename = calloc(101, sizeof(char));
    char* toFilename = calloc(101, sizeof(char));

    if(fromFilename == NULL){
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }
    if(toFilename == NULL){
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }


//    read/save first filename
    if (argc <= 1){
        int s = scanf("%100s", fromFilename);
        if (s <= 0){
            fprintf(stderr, "Error reading from input");
        }
    } else {
        strcpy(fromFilename, argv[1]);
    }

//    check first file
    if(access(fromFilename, F_OK) != 0){
        fprintf(stderr, "File do not exists: %s", fromFilename);
        exit(1);
    } else if(access(fromFilename, R_OK) != 0) {
        fprintf(stderr, "No read permissions: %s", fromFilename);
        exit(1);
    }


//    read/save second filename
    if (argc <= 2){
        int s = scanf("%100s", toFilename);
        if (s <= 0){
            fprintf(stderr, "Error reading from input");
        }
    } else {
        strcpy(toFilename, argv[2]);
    }

//    check second file
    if(access(toFilename, F_OK) != 0){
        fprintf(stderr, "File do not exists: %s", toFilename);
        exit(1);
    } else if(access(toFilename, W_OK) != 0) {
        fprintf(stderr, "No read permissions: %s", toFilename);
        exit(1);
    }


    copyWithDescriptors(fromFilename, toFilename);
    copyWithStreams(fromFilename, toFilename);


    return 0;
}

