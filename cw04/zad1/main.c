#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define IGNORE "ignore"
#define HANDLER "handler"
#define MASK "mask"
#define PENDING "pending"

//struct sigaction act;
//sigemptyset(&act.sa_mask);
//sigaddset(&act.sa_mask, SIGUSR1);
//act.sa_flags = SIGUSR1;
//sigaction(SIGUSR1, &act, NULL);

void handler(int sig_no) {
    printf("%d: Received signal: %d.\n", getpid(), sig_no);
}

int main(int argc, char* argv[]) {

    if (argc<2){
        fprintf(stderr, "Provide: action [ignore/handler/mask/pending]\n");
        exit(1);
    }

    char* action = argv[1]; // not copying, just taking pointer

//    raise(SIGUSR2);

    printf("%d: Main start\n", getpid());

    if (strcmp(action, IGNORE)==0){
        signal(SIGUSR1, SIG_IGN);

        if (vfork()==0){
            execl("child", "child",action, NULL);
            exit(0);
        }
        if (fork()==0){
            printf("%d: Fork start\n", getpid());
            raise(SIGUSR1);
            printf("%d: Fork end\n", getpid());
            exit(0);
        }


        raise(SIGUSR1);


    } else if (strcmp(action, HANDLER)==0){
        signal(SIGUSR1, handler);

        if (vfork()==0){
            execl("child", "child",action, NULL);
            exit(0);
        }
        if (fork()==0){
            printf("%d: Fork start\n", getpid());
            raise(SIGUSR1);
            printf("%d: Fork end\n", getpid());
            exit(0);
        }

        raise(SIGUSR1);


    } else if (strcmp(action, MASK)==0){
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigset, NULL);


        if (vfork()==0){
            execl("child", "child",action, NULL);
            exit(0);
        }
        if (fork()==0){
            printf("%d: Fork start\n", getpid());
            raise(SIGUSR1);

            sigset_t set;
            sigpending(&set);


            printf("%d: Fork blocked: %s\n", getpid(), sigismember(&set, SIGUSR1) ? "true" : "false");

            printf("%d: Fork end\n", getpid());
            exit(0);
        }


        raise(SIGUSR1);
        sigset_t set;
        sigpending(&set);
        printf("%d: Main blocked: %s\n", getpid(), sigismember(&set, SIGUSR1) ? "true" : "false");



    } else if (strcmp(action, PENDING)==0){

        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_BLOCK, &sigset, NULL);


        raise(SIGUSR1);

        sigset_t set;
        sigpending(&set);
        printf("%d: Main has pending: %s\n", getpid(), sigismember(&set, SIGUSR1) ? "true" : "false");


        if (vfork()==0){
            execl("child", "child",action, NULL);
            exit(0);
        }

        if (fork()==0){
            printf("%d: Fork start\n", getpid());

            sigpending(&set);

            printf("%d: Fork has pending: %s\n", getpid(), sigismember(&set, SIGUSR1) ? "true" : "false");

            printf("%d: Fork end\n", getpid());
            exit(0);
        }

    } else {
        fprintf(stderr, "Provide: action [ignore/handler/mask/pending]\n");
        exit(1);
    }

    int status;


    printf("%d: Main end\n", getpid());

    while(wait(&status) > 0);

    return 0;
}