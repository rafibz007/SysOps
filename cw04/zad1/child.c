#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define IGNORE "ignore"
#define HANDLER "handler"
#define MASK "mask"
#define PENDING "pending"

int main(int argc, char* argv[]) {

    if (argc<2){
        fprintf(stderr, "Provide: action [ignore/handler/mask/pending]");
        exit(1);
    }

    char* action = argv[1]; // not copying, just taking pointer

    printf("%d: Exec start\n", getpid());

    if (strcmp(action, IGNORE)==0){

        raise(SIGUSR1);

    } else if (strcmp(action, HANDLER)==0){

        raise(SIGUSR1);


    } else if (strcmp(action, MASK)==0){

        raise(SIGUSR1);

        sigset_t set;
        sigpending(&set);

        printf("%d: Exec blocked: %s\n", getpid(), sigismember(&set, SIGUSR1) ? "true" : "false");



    } else if (strcmp(action, PENDING)==0){

        sigset_t set;
        sigpending(&set);

        printf("%d: Exec has pending: %s\n", getpid(), sigismember(&set, SIGUSR1) ? "true" : "false");


    } else {
        fprintf(stderr, "Provide: action [ignore/handler/mask/pending]\n");
        exit(1);
    }

    printf("%d: Exec end\n", getpid());
    return 0;
}