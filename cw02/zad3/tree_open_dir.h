#ifndef ZAD3_TREE_OPEN_DIR_H
#define ZAD3_TREE_OPEN_DIR_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

struct resultsOpenDir{
    size_t files;
    size_t dirs;
    size_t charDevs;
    size_t blocks;
    size_t slinks;
    size_t fifos;
    size_t socks;
};

struct resultsOpenDir* visitAndCountWithOpenDir(char* nextDir, size_t depth);
void treeWithOpenDir(char* dirName);

#endif //ZAD3_TREE_OPEN_DIR_H
