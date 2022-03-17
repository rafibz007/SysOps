#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 256
#define BUFF_SIZE 5

void copyWithDescriptors(char* fromFilename, char* toFilename);
void copyWithStreams(char* fromFilename, char* toFilename);

int main(int argc, char** argv){
    char* fromFilename = calloc(101, sizeof(char));
    char* toFilename = calloc(101, sizeof(char));

    if(fromFilename == NULL){
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }
    if(toFilename == NULL){
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }


//    read/save first filename
    if (argc <= 1){
        int s = scanf("%100s", fromFilename);
        if (s <= 0){
            fprintf(stderr, "Error reading from input");
        }
    } else {
        strcpy(fromFilename, argv[1]);
    }

//    check first file
    if(access(fromFilename, F_OK) != 0){
        fprintf(stderr, "File do not exists: %s", fromFilename);
        exit(1);
    } else if(access(fromFilename, R_OK) != 0) {
        fprintf(stderr, "No read permissions: %s", fromFilename);
        exit(1);
    }


//    read/save second filename
    if (argc <= 2){
        int s = scanf("%100s", toFilename);
        if (s <= 0){
            fprintf(stderr, "Error reading from input");
        }
    } else {
        strcpy(toFilename, argv[2]);
    }

//    check second file
    if(access(toFilename, F_OK) != 0){
        fprintf(stderr, "File do not exists: %s", toFilename);
        exit(1);
    } else if(access(toFilename, W_OK) != 0) {
        fprintf(stderr, "No read permissions: %s", toFilename);
        exit(1);
    }

    printf("%s %s", fromFilename, toFilename);
    copyWithDescriptors(fromFilename, toFilename);


    return 0;
}

void copyWithDescriptors(char* fromFilename, char* toFilename){
    int fromFd = open(fromFilename, O_RDONLY);
    int toFd = open(toFilename, O_WRONLY);

    if (fromFd == -1){
        fprintf(stderr,"Error while opening a file: %s", fromFilename);
        exit(1);
    } else if (toFd == -1) {
        fprintf(stderr,"Error while opening a file: %s", toFilename);
        exit(1);
    }

    char* buff = calloc(BUFF_SIZE+1, sizeof(char));
    char* tmpBuff = calloc(BUFF_SIZE+1, sizeof(char));

    char filename[] = "/tmp/tmpFile.XXXXXX";
    int tmpFd = mkstemp(filename);
    if (tmpFd == -1 || buff == NULL || tmpBuff == NULL){
        perror("Error occurred...");
        exit(1);
    }
    unlink(filename);


    ssize_t len;
    size_t i;
    size_t lineLength;
    char* lineStart;
    bool foundCharInLine = false;
    bool reachedEOF = false;
    bool tmpFileHasLineData = false;
    size_t tmpContentLength = 0;
    while (!reachedEOF){

        len = read(fromFd, buff, BUFF_SIZE* sizeof(char));
        if (len < BUFF_SIZE){
            reachedEOF = true;
        }

//        read from file into buffer BUFF_SIZE chars in a loop, parse input marking start of line and looking for end of line for each line
//        if during this check non-white-char was found, save this line into file, if not, skip it
//        if not whole line was loaded in the buffer it need to be parsed differently.
//        If in this part of line, non-white-char was found, write rest of buff to file, but keep foundCharInLine true,
//        to allow rest of line to be loaded
//        If in this part of line some white-chars only were loaded, save them in tmp file, and load next chunk of data into
//        buffer and if non-white-char before new line was found, save tmp file content and buffer line


        lineLength = 0;
        lineStart = buff;
        for (i = 0; *(buff+i) && i < len; ++i) {

            lineLength += 1;
            if (*(buff+i)=='\n'){

                if (foundCharInLine){

                    if (tmpFileHasLineData){
                        lseek(tmpFd, 0, SEEK_SET);
                        read(tmpFd, tmpBuff, tmpContentLength*sizeof(char));
                        write(toFd, tmpBuff, tmpContentLength);
                    }

                    write(toFd, lineStart, lineLength);
                }

                lineLength = 0;
                lineStart = buff+i+1;
                foundCharInLine = false;

                tmpFileHasLineData = false;
                tmpContentLength = 0;

            } else if (!isspace(*(buff+i))){
                foundCharInLine = true;
            }

        }

        if (foundCharInLine){

            if (tmpFileHasLineData){
                lseek(tmpFd, 0, SEEK_SET);
                read(tmpFd, tmpBuff, tmpContentLength*sizeof(char));
                write(toFd, tmpBuff, tmpContentLength);
                tmpFileHasLineData = false;
                tmpContentLength = 0;
            }

            write(toFd, lineStart, lineLength);

        } else {

//            save progress into tmp file and add it if this line continuation has chars
            tmpFileHasLineData = true;
            tmpContentLength += lineLength;
            write(tmpFd, lineStart, lineLength);

        }



    }

    close(tmpFd);
    close(toFd);
    close(fromFd);
    free(buff);
    free(tmpBuff);

}


void copyWithStreams(char* fromFilename, char* toFilename){}