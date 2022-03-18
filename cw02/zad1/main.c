#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include "file_joiner.h"

struct tms tmsStart, tmsEnd;
clock_t clockStart, clockEnd;

void writeRowToReportFile(FILE* fs, const char* action, const char* system, const char* user, const char* real);
void startTimer();
void endTimer(const char* action);

char reportFilename[] = "pomiar_zad_1.txt";
FILE* reportFile;

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

//    if (access(reportFilename, F_OK)!=0){
//        creat()
//    } else if (access(reportFilename, W_OK)!=0){
//
//    }

    reportFile = fopen(reportFilename, "a+");
    if (reportFile == NULL){
        fprintf(stderr, "Could not open file: %s", reportFilename);
        exit(1);
    }

    writeRowToReportFile(reportFile, "Type", "System [s]", "User [s]", "Real [s]");

    startTimer();
    copyWithDescriptors(fromFilename, toFilename);
    endTimer("Descriptors");

    startTimer();
    copyWithStreams(fromFilename, toFilename);
    endTimer("Streams");

    return 0;
}


void writeRowToReportFile(FILE* fs, const char* action, const char* system, const char* user, const char* real){
    fprintf(fs, "%-30s   %10s   %10s   %10s\n", action, system, user, real);
    fprintf(stdout, "%-30s   %10s   %10s   %10s\n", action, system, user, real);
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

    char* sys = calloc(16, sizeof(char));
    char* usr = calloc(16, sizeof(char));
    char* rel = calloc(16, sizeof(char));

    snprintf(sys, 15, "%Lf", systemTime);
    snprintf(usr, 15, "%Lf", userTime);
    snprintf(rel, 15, "%Lf", realTime);

    writeRowToReportFile(reportFile, action, sys, usr, rel);

    free(sys);
    free(usr);
    free(rel);
}