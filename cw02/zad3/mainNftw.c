#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>


struct resultsNftw{
    size_t files;
    size_t dirs;
    size_t charDevs;
    size_t blocks;
    size_t slinks;
    size_t fifos;
    size_t socks;
};
struct resultsNftw* res;

int printAndCountFile(const char *path, const struct stat *sb, int typeflag, struct FTW *ftw);


char cwd[PATH_MAX];
bool providedRelativePath;


int main(int argc, char**argv){
    getcwd(cwd, PATH_MAX);

    res = calloc(1, sizeof(struct resultsNftw));
    res->files = 0;
    res->dirs = 0;
    res->charDevs = 0;
    res->blocks = 0;
    res->slinks = 0;
    res->fifos = 0;
    res->socks = 0;

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

    providedRelativePath = coreDir[0] != '/';



    printf("%6s   %10s   %15s   %25s   %25s   %s\n", "LINKS", "TYPE", "SIZE", "LAST_ACC", "LAST_MOD", "PATH");
    nftw(coreDir, printAndCountFile, 1, 0);
    printf("Files: %lu  Dirs:%lu  CharDevs:%lu  BlockDevs:%lu  SLinks:%lu  FIFOs:%lu  Socks:%lu\n", res->files, res->dirs, res->charDevs, res->blocks, res->slinks, res->fifos, res->socks);


    free(res);
    return 0;
}


int printAndCountFile(const char *path, const struct stat *sb, int typeflag, struct FTW *ftw){

    char type[10];
    switch (sb->st_mode & S_IFMT) {
        case S_IFSOCK:
            res->socks++;
            strcpy(type, "sock");
            break;
        case S_IFLNK:
            res->slinks++;
            strcpy(type, "slink");
            break;
        case S_IFREG:
            res->files++;
            strcpy(type, "file");
            break;
        case S_IFBLK:
            res->blocks++;
            strcpy(type, "block dev");
            break;
        case S_IFDIR:
            res->dirs++;
            strcpy(type, "dir");
            break;
        case S_IFCHR:
            res->charDevs++;
            strcpy(type, "char dev");
            break;
        case S_IFIFO:
            res->fifos++;
            strcpy(type, "fifo");
            break;
        default:
            strcpy(type, "unknown");
            break;
    }

    char* accessDate = ctime(&sb->st_atime);
    char* modifiedDate = ctime(&sb->st_mtime);


    accessDate[strcspn(accessDate, "\n")]=0;
    modifiedDate[strcspn(modifiedDate, "\n")]=0;


    if (providedRelativePath)
        printf("%6lu   %10s   %15ld   %25s   %25s   %s/%s\n", sb->st_nlink, type, sb->st_size, accessDate, modifiedDate, cwd, path);
    else
        printf("%6lu   %10s   %15ld   %25s   %25s   %s\n", sb->st_nlink, type, sb->st_size, accessDate, modifiedDate, path);
    return 0;
}