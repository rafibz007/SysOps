#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]){

    printf("Hi, I'm child with PID: %d\n", getpid());

    return 0;

}