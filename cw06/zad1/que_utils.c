#define _XOPEN_SOURCE 500

#include "que_utils.h"

// KEYS
key_t get_server_key(){
    key_t key;
    if ((key = ftok(getenv("HOME"), SERVER_PROJ_ID)) == -1)
    {
        perror("Error generating server key");
    }
    return key;
}

key_t get_client_key(){
    key_t key;
    if ((key = ftok(getenv("HOME"), getpid())) == -1)
    {
        perror("Error generating client key");
    }
    return key;
}

// QUEUE UTILS
// SENDING
int send_with_flag(int msqid, message_t* message, int msgflg){
    return msgsnd(msqid, message, MAX_MESSAGE_SIZE, msgflg);
}

int send_no_wait(int msqid, message_t* message){
    return send_with_flag(msqid, message, IPC_NOWAIT);
}

int send(int msqid, message_t* message){
    return send_with_flag(msqid, message, 0);
}

// RECEIVING
long receive_with_flag(int msqid, message_t* message, int msgflg){
    return msgrcv(msqid, message, MAX_MESSAGE_SIZE, MSGRCV_MTYPE, msgflg);
}

long receive_no_wait(int msqid, message_t* message){
    return receive_with_flag(msqid, message, IPC_NOWAIT);
}

long receive(int msqid, message_t* message){
    return receive_with_flag(msqid, message, 0);
}

// CREATING
int create_queue(int key){
    return msgget(key, IPC_CREAT | IPC_EXCL | 0600);
//    return msgget(key, IPC_CREAT | 0600);
}

int get_queue(int key){
    return msgget(key, 0);
}

// DELETING
int delete_queue(int msqid){
    return msgctl(msqid, IPC_RMID, NULL);
}

// INFO
int is_empty(int msqid){
    struct msqid_ds stat;
    msgctl(msqid, IPC_STAT, &stat);
    return stat.msg_qnum == 0;
}