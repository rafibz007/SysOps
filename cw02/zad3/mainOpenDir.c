#include <stdio.h>
#include <stdlib.h>

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

    treeWithOpenDir(coreDir);

    return 0;
}



