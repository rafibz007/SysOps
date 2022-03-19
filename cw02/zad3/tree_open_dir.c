#include "tree_open_dir.h"

void treeWithOpenDir(char* dirName){
    struct resultsOpenDir* res = visitAndCountWithOpenDir(dirName, 0);
    printf("Files: %lu  Dirs:%lu  CharDevs:%lu  BlockDevs:%lu  SLinks:%lu  FIFOs:%lu  Socks:%lu\n", res->files, res->dirs, res->charDevs, res->blocks, res->slinks, res->fifos, res->socks);
}

void printFileData(char* path, struct stat* statistics, struct dirent* entry){
    assert(statistics!=NULL);
    assert(entry!=NULL);

    char type[10];
    switch (statistics->st_mode & S_IFMT) {
        case S_IFSOCK:
            strcpy(type, "sock");
            break;
        case S_IFLNK:
            strcpy(type, "slink");
            break;
        case S_IFREG:
            strcpy(type, "file");
            break;
        case S_IFBLK:
            strcpy(type, "block dev");
            break;
        case S_IFDIR:
            strcpy(type, "dir");
            break;
        case S_IFCHR:
            strcpy(type, "char dev");
            break;
        case S_IFIFO:
            strcpy(type, "fifo");
            break;
        default:
            strcpy(type, "unknown");
            break;
    }

//    %6s   %10s   %15s   %15s   %15s   %s
    printf("%6lu   %10s   %15ld   %15ld   %15ld   %s\n", statistics->st_nlink, type, statistics->st_size, statistics->st_atim.tv_sec, statistics->st_mtim.tv_sec, path);
}

struct resultsOpenDir* visitAndCountWithOpenDir(char* nextDir, size_t depth){

    DIR* dir = opendir(nextDir);
    if (dir==NULL){
        fprintf(stderr, "Directory not found or no permissions: %s", nextDir);
        exit(1);
    }

    chdir(nextDir);

    struct dirent* entry;
    struct stat* statistics = calloc(1, sizeof(struct stat));


    if (statistics==NULL){
        perror("Not enough memory");
        exit(1);
    }


    char cwd[PATH_MAX+1];
    char path[2*PATH_MAX+1];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("Error getting current working directory");
        exit(1);
    }

    if (depth==0){
        printf("%6s   %10s   %15s   %15s   %15s   %s\n", "LINKS", "TYPE", "SIZE", "LAST_ACC", "LAST_MOD", "PATH");
    }

    struct resultsOpenDir* res = calloc(1, sizeof(struct resultsOpenDir));
    res->files=0;
    res->blocks=0;
    res->fifos=0;
    res->charDevs=0;
    res->slinks=0;
    res->socks=0;
    res->dirs=0;

    struct resultsOpenDir* chRes;
    entry = readdir(dir);
    while (entry){
        int s = stat(entry->d_name, statistics);
        if (s==-1){
            fprintf(stderr, "Error reading stat from file: %s\n", entry->d_name);
            exit(1);
        }


        switch (statistics->st_mode & S_IFMT) {
            case S_IFSOCK:
                res->socks++;
                break;
            case S_IFLNK:
                res->slinks++;
                break;
            case S_IFREG:
                res->files++;
                break;
            case S_IFBLK:
                res->blocks++;
                break;
            case S_IFDIR:
                res->dirs++;
                break;
            case S_IFCHR:
                res->charDevs++;
                break;
            case S_IFIFO:
                res->fifos++;
                break;
            default:
                break;
        }



        snprintf(path, 2*PATH_MAX, "%s/%s", cwd, entry->d_name);
        printFileData(path, statistics, entry);


        if (strcmp(entry->d_name,".")!=0 &&
            strcmp(entry->d_name,"..")!=0 &&
            !(statistics->st_mode & S_IFLNK) &&
            statistics->st_mode & S_IFDIR){

            chRes = visitAndCountWithOpenDir(entry->d_name, depth + 1);
            res->files+=chRes->files;
            res->blocks+=chRes->blocks;
            res->fifos+=chRes->fifos;
            res->charDevs+=chRes->charDevs;
            res->slinks+=chRes->slinks;
            res->socks+=chRes->socks;
            res->dirs+=chRes->dirs;

            free(chRes);
            chRes = NULL;
        }


        entry = readdir(dir);
    }

    free(entry);
    free(statistics);
    closedir(dir);
    chdir("..");

    return res;
}