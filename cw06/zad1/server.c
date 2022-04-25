#include "que_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>


#define LOG_FILE "server_log.txt"
FILE* log_file;

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
int handle_init(const message_t* message);
void handle_2all(const message_t* message);
void handle_2one(const message_t* message);
void send_to_client(int client_msqid, int client_pid, message_t* message);
int find_free_client_id();
char *strstrip(char *s);
size_t number_len(int num){
    size_t count = 0;
    while(num!=0){
        num=num/10;
        count++;
    }
    return count;
}

int main(){
    atexit(clean);
    init();

    message_t message;
    size_t LOG_MAX_LEN = MESSAGE_BUFFER_SIZE+number_len(MAX_CLIENTS)+30;
    char log[LOG_MAX_LEN];
    while (1){
        if (receive(server_msqid, &message)==-1){
            perror("Error receiving message\n");
            if (errno != EINTR){
                exit(1);
            }
            continue;
        }

        switch (message.type) {
            case TYPE_STOP: {
                snprintf(log, LOG_MAX_LEN, "%s STOP from %d\n",
                         strstrip(asctime(localtime(&message.timestamp))),
                         message.client_id);
                handle_stop(&message);
                break;
            }
            case TYPE_LIST: {
                snprintf(log, LOG_MAX_LEN, "%s LIST from %d\n",
                         strstrip(asctime(localtime(&message.timestamp))),
                         message.client_id);
                handle_list(&message);
                break;
            }
            case TYPE_2ALL: {
                snprintf(log, LOG_MAX_LEN, "%s 2ALL from %d - %s\n",
                         strstrip(asctime(localtime(&message.timestamp))),
                         message.client_id,
                         message.text);
                handle_2all(&message);
                break;
            }
            case TYPE_2ONE: {
                snprintf(log, LOG_MAX_LEN, "%s 2ONE from %d to %d - %s\n",
                         strstrip(asctime(localtime(&message.timestamp))),
                         message.client_id,
                         message.receiver_id,
                         message.text);
                handle_2one(&message);
                break;
            }
            case TYPE_INIT: {
                int new_id;
                if ((new_id = handle_init(&message)) == -1)
                    snprintf(log, LOG_MAX_LEN, "%s INIT failed\n",
                             strstrip(asctime(localtime(&message.timestamp))));
                else
                    snprintf(log, LOG_MAX_LEN, "%s INIT new client with id=%d\n",
                             strstrip(asctime(localtime(&message.timestamp))), new_id);
                break;
            }
            default: {
                snprintf(log, LOG_MAX_LEN, "%s WRONG TYPE: %ld\n",
                         strstrip(asctime(localtime(&message.timestamp))),message.type);
                fprintf(stderr, "Wrong message type\n");
                break;
            }
        }

        printf("%s", log);
        fwrite(log, sizeof(char), strlen(log), log_file);

    }
}

void init(){

    if ((log_file = fopen(LOG_FILE, "a+"))==NULL){
        fprintf(stderr, "Could not open log file\n");
        exit(1);
    }

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
    fclose(log_file);
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

int handle_init(const message_t* message){
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
        error.timestamp = get_time();

        send(client_msqid, &error);
        return -1;
    }


    clients[free_client_id][CLIENT_MSQID] = client_msqid;
    clients[free_client_id][CLIENT_PID] = client_pid;
    client_is_set[free_client_id] = true;


    message_t init;
    init.type = TYPE_INIT;
    strcpy(init.text, "Successfully joined");
    init.receiver_id = free_client_id;
    init.client_id = free_client_id;
    init.timestamp = get_time();

    send(client_msqid, &init);
    return free_client_id;
}

void handle_stop(const message_t* message){
    client_is_set[message->client_id] = false;
    clients[message->client_id][CLIENT_MSQID] = -1;
    clients[message->client_id][CLIENT_PID] = -1;
}

void handle_list(const message_t* message){
    message_t response;
    response.receiver_id = message->client_id;
    response.client_pid = message->client_pid;
    response.type = TYPE_LIST;
    response.timestamp = get_time();

    strcpy(response.text, "");

    size_t str_len = number_len(MAX_CLIENTS);
    char *string = calloc(str_len+2, sizeof(char));
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client_is_set[i]){
            snprintf(string, str_len+1, "%d ", i);
            strcat(response.text, string);
        }
    }
    free(string);

    int client_msqid = clients[message->client_id][CLIENT_MSQID];
    send_to_client(client_msqid, response.client_pid, &response);
}

void handle_2all(const message_t* message){
    int receiver_msqid;
    int receiver_pid;
    message_t new_message;
    new_message.client_id = message->client_id;
    new_message.timestamp = get_time();
    new_message.type = TYPE_2ALL;
    new_message.client_pid = message->client_pid;
    strcpy(new_message.text, message->text);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (client_is_set[i] && message->client_id != i){
            receiver_msqid = clients[i][CLIENT_MSQID];
            receiver_pid = clients[i][CLIENT_PID];
            new_message.receiver_id = i;
            send_to_client(receiver_msqid, receiver_pid, &new_message);
        }
    }
}

void handle_2one(const message_t* message){
    int receiver_msqid;
    int receiver_pid;

    message_t new_message;
    new_message.client_id = message->client_id;
    new_message.timestamp = get_time();
    new_message.type = TYPE_2ONE;
    new_message.client_pid = message->client_pid;
    new_message.receiver_id = message->receiver_id;
    receiver_msqid = clients[new_message.receiver_id][CLIENT_MSQID];
    receiver_pid = clients[new_message.receiver_id][CLIENT_PID];

    strcpy(new_message.text, message->text);

    send_to_client(receiver_msqid, receiver_pid, &new_message);
}

char *strstrip(char *s)
{
    size_t size;
    char *end;

    size = strlen(s);

    if (!size)
        return s;

    end = s + size - 1;
    while (end >= s && isspace(*end))
        end--;
    *(end + 1) = '\0';

    while (*s && isspace(*s))
        s++;

    return s;
}