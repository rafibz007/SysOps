#ifndef COUNTER_H
#define COUNTER_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


struct Block{
    char* data;
    bool hasValidData;
    size_t size;
};
typedef struct Block Block;

struct BlockTable{
    size_t size;
    Block** dataBlocks;
};
typedef struct BlockTable BlockTable;

struct TmpFile{
    char* filename;
    FILE* fd;
};
typedef struct TmpFile TmpFile;



BlockTable* createBlockTable(size_t); //returns new block table object
void removeBlockTable(BlockTable*);

size_t createBlock(BlockTable*); //returns index of newly assigned block
size_t createBlockWithSize(BlockTable*, size_t); //returns index of newly assigned block
size_t createBlockWithSizeAndData(BlockTable*, size_t, char*); //returns index of newly assigned block
void setBlockData(BlockTable*, size_t, char*);
void removeBlockData(BlockTable*, size_t);
void removeBlock(BlockTable*, size_t);
char* getBlockData(BlockTable*, size_t); //returns block data

TmpFile* createTmpFile(); //returns file handler
size_t tmpFileContentLength(TmpFile*); //returns length of tmp file
char* getTmpFileContent(TmpFile*); //returns content of tmp file
void removeTmpFile(TmpFile*);
void openFd(TmpFile*, const char*);
void closeFd(TmpFile*);

size_t createBlockFromTmpFileData(BlockTable*, TmpFile*); //returns index of newly assigned block with data from tmp file
size_t countWordsFromFiles(BlockTable*, char*); //returns counting results
TmpFile* countWordsFromFilesIntoTmpFile(char*); //returns blockTmpFile object of newly opened tmp file with results

#endif