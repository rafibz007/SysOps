#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include "char_counter.h"

struct tms tmsStart, tmsEnd;
clock_t clockStart, clockEnd;

void writeRowToReportFile(FILE* fs, const char* action, const char* system, const char* user, const char* real);
void startTimer();
void endTimer(const char* action);

char reportFilename[] = "pomiar_zad_2.txt";
FILE* reportFile;

int main(int argc, char** argv){

    if (argc < 3){
        perror("Provide: char filename");
        exit(1);
    }

    char seekChar;
    char* filename = calloc(strlen(argv[2])+1, sizeof(char ));
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

    reportFile = fopen(reportFilename, "a+");
    if (reportFile == NULL){
        fprintf(stderr, "Could not open file: %s", reportFilename);
        exit(1);
    }

    writeRowToReportFile(reportFile, "Type", "System [s]", "User [s]", "Real [s]");



    startTimer();
    countWithFd(seekChar, filename);
    endTimer("Descriptor");

    startTimer();
    countWithStream(seekChar, filename);
    endTimer("Stream");

    free(filename);
    filename=NULL;


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
