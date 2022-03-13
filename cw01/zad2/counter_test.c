#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "counter.c"

BlockTable* blockTable = NULL;
char OPTIONS[3][15];
int OPTION_LEN = 3;


void printUsage();
bool isValidOption(char*);
bool isNumber(char*);
void printWrongInputExitMessage(char*, char**);


/*arguments expected:
 *  Main table size : size_t
 *  Jobs with arguments:
 *      create_table - size : size_t
 *      wc_files - file_names : char*
 *      remove_block - index : size_t
 *      */
int main(int argc, char* argv[]){
    strcpy(OPTIONS[0], "create_table");
    strcpy(OPTIONS[1], "wc_files");
    strcpy(OPTIONS[2], "remove_block");

    if (argc <= 2){
        printUsage(argv);
        exit(1);
    }

    // check if digit was provided
    if (!isNumber(argv[1])){
        printWrongInputExitMessage("Expected number as a first argument", argv);
        exit(1);
    }
    size_t initialSize = strtol(argv[1], NULL, 10);
    blockTable = createBlockTable(initialSize);

    int argIndex = 2;
    while (argIndex < argc){
        char* currentJob = argv[argIndex];

        if (strcmp(currentJob, OPTIONS[0])==0){ //create_table
            // check if size was provided
            if (argIndex+1>=argc || isValidOption(argv[argIndex+1])){
                fprintf(stderr, "\nError at: %s\n", argv[argIndex+1]);
                printWrongInputExitMessage("Expected argument after create_table", argv);
                exit(1);
            }

            // go to next index, where table_size will be located
            argIndex++;

            // check if digit was provided
            if (!isNumber(argv[argIndex])){
                fprintf(stderr, "\nError at: %s\n", argv[argIndex]);
                printWrongInputExitMessage("Expected number after create_table", argv);
                exit(1);
            }
            size_t size = strtol(argv[argIndex], NULL, 10);
            removeBlockTable(blockTable);
            blockTable = createBlockTable(size);

            printf("Replaced BlockTable with new one with size %lu\n\n", size);

        } else if (strcmp(currentJob, OPTIONS[2])==0){ //remove_block
            // check if index was provided
            if (argIndex+1>=argc || isValidOption(argv[argIndex+1])){
                fprintf(stderr, "\nError at: %s\n", argv[argIndex+1]);
                printWrongInputExitMessage("Expected argument after remove_block", argv);
                exit(1);
            }

            // go to next index, where index will be located
            argIndex++;

            // check if digit was provided
            if (!isNumber(argv[argIndex])){
                fprintf(stderr, "\nError at: %s\n", argv[argIndex]);
                printWrongInputExitMessage("Expected number after remove_block", argv);
                exit(1);
            }
            size_t index = strtol(argv[argIndex], NULL, 10);
            removeBlock(blockTable,index);

            printf("Removed block at index=%lu\n\n", index);

        } else if (strcmp(currentJob, OPTIONS[1])==0){ //wc_files

            // check if filenames were provided
            // if no more arguments were provided, or next argument is not filename, but another option exit the program
            if (argIndex+1>=argc || isValidOption(argv[argIndex+1])){
                fprintf(stderr, "\nError at: %s\n", argv[argIndex+1]);
                printWrongInputExitMessage("Expected filenames after wc_files", argv);
                exit(1);
            }


            size_t filenamesStartIndex = argIndex+1;

            // find last filename index and check amount of files and their total names size
            // argIndex will be set on last filename or the end of arguments
            size_t filenamesLength = 0;
            while (argIndex+1<argc && !isValidOption(argv[argIndex+1])){
                argIndex++;

                filenamesLength += strlen(argv[argIndex]);
            }

            size_t filenamesAmount = argIndex-filenamesStartIndex+2;

            char* filenames = calloc(filenamesLength+filenamesAmount, sizeof(char));
            strcpy(filenames, "");
            for (int i = filenamesStartIndex; i <= argIndex; ++i) {
                strcat(filenames, argv[i]);
                strcat(filenames, " ");
            }

//            countWordsFromFiles(blockTable, filenames);
            size_t index = countWordsFromFiles(blockTable, filenames);
            printf("Performed word counting and saved data in block with index=%lu\n%s\n", index, getBlockData(blockTable, index));

        } else { //invalid job
            fprintf(stderr, "\nError at: %s\n", argv[argIndex]);
            printWrongInputExitMessage("Invalid job provided", argv);
            exit(1);
        }

        argIndex++;
    }


}


bool isValidOption(char* option){
    for (int i = 0; i < OPTION_LEN; ++i) {
        if (strcmp(option, OPTIONS[i])==0)
            return true;
    }
    return false;
}

bool isNumber(char* value){
    for (int i = 0; i < strlen(value); ++i) {
        if (!isdigit(value[i]))
            return false;
    }
    return true;
}

void printWrongInputExitMessage(char* message, char** argv){
    fprintf(stderr, "\n%s\n", message);
    printUsage(argv);
}

void printUsage(char** argv){
    fprintf(stderr, "\nProvide arguments:\n"
                    "  1) Main table size : size_t\n"
                    "  2) Jobs with arguments : list\n"
                    "      create_table - size : size_t\n"
                    "      wc_files - file_names : char*\n"
                    "      remove_block - index : size_t\n"
                    "\nExample usage:\n"
                    "%s 1 wc_files file1 create_table 3 wc_files file2 file3 wc_files file5 remove_block 0 wc_files file1 wc_files file4\n\n",
                    argv[0]
                    );
}