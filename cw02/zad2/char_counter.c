#include "char_counter.h"


results countWithFd(char seekChar, char* filename){
    int fd = open(filename, R_OK);
    if (fd == -1){
        fprintf(stderr, "Could not open file or file do not exists: %s", filename);
        exit(1);
    }

    char* buff = calloc(BUFF_SIZE+1, sizeof(char));
    if (buff==NULL){
        perror("Not enough memory");
        exit(1);
    }

    size_t foundChars = 0;
    size_t foundLines = 0;
    size_t len;
    bool reachedEOF = false;
    bool lineCounted = false;
    while (!reachedEOF){

        len = read(fd, buff, BUFF_SIZE);
        if (len<BUFF_SIZE){
            reachedEOF=true;
        }

        for (int i = 0; i < len && *(buff+1); ++i) {

            if (*(buff+i)=='\n'){
                lineCounted = false;
            } else if (*(buff+i) == seekChar){
                if (!lineCounted){
                    foundLines++;
                    lineCounted = true;
                }
                foundChars++;
            }

        }

    }

    results res;
    res.chars = foundChars;
    res.lines = foundLines;

    return res;

}


results countWithStream(char seekChar, char* filename){
    FILE* fd = fopen(filename, "r");
    if (fd == NULL){
        fprintf(stderr, "Could not open file or file do not exists: %s", filename);
        exit(1);
    }

    char* buff = calloc(BUFF_SIZE+1, sizeof(char));
    if (buff==NULL){
        perror("Not enough memory");
        exit(1);
    }

    size_t foundChars = 0;
    size_t foundLines = 0;
    size_t len;
    bool reachedEOF = false;
    bool lineCounted = false;
    while (!reachedEOF){

        len = fread(buff, sizeof(char), BUFF_SIZE, fd);
        if (len<BUFF_SIZE){
            reachedEOF=true;
        }

        for (int i = 0; i < len && *(buff+1); ++i) {

            if (*(buff+i)=='\n'){
                lineCounted = false;
            } else if (*(buff+i) == seekChar){
                if (!lineCounted){
                    foundLines++;
                    lineCounted = true;
                }
                foundChars++;
            }

        }

    }

    results res;
    res.chars = foundChars;
    res.lines = foundLines;

    return res;

}