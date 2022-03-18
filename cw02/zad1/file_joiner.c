#include "file_joiner.h"

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

//    append to file
    lseek(toFd, 0, SEEK_END);

    char* buff = calloc(BUFF_SIZE+1, sizeof(char));
    char* tmpBuff = calloc(BUFF_SIZE+1, sizeof(char));
    size_t tmpBuffSize = BUFF_SIZE+1;

    char filename[] = "/tmp/tmpFile.XXXXXX";
    int tmpFd = mkstemp(filename);
    if (tmpFd == -1 || buff == NULL || tmpBuff == NULL){
        perror("Error occurred...");
        exit(1);
    }
    unlink(filename);

    bool firstLineAdded = false;
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

            if (*(buff+i)=='\n'){

                if (foundCharInLine){

//                    remove new line when first line appended to file
                    if (!firstLineAdded){

                        if (tmpFileHasLineData){

                            lseek(tmpFd, 0, SEEK_SET);
                            if (tmpBuffSize < tmpContentLength+1){
                                tmpBuffSize = tmpContentLength+1;
                                free(tmpBuff);
                                tmpBuff = calloc(tmpContentLength+1, sizeof(char));
                                if (tmpBuff == NULL){
                                    perror("Error occurred");
                                    exit(1);
                                }
                            }
                            read(tmpFd, tmpBuff, tmpContentLength*sizeof(char));

                            if (tmpBuff[0]=='\n'){
                                write(toFd, tmpBuff+1, tmpContentLength-1);
                            } else {
                                write(toFd, tmpBuff, tmpContentLength);
                            }

                        }
                        if (*lineStart=='\n'){
                            write(toFd, lineStart+1, lineLength-1);
                        } else {
                            write(toFd, lineStart, lineLength);
                        }

                        firstLineAdded = true;
                    } else {
                        if (tmpFileHasLineData)
                            addTmpContentWithFd(tmpFd,toFd,&tmpBuff,tmpContentLength, &tmpBuffSize);

                        write(toFd, lineStart, lineLength);
                    }

                }

                lineLength = 0;
                lineStart = buff+i;
                foundCharInLine = false;

                tmpFileHasLineData = false;
                tmpContentLength = 0;
                lseek(tmpFd, 0, SEEK_SET);

            } else if (!isspace(*(buff+i))){
                foundCharInLine = true;
            }
            lineLength++;

        }

        if (foundCharInLine){

            //                    remove new line when first line appended to file
            if (!firstLineAdded){

                if (tmpFileHasLineData){

                    lseek(tmpFd, 0, SEEK_SET);
                    if (tmpBuffSize < tmpContentLength+1){
                        tmpBuffSize = tmpContentLength+1;
                        free(tmpBuff);
                        tmpBuff = calloc(tmpContentLength+1, sizeof(char));
                        if (tmpBuff == NULL){
                            perror("Error occurred");
                            exit(1);
                        }
                    }
                    read(tmpFd, tmpBuff, tmpContentLength*sizeof(char));

                    if (tmpBuff[0]=='\n'){
                        write(toFd, tmpBuff+1, tmpContentLength-1);
                    } else {
                        write(toFd, tmpBuff, tmpContentLength);
                    }

                    tmpFileHasLineData = false;
                    tmpContentLength = 0;
                    lseek(tmpFd, 0, SEEK_SET);
                }
                if (*lineStart=='\n'){
                    write(toFd, lineStart+1, lineLength-1);
                } else {
                    write(toFd, lineStart, lineLength);
                }

                firstLineAdded = true;
            } else {
                if (tmpFileHasLineData){
                    addTmpContentWithFd(tmpFd,toFd,&tmpBuff,tmpContentLength, &tmpBuffSize);
                    tmpFileHasLineData = false;
                    tmpContentLength = 0;
                    lseek(tmpFd, 0, SEEK_SET);
                }

                write(toFd, lineStart, lineLength);
            }

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

void copyWithStreams(char* fromFilename, char* toFilename){
    FILE* fromFd = fopen(fromFilename, "r");
    FILE* toFd = fopen(toFilename, "a");

    if (fromFd == NULL){
        fprintf(stderr,"Error while opening a file: %s", fromFilename);
        exit(1);
    } else if (toFd == NULL) {
        fprintf(stderr,"Error while opening a file: %s", toFilename);
        exit(1);
    }

//    append to file
    fseek(toFd, 0, SEEK_END);

    char* buff = calloc(BUFF_SIZE+1, sizeof(char));
    char* tmpBuff = calloc(BUFF_SIZE+1, sizeof(char));
    size_t tmpBuffSize = BUFF_SIZE+1;

    FILE* tmpFd = tmpfile();
    if (tmpFd == NULL || buff == NULL || tmpBuff == NULL){
        perror("Error occurred...");
        exit(1);
    }

    bool firstLineAdded = false;
    size_t len;
    size_t i;
    size_t lineLength;
    char* lineStart;
    bool foundCharInLine = false;
    bool reachedEOF = false;
    bool tmpFileHasLineData = false;
    size_t tmpContentLength = 0;
    while (!reachedEOF){

        len = fread(buff, sizeof(char ), BUFF_SIZE, fromFd);
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

            if (*(buff+i)=='\n'){

                if (foundCharInLine){

//                    remove new line when first line appended to file
                    if (!firstLineAdded){

                        if (tmpFileHasLineData){

                            fseek(tmpFd, 0, SEEK_SET);
                            if (tmpBuffSize < tmpContentLength+1){
                                tmpBuffSize = tmpContentLength+1;
                                free(tmpBuff);
                                tmpBuff = calloc(tmpContentLength+1, sizeof(char));
                                if (tmpBuff == NULL){
                                    perror("Error occurred");
                                    exit(1);
                                }
                            }
                            fread(tmpBuff, sizeof(char), tmpContentLength, tmpFd);

                            if (tmpBuff[0]=='\n'){
                                fwrite(tmpBuff+1, sizeof(char ), tmpContentLength-1, toFd);
                            } else {
                                fwrite(tmpBuff, sizeof(char ), tmpContentLength, toFd);
                            }

                        }
                        if (*lineStart=='\n'){
                            fwrite(lineStart+1, sizeof(char ), lineLength-1, toFd);
                        } else {
                            fwrite(lineStart, sizeof(char ), lineLength, toFd);
                        }

                        firstLineAdded = true;
                    } else {
                        if (tmpFileHasLineData)
                            addTmpContentWithStreams(tmpFd,toFd,&tmpBuff,tmpContentLength, &tmpBuffSize);

                        fwrite(lineStart, sizeof(char ), lineLength, toFd);
                    }

                }

                lineLength = 0;
                lineStart = buff+i;
                foundCharInLine = false;

                tmpFileHasLineData = false;
                tmpContentLength = 0;
                fseek(tmpFd, 0, SEEK_SET);

            } else if (!isspace(*(buff+i))){
                foundCharInLine = true;
            }
            lineLength++;

        }

        if (foundCharInLine){

            //                    remove new line when first line appended to file
            if (!firstLineAdded){

                if (tmpFileHasLineData){

                    fseek(tmpFd, 0, SEEK_SET);
                    if (tmpBuffSize < tmpContentLength+1){
                        tmpBuffSize = tmpContentLength+1;
                        free(tmpBuff);
                        tmpBuff = calloc(tmpContentLength+1, sizeof(char));
                        if (tmpBuff == NULL){
                            perror("Error occurred");
                            exit(1);
                        }
                    }

                    fread(tmpBuff, sizeof(char ), tmpContentLength, tmpFd);

                    if (tmpBuff[0]=='\n'){
                        fwrite(tmpBuff+1, sizeof(char ), tmpContentLength-1, toFd);
                    } else {
                        fwrite(tmpBuff, sizeof(char ), tmpContentLength, toFd);
                    }

                    tmpFileHasLineData = false;
                    tmpContentLength = 0;
                    fseek(tmpFd, 0, SEEK_SET);
                }
                if (*lineStart=='\n'){
                    fwrite(lineStart+1, sizeof(char ), lineLength-1, toFd);
                } else {
                    fwrite(lineStart, sizeof(char ), lineLength, toFd);;
                }

                firstLineAdded = true;
            } else {
                if (tmpFileHasLineData){
                    addTmpContentWithStreams(tmpFd,toFd,&tmpBuff,tmpContentLength, &tmpBuffSize);
                    tmpFileHasLineData = false;
                    tmpContentLength = 0;
                    fseek(tmpFd, 0, SEEK_SET);
                }

                fwrite(lineStart, sizeof(char ), lineLength, toFd);
            }

        } else {

//            save progress into tmp file and add it if this line continuation has chars
            tmpFileHasLineData = true;
            tmpContentLength += lineLength;
            fwrite(lineStart, sizeof(char ), lineLength, tmpFd);

        }

    }


    fclose(tmpFd);
    fclose(toFd);
    fclose(fromFd);
    free(buff);
    free(tmpBuff);
}


void addTmpContentWithFd(int tmpFd, int toFd, char** tmpBuff, size_t tmpContentLength, size_t* tmpBuffSize){
    lseek(tmpFd, 0, SEEK_SET);
    if (*tmpBuffSize < tmpContentLength+1){
        *tmpBuffSize = tmpContentLength+1;
        free(*tmpBuff);
        *tmpBuff = calloc(tmpContentLength+1, sizeof(char));
        if (*tmpBuff == NULL){
            perror("Error occurred");
            exit(1);
        }
    }
    read(tmpFd, *tmpBuff, tmpContentLength*sizeof(char));
    write(toFd, *tmpBuff, tmpContentLength);
}

void addTmpContentWithStreams(FILE* tmpFd, FILE* toFd, char** tmpBuff, size_t tmpContentLength, size_t* tmpBuffSize){
    fseek(tmpFd, 0, SEEK_SET);
    if (*tmpBuffSize < tmpContentLength+1){
        *tmpBuffSize = tmpContentLength+1;
        free(*tmpBuff);
        *tmpBuff = calloc(tmpContentLength+1, sizeof(char));
        if (*tmpBuff == NULL){
            perror("Error occurred");
            exit(1);
        }
    }
    fread(*tmpBuff, sizeof(char ), tmpContentLength, tmpFd);
    fwrite(*tmpBuff, sizeof(char ), tmpContentLength, toFd);
}