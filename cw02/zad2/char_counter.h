#ifndef ZAD2_CHAR_COUNTER_H
#define ZAD2_CHAR_COUNTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#ifndef BUFF_SIZE
    #define BUFF_SIZE 200
#endif


struct results{
    size_t lines;
    size_t chars;
};
typedef struct results results;

results countWithFd(char seekChar, char* filename);
results countWithStream(char seekChar, char* filename);

#endif //ZAD2_CHAR_COUNTER_H
