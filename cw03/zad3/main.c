#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>


void printData();



int main(int argc, char* argv[]){

//    PARSE INPUT
    if (argc < 4){
        fprintf(stderr, "Provide: directory string depth\n");
        exit(1);
    }

    char* coreDir = argv[1];
    char* string = argv[2];
    size_t depth = strtol(argv[3], NULL, 10);
    if (depth <= 0){
        exit(1);
    }

    struct dirent* entry;
    struct stat* stats = calloc(1, sizeof(struct stat));
    if (stats==NULL){
        perror("Not enough memory");
        exit(1);
    }

    int s = lstat(coreDir, stats);
    if (s==-1){
        fprintf(stderr, "Error reading stat from file: %s\n", coreDir);
        exit(1);
    }

    if ((stats->st_mode & S_IFMT) != S_IFDIR){
        fprintf(stderr,"%s is not a directory", coreDir);
        exit(1);
    }


    DIR* dir = opendir(coreDir);
    if (dir==NULL){
        fprintf(stderr, "Directory not found or no permissions: %s", coreDir);
        exit(1);
    }


//    START SEARCHING
    chdir(coreDir);
    depth--;
    printf("%s %s\n", argv[0], coreDir);

    char* path = calloc(PATH_MAX, sizeof(char));
    if (path == NULL){
        perror("Not enough memory");
        exit(1);
    }
    strcpy(path, coreDir);
    strcat(path, "/");

    while ((entry=readdir(dir))){

        s = lstat(entry->d_name, stats);
        if (s==-1){
            fprintf(stderr, "Error reading stat from file: %s\n", entry->d_name);
            continue;
        }

        if (strcmp(entry->d_name, "..")==0 || strcmp(entry->d_name, ".")==0){
            continue;
        }

        int ret;
        char* command;

//        printf("%d-%s\n", getpid(), entry->d_name);
        switch (stats->st_mode & S_IFMT) {
            case S_IFREG:
//                IF FILE EXECUTABLE
                if (stats->st_mode & S_IXUSR ||
                    stats->st_mode & S_IXGRP ||
                    stats->st_mode & S_IXOTH){
                    break;
                }

//                CHECK IF FILE CONTAIN STRING
                command = calloc(4+ strlen(entry->d_name)+10+ strlen(string)+2, sizeof(char));
                strcpy(command, "cat ");
                strcat(command, entry->d_name);
                strcat(command, " | grep -q ");
                strcat(command, string);
                ret = system(command);
                if (WEXITSTATUS(ret) == 0){
                    printf("%d found: %s%s\n", getpid(), path,entry->d_name);
                    printData();
                }
                free(command);
                command = NULL;
                break;
            case S_IFDIR:
                if(depth>0 && fork()==0){
//                    ENTER NEW DIRECTORY AND SEARCH IT USING SAME CODE
                    closedir(dir);
                    dir = opendir(entry->d_name);
                    if (dir==NULL){
                        fprintf(stderr, "Directory not found or no permissions: %s", coreDir);
                        break;
                    }
                    chdir(entry->d_name);
                    strcat(path, entry->d_name);
                    strcat(path, "/");
                    depth--;
                }
                break;
            default:
                break;
        }


    }

    free(entry);
    free(path);
    free(stats);
    closedir(dir);
    chdir("..");

    return 0;
}

void printData(){}