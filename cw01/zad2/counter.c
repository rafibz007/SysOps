#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "counter.h"

// MANAGING BLOCK TABLES
BlockTable* createBlockTable(size_t size){
    if (size == 0)
        size++;

    BlockTable* blockTable = calloc(1, sizeof(BlockTable));
    blockTable->size = size;
    blockTable->dataBlocks = calloc(size, sizeof(Block*));
    if (blockTable->dataBlocks == NULL){
        exit(1);
    }

    return blockTable;
}

void removeBlockTable(BlockTable* blockTable){
    for (int i = 0; i < blockTable->size; ++i) {
        if (blockTable->dataBlocks[i] != NULL){
            free(blockTable->dataBlocks[i]->data);
        }
        free(blockTable->dataBlocks[i]);
    }
    free(blockTable);
}



// MANAGING BLOCKS
size_t createBlock(BlockTable* blockTable){
    Block* block = calloc(1, sizeof(Block));
    if (block == NULL){
        removeBlockTable(blockTable);
        exit(1);
    }
    block->hasValidData = false;
    block->size = 0;
    block->data = NULL;

//    look for empty index in array
    size_t index;
    bool indexFound = false;
    for (int i = 0; i < blockTable->size; ++i) {
        if (blockTable->dataBlocks[i] == NULL){
            index = i;
            indexFound = true;
            break;
        }
    }

//    array to small, resize it making it twice as big
    if (!indexFound){
        Block** buffer = blockTable->dataBlocks;
        blockTable->dataBlocks = realloc(blockTable->dataBlocks, blockTable->size*2*sizeof(Block*));
        if (blockTable->dataBlocks == NULL){
            blockTable->dataBlocks = buffer;
            removeBlockTable(blockTable);
            exit(1);
        }
        indexFound = true;
        index = blockTable->size;
        blockTable->size*=2;
    }

//    assign block to index
    blockTable->dataBlocks[index]=block;
    return index;
}


size_t createBlockWithSize(BlockTable* blockTable, size_t dataLength){
    dataLength++;
    size_t index = createBlock(blockTable);
    Block* block = blockTable->dataBlocks[index];
    assert(block != NULL);
    block->data = calloc(dataLength, sizeof(char));
    if (block->data == NULL){
        removeBlockTable(blockTable);
        exit(1);
    }
    block->size = dataLength;

    return index;
}

size_t createBlockWithSizeAndData(BlockTable* blockTable, size_t dataLength, char* data){
    size_t index = createBlockWithSize(blockTable, dataLength);
    setBlockData(blockTable, index, data);

    return index;
}

void setBlockData(BlockTable* blockTable, size_t blockTableIndex, char* data){
    Block* block = blockTable->dataBlocks[blockTableIndex];
    assert(block != NULL);

    size_t dataLength = strlen(data)+1;

    free(block->data);
    block->data = calloc(dataLength, sizeof(char));
    if (block->data == NULL){
        removeBlockTable(blockTable);
        exit(1);
    }

    strcpy(block->data, data);
    block->hasValidData = true;
    block->size = dataLength;
}

void removeBlockData(BlockTable* blockTable, size_t blockTableIndex){
    Block* block = blockTable->dataBlocks[blockTableIndex];
    assert(block != NULL);

    free(block->data);
    block->data = NULL;
    block->hasValidData = false;
}

void removeBlock(BlockTable* blockTable, size_t blockTableIndex){
    Block* block = blockTable->dataBlocks[blockTableIndex];
    if (block == NULL)
        return;
    free(block->data);
    free(block);

    blockTable->dataBlocks[blockTableIndex] = NULL;
}

char* getBlockData(BlockTable* blockTable, size_t blockTableIndex){
    Block* block = blockTable->dataBlocks[blockTableIndex];
    assert(block != NULL);
    if (block->hasValidData){
        char* buffer = calloc(strlen(block->data), sizeof(char));
        strcpy(buffer, block->data);
        return buffer;
    }
    return NULL;
}


// MANAGING TEMPORARY FILE
TmpFile* createTmpFile(){
    char fileName[] = "/tmp/tmpFile.XXXXXX";
    int fd = mkstemp(fileName);
    if (fd == -1){
        exit(1);
    }

    TmpFile* tmpFile = calloc(1,sizeof(TmpFile));
    tmpFile->filename = calloc(strlen(fileName)+2, sizeof(char));
    strcpy(tmpFile->filename, fileName);
    tmpFile->fd=fd;

    return tmpFile;
}

size_t tmpFileContentLength(TmpFile* tmpFile){
    off_t size = lseek(tmpFile->fd, 0, SEEK_END);
    lseek(tmpFile->fd, 0, SEEK_SET);
    return size/sizeof(char);
}

char* getTmpFileContent(TmpFile* tmpFile){


    size_t contentLength = tmpFileContentLength(tmpFile);
    char* buffer = calloc(contentLength+1, sizeof(char));
    if (buffer == NULL){
        exit(1);
    }

    lseek(tmpFile->fd, 0, SEEK_SET);
    read(tmpFile->fd,buffer, contentLength*sizeof(char));
    lseek(tmpFile->fd, 0, SEEK_SET);

    return buffer;
}

void removeTmpFile(TmpFile* tmpFile){
    closeFd(tmpFile);
    remove(tmpFile->filename);
    free(tmpFile->filename);
    free(tmpFile);
}

void openFd(TmpFile* tmpFile){
    closeFd(tmpFile);
    File fd = open(tmpFile->filename, O_RDWR);
    if (fd == -1){
        fprintf(stderr, "Error while opening file...");
        exit(1);
    }
    tmpFile->fd=fd;
}

void closeFd(TmpFile* tmpFile){
    if (tmpFile->fd != -1){
        close(tmpFile->fd);
        tmpFile->fd=-1;
    }
}



// WORD COUNTING
size_t createBlockFromTmpFileData(BlockTable *blockTable, TmpFile* file){
    size_t contentLength = tmpFileContentLength(file);
    char* buffer = calloc(contentLength+1, sizeof(char));
    strcpy(buffer, getTmpFileContent(file));
    size_t index = createBlockWithSizeAndData(blockTable, contentLength, buffer);
    free(buffer);
    return index;
}

TmpFile* countWordsFromFilesIntoTmpFile(char* fileNames){
    TmpFile* tmpFile = createTmpFile();
    size_t tmpFileNameLength = strlen(tmpFile->filename);

    size_t filesStringLength = strlen(fileNames);

    char* command = calloc(filesStringLength + tmpFileNameLength + 5, sizeof(char));
    if (command == NULL){
        exit(1);
    }

    strcpy(command, "wc ");
    strcat(command, fileNames);
    strcat(command, ">");
    strcat(command, tmpFile->filename);

    system(command);
    if (errno != 0){
        fprintf (stderr, "error opening file: %s\n", strerror (errno));
        exit(1);
    }
    return tmpFile;
}

size_t countWordsFromFiles(BlockTable* blockTable, char* fileNames){
    TmpFile* tmpFile = countWordsFromFilesIntoTmpFile(fileNames);
    size_t index = createBlockFromTmpFileData(blockTable, tmpFile);
    removeTmpFile(tmpFile);
    return index;
}