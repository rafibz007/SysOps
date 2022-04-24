#include <stdlib.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/msg.h>

// TYPES
#define TYPE_STOP 1
#define TYPE_LIST 2
#define TYPE_2ALL 3
#define TYPE_2ONE 4
#define TYPE_INIT 5
#define TYPE_CLOSE 6
#define TYPE_INIT_ERROR 7
#define MAX_TYPE 8


// MESSAGES
#define MESSAGE_BUFFER_SIZE 1024
#define MESSAGE_AWAITS SIGRTMIN

struct message_t
{
    long type;
    time_t timestamp;
    int client_id;
    int receiver_id;
    char text[MESSAGE_BUFFER_SIZE];
    pid_t client_pid;
};
typedef struct message_t message_t;

#define MAX_MESSAGE_SIZE (sizeof(message_t) - sizeof(long))
#define MESSAGE_SIZE sizeof(message_t)

//QUEUE UTILS
#define MSGRCV_MTYPE (-MAX_TYPE)
#define SERVER_PROJ_ID 1

// KEYS
key_t get_server_key();

key_t get_client_key();

// QUEUE UTILS
// SENDING
int send_with_flag(int msqid, message_t* message, int msgflg);

int send_no_wait(int msqid, message_t* message);

int send(int msqid, message_t* message);

// RECEIVING
long receive_with_flag(int msqid, message_t* message, int msgflg);

long receive_no_wait(int msqid, message_t* message);

long receive(int msqid, message_t* message);

// CREATING
int create_queue(int key);

int get_queue(int key);

// DELETING
int delete_queue(int msqid);

// INFO
int is_empty(int msqid);