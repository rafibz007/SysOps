#include "utils.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

client *clients[MAX_CLIENTS];
int clients_counter = 0;

int get_client_opponent(int idx);
int add_new_client(char *name, int fd);
int get_client_id(char *name);
void free_client_data(int index);
void remove_client(char *name);
void remove_not_responding_clients();
void send_pings();
void* ping_clients();
int create_local_socket(char* socket_path);
int create_network_socket(char *port);
int check_socket_messages(int local_socket, int network_socket);
void end();

void manage_add(int client_fd, char* command, char* arg, char* name);
void manage_move(int client_fd, char* command, char* arg, char* name);
void manage_end(int client_fd, char* command, char* arg, char* name);
void manage_re_ping(int client_fd, char* command, char* arg, char* name);

int main(int argc, char* argv[]){
    if (argc < 3){
        printf("Provide: port path\n");
        exit(1);
    }

    signal(SIGINT, end);

    char *port = argv[1];
    char *socket_path = argv[2];

    int network_socket = create_network_socket(port);
    int local_socket = create_local_socket(socket_path);

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clients[i] = NULL;
    }

    pthread_t t;
    pthread_create(&t, NULL, &ping_clients, NULL);

    srand(getpid());
    while (true){
        int client_fd = check_socket_messages(local_socket, network_socket);
        char buffer[MAX_MSG_LEN + 1];
        recv(client_fd, buffer, MAX_MSG_LEN, 0);

        char *command = strtok(buffer, ":");
        char *arg = strtok(NULL, ":");
        char *name = strtok(NULL, ":");

        pthread_mutex_lock(&mutex);
        if (strcmp(command, COMMAND_ADD) == 0)
            manage_add(client_fd, command, arg, name);
        else if (strcmp(command, COMMAND_MOVE) == 0)
            manage_move(client_fd, command, arg, name);
        else if (strcmp(command, COMMAND_END) == 0)
            remove_client(name);
        else if (strcmp(command, COMMAND_RE_PING) == 0)
            manage_re_ping(client_fd, command, arg, name);
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

    // remove opponent if exists
    if (clients[get_client_opponent(index)] != NULL)
        remove_client(clients[get_client_opponent(index)]->username);

}

void manage_add(int client_fd, char* command, char* arg, char* name){
    printf("Received ADD\n");
    int index = add_new_client(name, client_fd);

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
        int random_num = rand() % 100;
        int first, second;

        if (random_num % 2 == 0){
            first = index;
            second = get_client_opponent(index);
        }else{
            second = index;
            first = get_client_opponent(index);
        }

        char response_buffer[MAX_MSG_LEN+1];
        sprintf(response_buffer, "%s:%s", COMMAND_ADD, "O");
        send(clients[first]->fd, response_buffer,
             MAX_MSG_LEN, 0);

        sprintf(response_buffer, "%s:%s", COMMAND_ADD, "X");
        send(clients[second]->fd, response_buffer,
             MAX_MSG_LEN, 0);
    }
}

void manage_move(int client_fd, char* command, char* arg, char* name){
    printf("Received MOVE\n");
    int player = get_client_id(name);
    int move = (int)strtol(arg, NULL, 10);

    char buffer[MAX_MSG_LEN+1];
    sprintf(buffer, "%s:%d", COMMAND_MOVE, move);
    send(clients[get_client_opponent(player)]->fd, buffer, MAX_MSG_LEN,0);
}

void manage_re_ping(int client_fd, char* command, char* arg, char* name){
    printf("Received REPING\n");
    int player = get_client_id(name);
    if (player != -1) clients[player]->is_responding = 1;
}

int get_client_opponent(int idx){
    return idx % 2 == 0 ? idx + 1 : idx - 1;
}

int add_new_client(char *name, int fd){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->username, name) == 0)
            return -1;

    int idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i += 2)
        if (clients[i] != NULL && clients[i + 1] == NULL){
            idx = i + 1;
            break;
        }

    if (idx == -1)
        for (int i = 0; i < MAX_CLIENTS; i++)
            if (clients[i] == NULL){
                idx = i;
                break;
            }

    if (idx != -1){
        client *new_client = calloc(1, sizeof(client));
        new_client->username = calloc(MAX_MSG_LEN, sizeof(char));
        strcpy(new_client->username, name);
        new_client->is_responding = true;
        new_client->fd = fd;

        clients[idx] = new_client;
        clients_counter++;
    }

    return idx;
}

int get_client_id(char *name){
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i] != NULL && strcmp(clients[i]->username, name) == 0) return i;
    return -1;
}

int create_network_socket(char *port){
    // create socket
    int sock_fd;

    if ((sock_fd = socket(AF_INET , SOCK_STREAM, 0))  == -1){
        perror("Error creating inet socket\n");
        exit(1);
    }

    struct sockaddr_in sock_addr;
    sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(atoi(port));

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

int check_socket_messages(int local_socket, int network_socket){

    struct pollfd *fds = calloc(2 + clients_counter, sizeof(struct pollfd));
    fds[0].fd = local_socket;
    fds[0].events = POLLIN;
    fds[1].fd = network_socket;
    fds[1].events = POLLIN;

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < clients_counter; i++){
        fds[i + 2].fd = clients[i]->fd;
        fds[i + 2].events = POLLIN;
    }

    pthread_mutex_unlock(&mutex);

    poll(fds, clients_counter + 2, -1);
    int retval;

    for (int i = 0; i < clients_counter + 2; i++){
        if (fds[i].revents & POLLIN){
            retval = fds[i].fd;
            break;
        }
    }
    if (retval == local_socket || retval == network_socket){
        retval = accept(retval, NULL, NULL);
    }
    free(fds);

    return retval;
}

void remove_client(char *name){
    int idx = -1;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i] != NULL && strcmp(clients[i]->username, name) == 0)
            idx = i;
    }

    if (idx == -1)
        return;

    printf("Removing client: %s\n", name);
    char buffer[MAX_MSG_LEN+1];
    sprintf(buffer, "%s:%s", COMMAND_END, "");
    send(clients[idx]->fd, buffer, MAX_MSG_LEN, 0);
    free_client_data(idx);
}

void* ping_clients(){
    while (true){
        printf("CHECKING CLIENTS\n");
        pthread_mutex_lock(&mutex);
        remove_not_responding_clients();
        send_pings();
        pthread_mutex_unlock(&mutex);

        sleep(10);
    }
}

void send_pings(){
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i]){
            char buffer[MAX_MSG_LEN+1];
            sprintf(buffer, "%s: ", COMMAND_PING);
            send(clients[i]->fd, buffer, MAX_MSG_LEN, 0);
            clients[i]->is_responding = false;
        }
    }
}

int create_local_socket(char* socket_path){
    // create socket
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