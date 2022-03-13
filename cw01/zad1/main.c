#include <stdio.h>
#include "counter.c"

int main(){
    printf("Hello world\n");
    BlockTable* blockTable = createBlockTable(1);

//    does same thing as 3 lines under
//    size_t index = countWordsFromFiles(blockTable,"counter.h counter.c");
    TmpFile* tmpFile = countWordsFromFilesIntoTmpFile("counter.h counter.c");
    size_t index = createBlockFromTmpFileData(blockTable, tmpFile);
    removeTmpFile(tmpFile);


    printf("Index: %d\n", (int)index);
    printf("Data:\n%s", getBlockData(blockTable, index));
    printf("Block: %s\n", (char*)blockTable->dataBlocks[index]);
    removeBlock(blockTable, index);
    printf("Block: %s\n", (char*)blockTable->dataBlocks[index]);
    size_t index2 = createBlock(blockTable);
    index2 = createBlock(blockTable);
    printf("Index: %d\n", (int)index2);

}