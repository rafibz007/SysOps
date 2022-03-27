#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]){

    if (argc < 2){
        fprintf(stderr, "Provide 'n'");
        exit(1);
    }

    long int n = strtol(argv[1], NULL, 10);
    char* arg[1];
    arg[0] = NULL;

    printf("Hi, I'm main with PID: %d\n", getpid());
    for (int i = 0; i < n; ++i) {
        if (vfork() == 0){
            execvp("./child", arg);
        }
    }

    int status = 0;
    while (wait(&status)>0);

    return 0;
}