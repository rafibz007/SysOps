#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <pthread.h>

#define NUMBERS "numbers"
#define BLOCKS "block"
#define BLOCK_TYPE 0
#define NUMBER_TYPE 1

size_t threadsAmount;
int type;
char *inputFilename;
char *outputFilename;

char header[3];
int W;
int H;
int M;
int** image;

void* blockFunction(void* arg);
void* numbersFunction(void* arg);

int main(int argc, char* argv[]) {
    if (argc < 5) {
        fprintf(stderr, "Provide: <threads_amount> <type: block/numbers> <input file> <output file>\n");
        exit(1);
    }


//    PARSE INPUT
    threadsAmount = strtol(argv[1], NULL, 10);
    if (strcmp(argv[2], BLOCKS)==0)
        type = BLOCK_TYPE;
    else if (strcmp(argv[2], NUMBERS)==0)
        type = NUMBER_TYPE;
    else {
        fprintf(stderr, "Wrong type provided\n");
        exit(1);
    }
    inputFilename = argv[3];
    outputFilename = argv[4];


//    PREPARE CALCULATIONS
    FILE* fp;
    if ((fp = fopen(inputFilename, "r"))==NULL){
        perror("Error opening input file\n");
        exit(1);
    }

    fscanf(fp, "%s", header);
    fscanf(fp, "%d %d\n%d", &W, &H, &M);

    image = calloc(H, sizeof(int*));
    for (int i = 0; i < H; ++i) {
        image[i] = calloc(W, sizeof(int));
    }


    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            fscanf(fp, "%d", &image[i][j]);
        }
    }

    fclose(fp);

//    RUN THREADS
    pthread_t *pthreads;
    if ((pthreads = calloc(threadsAmount, sizeof(pthread_t)))==NULL){
        perror("Error allocating memory\n");
        exit(1);
    }

    int *thread_no;
    if ((thread_no = calloc(threadsAmount, sizeof(int)))==NULL){
        perror("Error allocating memory\n");
        exit(1);
    }

    for (int i = 0; i < threadsAmount; ++i) {
        thread_no[i] = i;
        if (type == BLOCK_TYPE){
            pthread_create(&pthreads[i], NULL, &blockFunction, &thread_no[i]);
        } else if (type == NUMBER_TYPE){
            pthread_create(&pthreads[i], NULL, &numbersFunction, &thread_no[i]);
        }
    }

    for (int i = 0; i < threadsAmount; ++i) {
        if(pthread_join(pthreads[i], NULL) != 0){
            perror("Error while thread join\n");
            exit(1);
        }
    }

    free(pthreads);
    free(thread_no);

    printf("Threads ended execution\n");

//    SAVE IMAGE
    FILE* fpo;
    if ((fpo = fopen(outputFilename, "w+"))==NULL){
        perror("Error opening a file\n");
        exit(1);
    }

    fprintf(fpo, "%s\n%d %d\n%d", header, W, H, M);

    for (int y = 0; y < H; ++y) {
        fprintf(fpo, "\n");
            for (int x = 0; x < W; ++x) {
            fprintf(fpo,"%d ", image[y][x]);
        }
    }
    fclose(fpo);

    for (int i = 0; i < threadsAmount; ++i) {
        free(image[i]);
    }
    free(image);

    return 0;
}

void* blockFunction(void* arg){
    int id = *(int*)arg;
    printf("Block thread %d running\n", id);

    int start = (id)*ceil(W/threadsAmount);
    int end = (id+1)*ceil(W/threadsAmount)-1;

    for (int x = start; x <= end; ++x) {
        for (int y = 0; y < H; ++y) {
            image[y][x] = M-image[y][x];
        }
    }

    return NULL;
}


void* numbersFunction(void* arg){
    int id = *(int*)arg;
    printf("Numbers thread %d running\n", id);

    for (int i = id; i < W*H; i+=threadsAmount) {
        image[i/W][i%W] = M-image[i/W][i%W];
    }

    return NULL;
}