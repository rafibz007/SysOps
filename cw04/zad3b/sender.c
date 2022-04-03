#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>
#include <math.h>

#define KILL "KILL"
#define SIGQUEUE "SIGQUEUE"
#define SIGRT "SIGRT"

bool isNumber(char* value);

int SEND_SIGNAL=SIGUSR1;
int VERIFY_SIGNAL=SIGUSR2;

sigset_t mask;
bool usr_interrupt = false;

bool LISTENING = false;
bool SENDING = false;

size_t RECEIVED_MESSAGES = 0;

pid_t catcherPID;

bool SIGQUEUE_FLAG = false;
size_t AMOUNT_FROM_CATCHER = 0;

size_t max(size_t a, size_t b){return b>a ? b : a;}

void setup_with_sigaction(int signo, int flags, void (*action)(int, siginfo_t*, void*));
void send_handler(int sig, siginfo_t* info, void* ucontext);
void verify_handler(int sig, siginfo_t* info, void* ucontext);
void await_signal(int sig);

int main(int argc, char* argv[]) {

    printf("Sender starting with PID=%d...\n", getpid());
    if (argc<4 ||
        (strcmp(argv[3], KILL)!=0 &&
         strcmp(argv[3], SIGQUEUE)!=0 &&
         strcmp(argv[3], SIGRT)!=0) ||
        !isNumber(argv[2]) ||
        !isNumber(argv[1])){
        fprintf(stderr, "Provide: catcherPID amount type[KILL/SIGQUEUE/SIGRT]\n");
        kill(getppid(), SIGINT);
        exit(1);
    }

    char* action;
    if ((action = calloc(10, sizeof(char)))==NULL){
        perror("Error allocating memory");
        kill(getppid(), SIGINT);
    }

    strcpy(action, argv[3]);
    size_t amount = strtol(argv[2], NULL, 10);
    catcherPID = (int)strtol(argv[1],  NULL, 10);


    if (strcmp(action, SIGRT)==0){
        SEND_SIGNAL=SIGRTMIN+1;
        VERIFY_SIGNAL=SIGRTMIN;
    }

    if (strcmp(action, SIGQUEUE)==0){
        SIGQUEUE_FLAG = true;
    }

//    make mask to block signals
    sigfillset(&mask);
    sigdelset(&mask, SEND_SIGNAL);
    sigdelset(&mask, VERIFY_SIGNAL);
    sigprocmask(SIG_BLOCK, &mask, NULL);


//    signal handlers for signals
    setup_with_sigaction(SEND_SIGNAL, SA_SIGINFO, send_handler);
    setup_with_sigaction(VERIFY_SIGNAL, SA_SIGINFO, verify_handler);


    printf("Sender start transmission...\n");
    SENDING = true;

//    block USR1 and wait for it after every send signal
    sigset_t add_mask, old_mask;
    sigemptyset(&add_mask);
    sigaddset(&add_mask, SEND_SIGNAL);

    sigprocmask (SIG_BLOCK, &add_mask, &old_mask);

    sigval_t sigval;
    for (int i = 0; i < amount; ++i) {
        if (strcmp(action, KILL)==0 || strcmp(action, SIGRT)==0){
            kill(catcherPID, SEND_SIGNAL);
        } else if (strcmp(action, SIGQUEUE)==0){
            sigval.sival_int = i+1;
            sigqueue(catcherPID, SEND_SIGNAL, sigval);
        }
        while (!usr_interrupt)
            sigsuspend(&old_mask);
        usr_interrupt = false;
    }
    sigprocmask (SIG_UNBLOCK, &add_mask, NULL);
    setup_with_sigaction(SEND_SIGNAL, SA_SIGINFO, send_handler);
    setup_with_sigaction(VERIFY_SIGNAL, SA_SIGINFO, verify_handler);

    SENDING = false;
    printf("Sender ends transmission...\n");



//    start listening
    LISTENING = true;
    if (fork()==0){
        sleep(1);
        printf("Sender sends verifier...\n");
        kill(catcherPID, VERIFY_SIGNAL);
        exit(0);
    }

    printf("Sender starts listening...\n");

    await_signal(VERIFY_SIGNAL);
    setup_with_sigaction(SEND_SIGNAL, SA_SIGINFO, send_handler);
    setup_with_sigaction(VERIFY_SIGNAL, SA_SIGINFO, verify_handler);

    printf("Sender received verifier\n");


    printf("Sender received back a total of %zu signals and should receive %zu\n", RECEIVED_MESSAGES, amount);
    if (SIGQUEUE_FLAG){
        printf("According to catcher sender should have received %zu signals\n", AMOUNT_FROM_CATCHER);
    }
    LISTENING = false;

    return 0;
}

void await_signal(int sig){
    sigset_t add_mask, old_mask;
    sigemptyset(&add_mask);
    sigaddset(&add_mask, sig);

    sigprocmask (SIG_BLOCK, &add_mask, &old_mask);
    while (!usr_interrupt)
        sigsuspend(&old_mask);
    usr_interrupt = false;
    sigprocmask (SIG_UNBLOCK, &add_mask, NULL);
}

bool isNumber(char* value){
    for (int i = 0; i < strlen(value); ++i) {
        if (!isdigit(value[i]))
            return false;
    }
    return true;
}

void setup_with_sigaction(int signo, int flags, void (*action)(int, siginfo_t*, void*)){
    struct sigaction act;

    act.sa_flags = flags;

    act.sa_sigaction = action;

    sigemptyset(&act.sa_mask);
//    sigaddset(&act.sa_mask, signo); - added by default


    sigaction(signo, &act, NULL);
}


void verify_handler(int sig, siginfo_t* info, void* ucontext) {
    LISTENING = false;
    signal(SEND_SIGNAL, SIG_IGN);
    usr_interrupt = true;
}

void send_handler(int sig, siginfo_t* info, void* ucontext) {
    if (LISTENING){
        RECEIVED_MESSAGES += 1;
        if (SIGQUEUE_FLAG){
            AMOUNT_FROM_CATCHER = max(AMOUNT_FROM_CATCHER, info->si_value.sival_int);
        }
        kill(catcherPID, SEND_SIGNAL);
    }

    if (SENDING){
        usr_interrupt = true;
    }
}