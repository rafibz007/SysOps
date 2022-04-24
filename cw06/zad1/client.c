#include "que_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>


int server_msqid;
int client_msqid;
int client_id;

void clean();
void init();
void pick_action();

void handle_sigint(int sig);
void handle_list(const message_t* message);
void stop();
void handle_init(const message_t* message);
void handle_2all(const message_t* message);
void handle_2one(const message_t* message);
void handle_close(const message_t* message);
void handle_message();

int main(){
    atexit(clean);
    init();

    while (1){
        pick_action();
    }
}

void init(){
    server_msqid = get_server_key();

    struct sigaction act1;

    sigemptyset(&act1.sa_mask);
    act1.sa_flags = 0;
    act1.sa_handler = handle_sigint;
    sigaction(SIGINT, &act1, NULL);

    struct sigaction act2;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;
    act2.sa_handler = handle_message;
    sigaction(MESSAGE_AWAITS, &act2, NULL);

    key_t key = get_client_key();
    if ((client_msqid = create_queue(key))==-1){
        perror("Error creating client message queue");
        exit(1);
    }
    printf("Creating client=%d\n", client_msqid);

    printf("Sending init message to server=%d...\n", server_msqid);
    message_t init;

    init.type = TYPE_INIT;
    snprintf(init.text, MESSAGE_BUFFER_SIZE-1, "%d", client_msqid);
    init.client_pid = getpid();

    send(server_msqid, &init);

    bool initReceived = false;
    message_t message;
    while (!initReceived){
        if (receive(client_msqid, &message)==-1){
            perror("Error receiving message\n");
            if (errno != EINTR){
                exit(1);
            }
            continue;
        }

        switch (message.type) {
            case TYPE_CLOSE:
                handle_close(&message);
                break;
            case TYPE_INIT:
                handle_init(&message);
                break;
            case TYPE_INIT_ERROR:
                fprintf(stderr, "Error connecting to server: %s\n", message.text);
                exit(0);
                break;
            default:
                fprintf(stderr, "Waiting for init response");
                break;
        }
    }

    printf("Client initiated successfully\n");
}

void clean(){
    delete_queue(client_msqid);
    printf("Client cleaned successfully\n");
}

void handle_message(){
    message_t message;
    while (!is_empty(client_msqid)){
        if (receive(server_msqid, &message)==-1){
            perror("Error receiving message\n");
            if (errno != EINTR){
                exit(1);
            }
            continue;
        }

        switch (message.type) {
            case TYPE_LIST:
                handle_list(&message);
                break;
            case TYPE_2ALL:
                handle_2all(&message);
                break;
            case TYPE_2ONE:
                handle_2one(&message);
                break;
            case TYPE_CLOSE:
                handle_close(&message);
                break;
            default:
                fprintf(stderr, "Wrong message type");
                break;
        }
    }
}

void handle_init(const message_t* message){
    client_id = message->client_id;
}

//todo
void handle_sigint(int sig){
    stop();
    printf("Client exiting\n");
}

void stop(){
    message_t stop;
    stop.type = TYPE_STOP;
    stop.client_id = client_id;
    stop.client_pid = getpid();

    send(server_msqid, &stop);
    exit(0);
}

void handle_close(const message_t* message){
    stop();
}

void pick_action(){}

void handle_list(const message_t* message){}
void handle_2all(const message_t* message){}
void handle_2one(const message_t* message){}
