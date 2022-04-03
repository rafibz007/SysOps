#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <signal.h>

#define KILL "KILL"
#define SIGQUEUE "SIGQUEUE"
#define SIGRT "SIGRT"

bool isNumber(char* value);

sigset_t mask;
bool usr_interrupt = false;

int RECEIVE_SIGNAL=SIGUSR1;
int VERIFY_SIGNAL=SIGUSR2;

bool LISTENING = false;
bool SENDING = false;

size_t RECEIVED_MESSAGES = 0;
pid_t SENDER_PID = 0;


void setup_with_sigaction(int signo, int flags, void (*action)(int, siginfo_t*, void*));
void receive_handler(int sig, siginfo_t* info, void* ucontext);
void verify_handler(int sig, siginfo_t* info, void* ucontext);
void await_signal(int sig);

int main(int argc, char* argv[]) {

    printf("Catcher starting with PID=%d...\n", getpid());
    if (argc<2 ||
        (strcmp(argv[1], KILL)!=0 &&
         strcmp(argv[1], SIGQUEUE)!=0 &&
         strcmp(argv[1], SIGRT)!=0)){
        fprintf(stderr, "Provide: type[KILL/SIGQUEUE/SIGRT]\n");
        kill(getppid(), SIGINT);
        exit(1);
    }

    char* action;
    if ((action = calloc(10, sizeof(char)))==NULL){
        perror("Error allocating memory");
        kill(getppid(), SIGINT);
        exit(1);
    }

    strcpy(action, argv[1]);

    if (strcmp(action, SIGRT)==0){
        RECEIVE_SIGNAL=SIGRTMIN+1;
        VERIFY_SIGNAL=SIGRTMIN;
    }

//    make add_mask to block signals

    sigfillset(&mask);
    sigdelset(&mask, RECEIVE_SIGNAL);
    sigdelset(&mask, VERIFY_SIGNAL);
    sigprocmask(SIG_BLOCK, &mask, NULL);


//    signal handlers receive signals
    setup_with_sigaction(RECEIVE_SIGNAL, SA_SIGINFO, receive_handler);
    setup_with_sigaction(VERIFY_SIGNAL, SA_SIGINFO, verify_handler);


//    tell main that catcher is ready
    LISTENING = true;
    pid_t main = getppid();
    if (fork()==0){
        sleep(1);
        kill(main, SIGUSR1);
        exit(0);
    }
    printf("Catcher start listening...\n");

//    todo
    await_signal(VERIFY_SIGNAL);
    setup_with_sigaction(RECEIVE_SIGNAL, SA_SIGINFO, receive_handler);
    setup_with_sigaction(VERIFY_SIGNAL, SA_SIGINFO, verify_handler);

    printf("Catcher received verifier\n");

    LISTENING = false;


    printf("Catcher start retransmission...\n");
    SENDING = true;

//    block USR1 and wait for it after every send signal
    sigset_t add_mask, old_mask;
    sigemptyset(&add_mask);
    sigaddset(&add_mask, RECEIVE_SIGNAL);


    sigprocmask (SIG_BLOCK, &add_mask, &old_mask);

    sigval_t sigval;
    for (int i = 0; i < RECEIVED_MESSAGES; ++i) {
        if (strcmp(action, KILL)==0 || strcmp(action, SIGRT)==0){
            kill(SENDER_PID, RECEIVE_SIGNAL);
        } else if (strcmp(action, SIGQUEUE)==0){
            sigval.sival_int = i+1;
            sigqueue(SENDER_PID, RECEIVE_SIGNAL, sigval);
        }
        while (!usr_interrupt){
            sigsuspend(&old_mask);
        }
        usr_interrupt = false;
    }
    sigprocmask (SIG_UNBLOCK, &add_mask, NULL);
    setup_with_sigaction(RECEIVE_SIGNAL, SA_SIGINFO, receive_handler);
    setup_with_sigaction(VERIFY_SIGNAL, SA_SIGINFO, verify_handler);
    SENDING = false;

    printf("Catcher end retransmission...\n");

    printf("Catcher sends verifier...\n");
    kill(SENDER_PID, VERIFY_SIGNAL);

    printf("Catcher received a total of %zu signals from PID=%d\n", RECEIVED_MESSAGES, SENDER_PID);


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

void receive_handler(int sig, siginfo_t* info, void* ucontext) {
    if (LISTENING){
        RECEIVED_MESSAGES += 1;
        SENDER_PID = info->si_pid;
        kill(SENDER_PID, RECEIVE_SIGNAL);
    }
    if (SENDING){
        usr_interrupt = true;
    }
}

void verify_handler(int sig, siginfo_t* info, void* ucontext) {
    LISTENING = false;
    signal(RECEIVE_SIGNAL, SIG_IGN);
    usr_interrupt = true;
}