#include "utils.h"

int clients_counter = 0;
client *clients[MAX_CLIENTS];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int get_client_opponent(int index);
int add_new_client(char *username, int fd);
int get_client_id(char *username);
void free_client_data(int index);
void remove_client(char *username);
void remove_not_responding_clients();
void send_pings();
void* ping_clients();
int create_unix_socket(char* socket_path);
int create_inet_socket(char *socket_port);
int check_socket_messages(int unix_socket, int inet_socket);
void end();

void manage_add(int client_fd, char* command, char* arg, char* username);
void manage_move(int client_fd, char* command, char* arg, char* username);
void manage_re_ping(int client_fd, char* command, char* arg, char* username);

int main(int argc, char* argv[]){
    if (argc < 3){
        printf("Provide: port path\n");
        exit(1);
    }

    signal(SIGINT, end);

    char *socket_port = argv[1];
    char *socket_path = argv[2];

    int inet_socket = create_inet_socket(socket_port);
    int unix_socket = create_unix_socket(socket_path);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i] = NULL;
    }

    pthread_t pthread;
    pthread_create(&pthread, NULL, &ping_clients, NULL);

    srand(getpid());
    printf("\n=== SERVER RUNNING... ===\n\n");
    while (true){
        int waiting_client_fd = check_socket_messages(unix_socket, inet_socket);

        char buffer[MAX_MSG_LEN + 1];
        recv(waiting_client_fd, buffer, MAX_MSG_LEN, 0);

        char *command = strtok(buffer, ":");
        char *arg = strtok(NULL, ":");
        char *username = strtok(NULL, ":");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, COMMAND_ADD) == 0)
            manage_add(waiting_client_fd, command, arg, username);
        else if (strcmp(command, COMMAND_MOVE) == 0)
            manage_move(waiting_client_fd, command, arg, username);
        else if (strcmp(command, COMMAND_END) == 0)
            remove_client(username);
        else if (strcmp(command, COMMAND_RE_PING) == 0)
            manage_re_ping(waiting_client_fd, command, arg, username);
        pthread_mutex_unlock(&mutex);
    }
}

void free_client_data(int index){
    if (clients[index] == NULL)
        return;

    free(clients[index]->username);
    free(clients[index]);
    clients[index] = NULL;
    clients_counter--;

    if (clients[get_client_opponent(index)] != NULL)
        remove_client(clients[get_client_opponent(index)]->username);

}

void manage_add(int client_fd, char* command, char* arg, char* username){
    printf("Received ADD\n");

    int index = add_new_client(username, client_fd);

    if (index == -1){
        char response_buffer[MAX_MSG_LEN+1];
        sprintf(response_buffer, "%s:%s", COMMAND_ADD, ARG_NAME_USED);
        send(client_fd, response_buffer, MAX_MSG_LEN, 0);
        close(client_fd);
    } else if (index % 2 == 0){
        char response_buffer[MAX_MSG_LEN+1];
        sprintf(response_buffer, "%s:%s", COMMAND_ADD, ARG_NO_OPPONENT);
        send(client_fd, response_buffer, MAX_MSG_LEN, 0);
    } else {
        int O_player, X_player;

        if (rand() % 2 == 0){
            O_player = index;
            X_player = get_client_opponent(index);
        } else {
            X_player = index;
            O_player = get_client_opponent(index);
        }

        char response_buffer[MAX_MSG_LEN+1];
        sprintf(response_buffer, "%s:%s", COMMAND_ADD, "X");
        send(clients[X_player]->fd, response_buffer,MAX_MSG_LEN, 0);

        sprintf(response_buffer, "%s:%s", COMMAND_ADD, "O");
        send(clients[O_player]->fd, response_buffer,MAX_MSG_LEN, 0);

    }
}

void manage_move(int client_fd, char* command, char* arg, char* username){
    printf("Received MOVE\n");
    int player = get_client_id(username);
    int move = (int)strtol(arg, NULL, 10);

    char buffer[MAX_MSG_LEN+1];
    sprintf(buffer, "%s:%d", COMMAND_MOVE, move);
    send(clients[get_client_opponent(player)]->fd, buffer, MAX_MSG_LEN,0);
}

void manage_re_ping(int client_fd, char* command, char* arg, char* username){
    printf("Received RE-PING\n");
    int player = get_client_id(username);
    if (player != -1) clients[player]->is_responding = 1;
}

int get_client_opponent(int index){
    return index % 2 == 0 ? index + 1 : index - 1;
}

int add_new_client(char *username, int fd){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->username, username) == 0)
            return -1;

    int index = -1;
    for (int i = 0; i < MAX_CLIENTS; i += 2)
        if (clients[i] != NULL && clients[i + 1] == NULL){
            index = i + 1;
            break;
        }

    if (index == -1)
        for (int i = 0; i < MAX_CLIENTS; i++)
            if (clients[i] == NULL){
                index = i;
                break;
            }

    if (index != -1){
        client *clt = calloc(1, sizeof(client));
        clt->username = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(clt->username, username);
        clt->is_responding = true;
        clt->fd = fd;

        clients[index] = clt;
        clients_counter++;

        printf("Added client with username: %s\n", username);
    }

    return index;
}

int get_client_id(char *username){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->username, username) == 0)
            return i;
    return -1;
}

int create_inet_socket(char *socket_port){
    int sock_fd;

    if ((sock_fd = socket(AF_INET , SOCK_STREAM, 0))  == -1){
        perror("Error creating inet socket\n");
        exit(1);
    }

    struct sockaddr_in sock_addr;
    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(strtol(socket_port, NULL, 10));

    if ((bind(sock_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr))) == -1){
        perror("Error binding inet socket\n");
        exit(1);
    }

    if ((listen(sock_fd, MAX_CLIENTS)) == -1){
        perror("Error listening inet socket\n");
        exit(1);
    }

    return sock_fd;

}

int check_socket_messages(int unix_socket, int inet_socket){

    struct pollfd *descriptors = calloc(2 + clients_counter, sizeof(struct pollfd));

    descriptors[0].fd = unix_socket;
    descriptors[0].events = POLLIN;

    descriptors[1].fd = inet_socket;
    descriptors[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clients_counter; i++){
        descriptors[i + 2].fd = clients[i]->fd;
        descriptors[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    int ret = 0;
    poll(descriptors, clients_counter + 2, -1);
    for (int i = 0; i < clients_counter + 2; i++){
        if (descriptors[i].revents & POLLIN){
            ret = descriptors[i].fd;
            break;
        }
    }
    if (ret == unix_socket || ret == inet_socket)
        ret = accept(ret, NULL, NULL);

    free(descriptors);

    return ret;
}

void remove_client(char *username){
    int index = -1;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL && strcmp(clients[i]->username, username) == 0)
            index = i;
    }

    if (index == -1)
        return;

    printf("Removing client with username: %s\n", username);
    char buffer[MAX_MSG_LEN+1];
    sprintf(buffer, "%s:%s", COMMAND_END, "");
    send(clients[index]->fd, buffer, MAX_MSG_LEN, 0);
    free_client_data(index);
}

void* ping_clients(){
    while (true){
        printf("CHECKING CLIENTS\n");

        pthread_mutex_lock(&mutex);
        remove_not_responding_clients();
        send_pings();
        pthread_mutex_unlock(&mutex);

        sleep(5);
    }
}

void send_pings(){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL){
            char buffer[MAX_MSG_LEN+1];
            sprintf(buffer, "%s: ", COMMAND_PING);
            send(clients[i]->fd, buffer, MAX_MSG_LEN, 0);
            clients[i]->is_responding = false;
        }

}

int create_unix_socket(char* socket_path){
    int sock_fd;
    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("Error creating local socket\n");
        exit(1);
    }

    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, socket_path);

    unlink(socket_path);
    if ((bind(sock_fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr))) == -1){
        perror("Error binding local socket\n");
        exit(1);
    }

    if ((listen(sock_fd, MAX_CLIENTS)) == -1){
        perror("Listen on local socket failed\n");
        exit(1);
    }

    return sock_fd;
}

void remove_not_responding_clients(){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] && ! clients[i]->is_responding)
            remove_client(clients[i]->username);
}

void end(){
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients[i] != NULL){
            remove_client(clients[i]->username);
        }
    }
    exit(0);
}