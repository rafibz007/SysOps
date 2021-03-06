#include "que_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>

#define TYPE_STOP_STR "STOP"
#define TYPE_LIST_STR "LIST"
#define TYPE_2ALL_STR "2ALL"
#define TYPE_2ONE_STR "2ONE"

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
    key_t server_key = get_server_key();
    server_msqid = get_queue(server_key);

    struct sigaction act1;

    sigemptyset(&act1.sa_mask);
    act1.sa_flags = 0;
    act1.sa_handler = handle_sigint;
    sigaction(SIGINT, &act1, NULL);

    key_t key = get_client_key();
    if ((client_msqid = create_queue(key))==-1){
        perror("Error creating client message queue");
        exit(1);
    }

    message_t init;

    init.type = TYPE_INIT;
    snprintf(init.text, MESSAGE_BUFFER_SIZE-1, "%d", key);
    init.client_pid = getpid();
    init.timestamp = get_time();

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
                initReceived = true;
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

    struct sigaction act2;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;
    act2.sa_handler = handle_message;
    sigaction(MESSAGE_AWAITS, &act2, NULL);

    printf("Client initiated successfully\n");
    printf("Your id=%d\n", client_id);
}

void clean(){
    delete_queue(client_msqid);
    printf("Client cleaned successfully\n");
}

void handle_message(){
    message_t message;
    while (!is_empty(client_msqid)){
        if (receive(client_msqid, &message)==-1){
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

void handle_sigint(int sig){
    printf("Client exiting\n");
    stop();
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

void pick_action(){
    char input[PATH_MAX] = "";
    printf(">");
    fgets(input, PATH_MAX-1, stdin);

    char* action = strtok(input, " \n");

    if (action == NULL){
        return;
    }


    message_t message;
    message.client_id = client_id;
    message.client_pid = getpid();
    message.timestamp = get_time();
    if (strcmp(action, TYPE_LIST_STR)==0){
        message.type = TYPE_LIST;
        send(server_msqid, &message);
    } else if (strcmp(action, TYPE_2ALL_STR)==0){

        message.type = TYPE_2ALL;
        char* text = strtok(NULL, "\n");
        if (text == NULL){
            fprintf(stderr, "2ALL text\n");
            return;
        }
        strcpy(message.text, text);
        send(server_msqid, &message);

    } else if (strcmp(action, TYPE_2ONE_STR)==0){

        message.type = TYPE_2ONE;
        char* id = strtok(NULL, " ");
        char* text = strtok(NULL, "\n");
        if (id == NULL || text == NULL){
            fprintf(stderr, "2ONE id text\n");
            return;
        }
//        printf("id: %d, text: %s", (int)strtol(id, NULL, 10), text);
        strcpy(message.text, text);
        message.receiver_id = (int)strtol(id, NULL, 10);
        send(server_msqid, &message);

    } else if (strcmp(action, TYPE_STOP_STR)==0){
        stop();
    } else {
        fprintf(stderr, "'%s' is not valid action.\n", action);
    }

}

void handle_list(const message_t* message){
    printf(">LIST: %s\n", message->text);
}

void handle_2all(const message_t* message){
    printf(">2ALL: %d - %s\n", message->client_id, message->text);
}

void handle_2one(const message_t* message){
    printf(">2ONE: %d - %s\n", message->client_id, message->text);
}
