#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>

#include "tree_open_dir.h"


int main(int argc, char**argv){

    if (argc < 2){
        perror("Provide: directory");
        exit(1);
    }

    char* coreDir = calloc(MAX_INPUT, sizeof(char));
    if (coreDir == NULL){
        perror("Not enough memory");
        exit(1);
    }

    strcpy(coreDir, argv[1]);

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

    switch (stats->st_mode & S_IFDIR) {
        case S_IFDIR:
            break;
        default:
            perror("Provide directory");
            exit(1);
            break;
    }

    treeWithOpenDir(coreDir);

    return 0;
}



