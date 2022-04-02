#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

void setup_with_handler(int signo, int flags, void (*handler)(int));
void setup_with_sigaction(int signo, int flags, void (*action)(int, siginfo_t*, void*));

void sigusr1_action(int sig, siginfo_t* info, void* ucontext);
void sigusr2_action(int sig, siginfo_t* info, void* ucontext);
void sigchild_action(int sig, siginfo_t* info, void* ucontext);

void sigchild_handler(int sig);
void sigusr1_handler(int sig);

int main(int argc, char* argv[]) {

    printf("=== SA_SIGINFO ===\n");

    setup_with_sigaction(SIGUSR1, SA_SIGINFO,sigusr1_action);
    raise(SIGUSR1);

    setup_with_sigaction(SIGUSR2, SA_SIGINFO, sigusr2_action);
    sigval_t sigval = {15};
    sigqueue(getpid(), SIGUSR2, sigval);


    setup_with_sigaction(SIGCHLD, SA_SIGINFO, sigchild_action);
    if (fork()==0){
        printf("I'm child with PID=%d and I will exit with code 5\n", getpid());
        exit(5);
    }

    int status;
    while (wait(&status)>0);

    printf("\n=== SA_NOCLDSTOP ===\n");

    setup_with_handler(SIGCHLD, SA_NOCLDSTOP, sigchild_handler);
    pid_t child;
    if ((child = fork())==0){
        printf("I'm child with PID=%d and I will be stopped for couple of seconds, so my parent's "
               "handler should not run for until I kill him, because of SA_NOCLDSTOP\n", getpid());
        raise(SIGSTOP);
    }

    sleep(3);
    printf("Killing child\n");
    kill(child, SIGKILL);
    while (wait(&status)>0);

    printf("\n=== SA_RESETHAND ===\n");

    setup_with_handler(SIGUSR1, SA_RESETHAND, sigusr1_handler);
    raise(SIGUSR1);
    raise(SIGUSR1);

    return 0;
}

void sigusr1_handler(int sig){
    printf("Got signal: %d, next time this signal will be handled default way\n", sig);
}

void sigchild_handler(int sig){
    printf("Got signal: %d, from my dead child, a bit scary\n", sig);
}

void sigusr1_action(int sig, siginfo_t* info, void* ucontext){
    printf("Got signal %d from PID=%d with signal code: %d\n", info->si_signo, info->si_pid, info->si_code);
}

void sigusr2_action(int sig, siginfo_t* info, void* ucontext){
    printf("Got signal %d from PID=%d with value int: %d\n", info->si_signo, info->si_pid, info->si_value.sival_int);
}

void sigchild_action(int sig, siginfo_t* info, void* ucontext){
    printf("Got signal %d from PID=%d with child exit code: %d\n", info->si_signo, info->si_pid, info->si_status);
}


void setup_with_handler(int signo, int flags, void (*handler)(int)){
    struct sigaction act;

    act.sa_flags = flags;

    act.sa_handler = handler;

    sigemptyset(&act.sa_mask);
//    sigaddset(&act.sa_mask, signo); - added by default


    sigaction(signo, &act, NULL);
}

void setup_with_sigaction(int signo, int flags, void (*action)(int, siginfo_t*, void*)){
    struct sigaction act;

    act.sa_flags = flags;

    act.sa_sigaction = action;

    sigemptyset(&act.sa_mask);
//    sigaddset(&act.sa_mask, signo); - added by default


    sigaction(signo, &act, NULL);
}