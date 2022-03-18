#ifndef ZAD1_FILE_JOINER_H
#define ZAD1_FILE_JOINER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>

#ifndef BUFF_SIZE
    #define BUFF_SIZE 256
#endif

void copyWithDescriptors(char* fromFilename, char* toFilename);
void copyWithStreams(char* fromFilename, char* toFilename);

void addTmpContentWithFd(int tmpFd, int toFd, char** tmpBuff, size_t tmpContentLength, size_t* tmpBuffSize);
void addTmpContentWithStreams(FILE* tmpFd, FILE* toFd, char** tmpBuff, size_t tmpContentLength, size_t* tmpBuffSize);

#endif //ZAD1_FILE_JOINER_H
