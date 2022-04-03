#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define KILL "KILL"
#define SIGQUEUE "SIGQUEUE"
#define SIGRT "SIGRT"

bool isNumber(char* value);

pid_t sender = 0;
pid_t catcher = 0;

bool CATCHER_UP = false;

void sigint_handler(int sig_no) {
    printf("Killing children and exiting...\n");

    if (sender > 0)
        kill(sender, SIGKILL);
    if (catcher > 0)
        kill(catcher, SIGKILL);

    exit(1);

}

void sigusr1_handler(int sig_no) {
    CATCHER_UP = true;
}

int main(int argc, char* argv[]) {
    
    if (argc<3 ||
        (strcmp(argv[2], KILL)!=0 &&
            strcmp(argv[2], SIGQUEUE)!=0 &&
            strcmp(argv[2], SIGRT)!=0) ||
            !isNumber(argv[1])){
        fprintf(stderr, "Provide: amount type[KILL/SIGQUEUE/SIGRT]\n");
        exit(1);
    }

    char* action;
    if ((action = calloc(10, sizeof(char)))==NULL)
        perror("Error allocating memory");

    strcpy(action, argv[2]);
    size_t amount = strtol(argv[1], NULL, 10);


//    if main will receive SIGINT, will kill his children and exit
    signal(SIGINT, sigint_handler);

//    SIGUSR1 will ensure main that catcher is ready and sender can be started
    signal(SIGUSR1, sigusr1_handler);



//    start catcher
    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);

    if ((catcher = vfork()) == 0){
        execl("./catcher", "./catcher", action, NULL);
    }
    int signal;
    printf("Waiting for catcher to start...\n");
    sigwait(&sigset, &signal);

//    start sender


    if ((sender = vfork()) == 0){
        char** argvc = calloc(5, sizeof(char*));
        argvc[0] = "./sender";

        char catcher_str[50];
        char amount_str[50];

        snprintf(catcher_str, 49, "%d", catcher);
        snprintf(amount_str, 49, "%zu", amount);

        argvc[1] = catcher_str;
        argvc[2] = amount_str;

        argvc[3] = action;
        argvc[4] = NULL;

        execv("./sender", argvc);
    }


    int status;
    while(wait(&status) > 0);

    return 0;
}

bool isNumber(char* value){
    for (int i = 0; i < strlen(value); ++i) {
        if (!isdigit(value[i]))
            return false;
    }
    return true;
}
