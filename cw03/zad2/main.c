#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <stdbool.h>

struct tms tmsStart, tmsEnd;
clock_t clockStart, clockEnd;

void startTimer();
void endTimer(const char* action);

void writeRowToReportFile(FILE* fs, const char* action, const char* system, const char* user, const char* real);

char reportFilename[] = "raport.txt";
FILE* reportFile;


int main(int argc, char* argv[]){

    if (argc < 3){
        fprintf(stderr, "Provide: step amount\n");
        exit(1);
    }

    bool addHeader = false;
    if (access(reportFilename, F_OK)!=0)
        addHeader = true;

    reportFile = fopen(reportFilename, "a+");
    if (reportFile == NULL){
        fprintf(stderr, "Could not open file: %s", reportFilename);
        exit(1);
    }

    if (addHeader)
        writeRowToReportFile(reportFile, "Options", "System [s]", "User [s]", "Real [s]");


    char* action = calloc(strlen(argv[1])+strlen(argv[2])+3, sizeof(char));
    if (action==NULL){
        perror("Could not allocate memory");
        exit(1);
    }

    snprintf(action, strlen(argv[1])+strlen(argv[2])+2, "%s %s", argv[1], argv[2]);

    startTimer();

    size_t amount = strtol(argv[2], NULL, 10);

    char* arg[6];
    for (int i = 0; i < 6; ++i) {
        arg[i] = calloc(sizeof(char), strlen(argv[1])+ strlen(argv[2]));
    }

    strcpy(arg[0], "./child");
    strcpy(arg[3], argv[1]);
    arg[5]=NULL;

    double start = 0, in_start = 0;
    double end, in_end = 1;

    double step = (in_end-in_start)/(double)amount;
    end = step;

    for (int i = 1; i <= amount; ++i) {
        snprintf(arg[1], 50, "%f", start);
        snprintf(arg[2], 50, "%f", end);
        snprintf(arg[4], strlen(argv[2])+1, "%d", i);

        if (vfork()==0){
            execvp("./child", arg);
        }
        start+=step;
        end+=step;
    }

    int status = 0;
    while (wait(&status)>0);

    char* filename = calloc(strlen(argv[2])+7,sizeof(char));
    char* value = calloc(51, sizeof(char));
    double sum = 0;
    FILE* file;
    for (int i = 1; i <= amount; ++i) {
        snprintf(filename, strlen(argv[2])+6, "w%d.txt", i);
        file = fopen(filename, "r");
        if (file == NULL){
            perror("Error opening file");
            exit(1);
        }

        fread(value, sizeof(char), 50, file);

        sum += strtod(value, NULL);
        fclose(file);
    }

    printf("%f\n", sum);

    endTimer(action);
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