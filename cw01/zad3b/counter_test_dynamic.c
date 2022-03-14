#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/times.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
//#include "counter.h"


struct tms tmsStart, tmsEnd;
clock_t clockStart, clockEnd;

int OPTION_LEN = 4;
char OPTIONS[4][20];
FILE* reportFile;


void printUsage();
bool isValidOption(char*);
bool isNumber(char*);
void printWrongInputExitMessage(char*, char**);
void writeRowToReportFile(FILE* fs, const char* action, const char* system, const char* user, const char* real);

void startTimer();
void endTimer(const char* action);


int main(int argc, char* argv[]){
    strcpy(OPTIONS[0], "create_table");
    strcpy(OPTIONS[1], "wc_files");
    strcpy(OPTIONS[2], "remove_block");
    strcpy(OPTIONS[3], "add_remove_blocks");


    char* LIB_NAME = "./libcounter.so";
    void *handle = dlopen(LIB_NAME, RTLD_LAZY);
    if(!handle){
        fprintf(stderr, "Could not open %s\n", LIB_NAME);
        exit(1);
    }



    //dynamic library
    typedef struct BlockTable BlockTable;
    typedef struct TmpFile TmpFile;

    BlockTable* (*createBlockTable)(size_t size) = dlsym(handle, "createBlockTable");
    void (*removeBlockTable)(BlockTable* blockTable) = dlsym(handle, "removeBlockTable");

    size_t (*createBlockWithSizeAndData)(BlockTable* blockTable, size_t dataLength, char* data) = dlsym(handle, "createBlockWithSizeAndData");
    void (*removeBlock)(BlockTable* blockTable, size_t blockTableIndex) = dlsym(handle, "removeBlock");

    TmpFile* (*countWordsFromFilesIntoTmpFile)(char* fileNames) = dlsym(handle, "countWordsFromFilesIntoTmpFile");
    size_t (*createBlockFromTmpFileData)(BlockTable *blockTable, TmpFile* file) = dlsym(handle,"createBlockFromTmpFileData");
    void (*removeTmpFile)(TmpFile* tmpFile) = dlsym(handle,"removeTmpFile");



    BlockTable* blockTable = NULL;

    // PARSING FIRST ARGUMENT
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


    // PREPARING REPORT FILE
    const char* REPORT_FILE_NAME = "raport3b.txt";

    int t = system("touch raport3b.txt"); //tmp fix
    if (t==-1){
        perror("Error occurred");
        exit(1);
    }

    // check if file already exist and programme have access rights
    // open file and append header
    // all test will separately start from header file
    bool fileExist = access(REPORT_FILE_NAME, F_OK)==0;
    if (fileExist){
        if (access(REPORT_FILE_NAME, W_OK)!=0){
            fprintf(stderr,"Could not open %s file. No write permission", REPORT_FILE_NAME);
            exit(1);
        }
    }
    reportFile = fopen(REPORT_FILE_NAME, "a");
    if (reportFile == NULL){
        fprintf(stderr,"Could not open %s file", REPORT_FILE_NAME);
        exit(1);
    }

    writeRowToReportFile(reportFile, "Action[:amount/size]", "System [s]", "User [s]", "Real [s]");


    //PARSING JOBS
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

            startTimer();
            removeBlockTable(blockTable);
            endTimer("remove_table");

            char* action = calloc(strlen("create_table:")+ strlen(argv[argIndex]), sizeof(char));
            if (action == NULL)
                exit(1);
            snprintf(action, 30, "create_table:%ld", size);
            startTimer();
            blockTable = createBlockTable(size);
            endTimer(action);

            free(action);

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

            // measure time
            startTimer();
            removeBlock(blockTable,index);
            endTimer("remove_block");


        } else if (strcmp(currentJob, OPTIONS[3])==0) { //add_remove_blocks
            // check if amount was provided
            if (argIndex+1>=argc || isValidOption(argv[argIndex+1])){
                fprintf(stderr, "\nError at: %s\n", argv[argIndex+1]);
                printWrongInputExitMessage("Expected argument after add_remove_blocks", argv);
                exit(1);
            }

            // go to next index, where index will be located
            argIndex++;

            // check if digit was provided
            if (!isNumber(argv[argIndex])){
                fprintf(stderr, "\nError at: %s\n", argv[argIndex]);
                printWrongInputExitMessage("Expected number after add_remove_blocks", argv);
                exit(1);
            }
            size_t amount = strtol(argv[argIndex], NULL, 10);

            size_t* indexes = calloc(amount, sizeof(size_t));
            if (indexes == NULL){
                exit(1);
            }

            char* DATA = "Data for testing : creating and removing blocks";
            size_t DATA_LEN = strlen(DATA);

            //measure time
            char* action = calloc(30, sizeof(char));
            if (action == NULL){
                exit(1);
            }
            snprintf(action, 30, "add_blocks:%ld", amount);

            startTimer();
            for (int i = 0; i < amount; ++i) {
                indexes[i] = createBlockWithSizeAndData(blockTable, DATA_LEN, DATA);
            }
            endTimer(action);


            snprintf(action, 30, "remove_blocks:%ld", amount);
            startTimer();
            for (int i = 0; i < amount; ++i) {
                removeBlock(blockTable, indexes[i]);
            }
            endTimer(action);

            free(indexes);
            free(action);


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

            size_t filenamesAmount = argIndex-filenamesStartIndex+1;

            char* filenames = calloc(filenamesLength+filenamesAmount+1, sizeof(char));
            strcpy(filenames, "");
            for (size_t i = filenamesStartIndex; i <= argIndex; ++i) {
                strcat(filenames, argv[i]);
                strcat(filenames, " ");
            }

            //measure time
            char* action = calloc(30, sizeof(char));
            if (action == NULL)
                exit(1);
            snprintf(action, 30, "count_files:%ld", filenamesAmount);
            startTimer();
            TmpFile* tmpFile = countWordsFromFilesIntoTmpFile(filenames);
            endTimer(action);

            free(action);

            startTimer();
            createBlockFromTmpFileData(blockTable, tmpFile);
            endTimer("read_to_block");


            removeTmpFile(tmpFile);


        } else { //invalid job
            fprintf(stderr, "\nError at: %s\n", argv[argIndex]);
            printWrongInputExitMessage("Invalid job provided", argv);
            exit(1);
        }

        argIndex++;
    }

    dlclose(handle);
    fwrite("\n", sizeof(char), 1, reportFile);
    fclose(reportFile);
    return 0;
}


void startTimer(){
    clockStart = times(&tmsStart);
}
void endTimer(const char* action){
    clockEnd = times(&tmsEnd);

    long clkTics = sysconf(_SC_CLK_TCK);
    long double systemTime = (long double)(tmsEnd.tms_stime - tmsStart.tms_stime)/clkTics;
    long double userTime = (long double)(tmsEnd.tms_utime - tmsStart.tms_utime)/clkTics;
    long double realTime = (long double)(clockEnd - clockStart)/clkTics;

    char* sys = calloc(15, sizeof(char));
    char* usr = calloc(15, sizeof(char));
    char* rel = calloc(15, sizeof(char));

    snprintf(sys, 15, "%Lf", systemTime);
    snprintf(usr, 15, "%Lf", userTime);
    snprintf(rel, 15, "%Lf", realTime);

    writeRowToReportFile(reportFile, action, sys, usr, rel);
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


void writeRowToReportFile(FILE* fs, const char* action, const char* system, const char* user, const char* real){
    fprintf(fs, "%-30s   %10s   %10s   %10s\n", action, system, user, real);
    fprintf(stdout, "%-30s   %10s   %10s   %10s\n", action, system, user, real);
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
                    "      add_remove_blocks - amount : size_t\n"
                    "\nExample usage:\n"
                    "%s 1 \\\n"
                    "\twc_files file1 \\\n"
                    "\tcreate_table 3 \\\n"
                    "\twc_files file2 file3 \\\n"
                    "\twc_files file5 \\\n"
                    "\tremove_block 0 \\\n"
                    "\twc_files file1 \\\n"
                    "\twc_files file4 \\\n"
                    "\tcreate_table 100 \\\n"
                    "\tadd_remove_blocks 100\n\n",
                    argv[0]
                    );
}