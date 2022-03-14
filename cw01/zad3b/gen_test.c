#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

bool isNumber(char* value);
void printUsage();

char* ALPHABET = "ABCDEFGHIJ   \n\n\n";
char* FILENAME_CORE = "test_file.for.lib.tests";


int main(int argc, char* argv[]) {

    if (argc < 3 || !isNumber(argv[1]) || !isNumber(argv[2])){
        printUsage();
        exit(1);
    }

    size_t amountOfFiles, amountOfChars;
    amountOfFiles = strtol(argv[1], NULL, 10);
    amountOfChars = strtol(argv[2], NULL, 10);

    size_t ALPHABET_LEN = strlen(ALPHABET);
    size_t FILENAME_CORE_LEN = strlen(FILENAME_CORE);

    srand( time( NULL ) );

    for (size_t i = 0; i < amountOfFiles; ++i) {

        char* filename = calloc(FILENAME_CORE_LEN + strlen(argv[1]) + 5, sizeof(char));
        snprintf(filename, FILENAME_CORE_LEN + strlen(argv[1]) + 4, "%s%lu", FILENAME_CORE, i);
        if (access(filename, F_OK)==0){
            fprintf(stderr, "File already exists: %s. Generator cannot overwrite files", filename);
            exit(1);
        }

        FILE* fs = fopen(filename, "w");

        size_t buff_size = 1000;
        char* buff = calloc(buff_size, sizeof(char));
        size_t buff_index = 0;
        for (int j = 0; j < amountOfChars; ++j) {
            buff[buff_index] = ALPHABET[rand()%ALPHABET_LEN];
            if (buff_index >= buff_size-1){
                fwrite(buff, sizeof(char), buff_index+1, fs);
                buff_index = -1;
            }
            buff_index++;
        }
        if (buff_index > 0){
            fwrite(buff, sizeof(char), buff_index+1, fs);
        }

        free(buff);

    }

    return 0;
}

bool isNumber(char* value){
    for (int i = 0; i < strlen(value); ++i) {
        if (!isdigit(value[i]))
            return false;
    }
    return true;
}

void printUsage(){
    fprintf(stderr, "Provide: amount_of_files amount_of_chars");
}