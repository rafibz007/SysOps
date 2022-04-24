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



#define MAX_CLIENTS 10

#define CLIENT_MSQID 0
#define CLIENT_PID 1
int clients[MAX_CLIENTS][2]; // client_msqid, client_pid ; index - client_id
bool client_is_set[MAX_CLIENTS];

int server_msqid;

void clean();
void init();

void handle_sigint(int sig);
void handle_list(const message_t* message);
void handle_stop(const message_t* message);
void handle_init(const message_t* message);
void handle_2all(const message_t* message);
void handle_2one(const message_t* message);
void send_to_client(int client_msqid, int client_pid, message_t* message);
int find_free_client_id();

int main(){
    atexit(clean);
    init();

    message_t message;
    while (1){
        if (receive(server_msqid, &message)==-1){
            perror("Error receiving message\n");
            if (errno != EINTR){
                exit(1);
            }
            continue;
        }

//        printf("Received message with type=%ld\n", message.type);

        switch (message.type) {
            case TYPE_STOP:
                handle_stop(&message);
                break;
            case TYPE_LIST:
                handle_list(&message);
                break;
            case TYPE_2ALL:
                handle_2all(&message);
                break;
            case TYPE_2ONE:
                handle_2one(&message);
                break;
            case TYPE_INIT:
                handle_init(&message);
                break;
            default:
                fprintf(stderr, "Wrong message type\n");
                break;
        }
    }
}

void init(){
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = handle_sigint;
    sigaction(SIGINT, &act, NULL);


    key_t key = get_server_key();
    if ((server_msqid = create_queue(key))==-1){
        perror("Error creating server message queue\n");
        exit(1);
    }
//    printf("Creating server=%d\n", server_msqid);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_is_set[i] = false;
        clients[i][0] = -1;
        clients[i][1] = -1;
    }

    printf("Server initiated successfully\n");
}

bool all_trues(const bool array[]){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!array[i])
            return false;
    }
    return true;
}

void clean(){

    message_t close_message;
    strcpy(close_message.text, "Server is closing");
    close_message.type = TYPE_CLOSE;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client_is_set[i]){
            close_message.client_pid = clients[i][CLIENT_PID];
            close_message.client_id = i;
            close_message.receiver_id = i;
            send_to_client(clients[i][CLIENT_MSQID], clients[i][CLIENT_PID], &close_message);
        }
    }


//    WAIT UNTIL EVERYBODY IS CLOSED
    bool client_shutdown[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; ++i)
        client_shutdown[i] = !client_is_set[i];


    message_t message;
    while (!all_trues(client_shutdown)){
        if (receive(server_msqid, &message)==-1){
            perror("Error receiving message\n");
            if (errno != EINTR){
                exit(1);
            }
            continue;
        }

        switch (message.type) {
            case TYPE_STOP:
                handle_stop(&message);
                client_shutdown[message.client_id] = true;
                break;
            default:
                fprintf(stderr, "Waiting only for STOP messages\n");
                break;
        }
    }

    delete_queue(server_msqid);
    printf("Server cleaned successfully\n");
}

void send_to_client(int client_msqid, int client_pid, message_t* message){
    send(client_msqid, message);
    kill(client_pid,MESSAGE_AWAITS);
}

int find_free_client_id(){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (!client_is_set[i])
            return i;
    }

    return -1;
}

void handle_sigint(int sig){
    exit(0);
}

void handle_init(const message_t* message){
    key_t key;
    key = (key_t) strtol(message->text, NULL, 10);
    int client_pid = message->client_pid;
    int client_msqid = get_queue(key);


    int free_client_id = find_free_client_id();
    if (free_client_id == -1){
        fprintf(stderr, "Max amount of clients reached\n");

        message_t error;
        error.type = TYPE_INIT_ERROR;
        strcpy(error.text, "Max amount of clients reached");
        error.client_pid = client_pid;
        error.receiver_id = -1;
        error.client_id = -1;

        send(client_msqid, &error);
        return;
    }


    clients[free_client_id][CLIENT_MSQID] = client_msqid;
    clients[free_client_id][CLIENT_PID] = client_pid;
    client_is_set[free_client_id] = true;

    printf("Registered client=%d with id=%d\n", client_msqid, free_client_id);

    message_t init;
    init.type = TYPE_INIT;
    strcpy(init.text, "Successfully joined");
    init.receiver_id = free_client_id;
    init.client_id = free_client_id;

    send(client_msqid, &init);
}

void handle_stop(const message_t* message){
    client_is_set[message->client_id] = false;
    clients[message->client_id][CLIENT_MSQID] = -1;
    clients[message->client_id][CLIENT_PID] = -1;
    printf("Unregistered with id=%d\n", message->client_id);
}
//todo
void handle_list(const message_t* message){}
void handle_2all(const message_t* message){}
void handle_2one(const message_t* message){}