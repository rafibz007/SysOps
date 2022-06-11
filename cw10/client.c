#include "utils.h"

int server_socket;
bool client_is_O;
char buffer[MAX_MSG_LEN + 1];
char *name, *command, *arg;
game_board client_game_board;
game_state current_state = START;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

bool make_move(game_board *board, int position);
void parse_command(char* msg);
void close_program();
void start_game();
tile_state check_winning_sign(game_board *board);
void check_game_status();
game_board init_board();
void draw_board();
void connect_inet(char* port);
void connect_local(char* path);
void listen_to_server();

void manage_start();
void manage_wait_for_opponent();
void manage_wait_for_move();
void manage_opponent_move();
void manage_move();


int main(int argc, char* argv[]){

    if (argc < 4){
        fprintf(stderr, "Provide: username [unix/inet] [path/ip:port]\n");
        exit(1);
    }

    signal(SIGINT, close_program);

    name = argv[1];

    if (strcmp(argv[2], "unix") == 0){
        char* path = argv[3];
        connect_local(path);
    }else if (strcmp(argv[2], "inet") == 0){
        char* ip_and_port = argv[3];
        connect_inet(ip_and_port);
    }else{
        fprintf(stderr, "Provide: username [unix/inet] [path/ip:port]\n");
        exit(1);
    }

    sprintf(buffer, "%s: :%s", COMMAND_ADD, name);
    send(server_socket, buffer, MAX_MSG_LEN, 0);

    listen_to_server();

    return 0;
}


void parse_command(char* msg){
    command = strtok(msg, ":");
    arg = strtok(NULL, ":");
}

tile_state check_winning_sign(game_board *board){

    tile_state tile1;
    tile_state tile2;
    tile_state tile3;

    for (int y = 0; y < 3; y++){
        tile1 = board->objects[y][0];
        tile2 = board->objects[y][1];
        tile3 = board->objects[y][2];
        if (tile1 == tile2 && tile1 == tile3 && tile1 != FREE)
            return tile1;
    }

    for (int x = 0; x < 3; x++){
        tile1 = board->objects[0][x];
        tile2 = board->objects[1][x];
        tile3 = board->objects[2][x];
        if (tile1 == tile2 && tile1 == tile3 && tile1 != FREE)
            return tile1;
    }

    tile1 = board->objects[0][0];
    tile2 = board->objects[1][1];
    tile3 = board->objects[2][2];
    if (tile1 == tile2 && tile1 == tile3 && tile1 != FREE)
        return tile1;

    tile1 = board->objects[0][2];
    tile2 = board->objects[1][1];
    tile3 = board->objects[2][0];
    if (tile1 == tile2 && tile1 == tile3 && tile1 != FREE)
        return tile1;

    return FREE;
}

bool make_move(game_board *board, int position){
    int x = position%3;
    int y = (int)position/3;

    if (position < 0 || position > 9 || board->objects[y][x] != FREE)
        return false;

    board->objects[y][x] = board->player_move_O ? O : X;
    board->player_move_O = !board->player_move_O;
    return true;
}

void close_program(){
    char buffer[MAX_MSG_LEN + 1];
    sprintf(buffer, "%s: :%s", COMMAND_END, name);
    send(server_socket, buffer, MAX_MSG_LEN, 0);
    exit(0);
}

void check_game_status(){
    bool game_won = false;

    bool game_drawn = true;
    for (int x = 0; x < 9; x++)
        for (int y = 0; y < 3; ++y)
            if (client_game_board.objects[y][x] == FREE){
                game_drawn = false;
                break;
            }

    tile_state winning_tile = check_winning_sign(&client_game_board);
    if (winning_tile != FREE){
        if ((client_is_O && winning_tile == O) || (!client_is_O && winning_tile == X)){
            printf("You won!\n");
        } else {
            printf("You lost!\n");
        }

        game_won = true;
    }

    if (game_won || game_drawn)
        current_state = QUIT;

    if (game_drawn && !game_won)
        printf("Draw!\n");

}


void start_game(){
    while (true){

        if (current_state!=START && current_state!=WAIT_FOR_OPPONENT && current_state!=OPPONENT_MOVE){
            printf("\n");
            draw_board();
        }

        if (current_state == START)
            manage_start();
        else if (current_state == WAIT_FOR_OPPONENT)
            manage_wait_for_opponent();
        else if (current_state == WAIT_FOR_MOVE)
            manage_wait_for_move();
        else if (current_state == OPPONENT_MOVE)
            manage_opponent_move();
        else if (current_state == MOVE)
            manage_move();
        else if (current_state == QUIT)
            close_program();
    }
}

game_board init_board(){
    game_board board = {true,{FREE, FREE, FREE, FREE, FREE, FREE, FREE, FREE, FREE}};
    return board;
}


void draw_board(){
    char symbol;

    for (int y = 0; y < 3; y++){
        printf("|");
        for (int x = 0; x < 3; x++){
            if (client_game_board.objects[y][x] == X)
                symbol = 'X';
            else if (client_game_board.objects[y][x] == O)
                symbol = 'O';
            else
                symbol = '-';

            printf("%c|", symbol);
        }
        printf("\n");
    }
}

void manage_start(){
    if (strcmp(arg, ARG_NAME_USED) == 0)
        exit(1);
    else if (strcmp(arg, ARG_NO_OPPONENT) == 0)
        current_state = WAIT_FOR_OPPONENT;
    else{
        client_game_board = init_board();
        client_is_O = arg[0] == 'O';
        current_state = client_is_O ? MOVE : WAIT_FOR_MOVE;
    }
}

void manage_wait_for_opponent(){
    printf("Waiting for opponent\n");
    pthread_mutex_lock(&mutex);

    while (current_state != START && current_state != QUIT)
        pthread_cond_wait(&cond, &mutex);

    pthread_mutex_unlock(&mutex);

    client_game_board = init_board();
    client_is_O = arg[0] == 'O';
    current_state = client_is_O ? MOVE : WAIT_FOR_MOVE;
}

void manage_wait_for_move(){
    printf("Waiting for opponent move\n");

    pthread_mutex_lock(&mutex);
    while (current_state != OPPONENT_MOVE && current_state != QUIT)
        pthread_cond_wait(&cond, &mutex);

    pthread_mutex_unlock(&mutex);
}

void manage_opponent_move(){
    int pos = (int)strtol(arg, NULL, 10);
    make_move(&client_game_board, pos);
    check_game_status();
    if (current_state != QUIT)
        current_state = MOVE;
}

void manage_move(){
    int pos;
    do{
        printf("Make move (%c)[1-9]: ", client_is_O ? 'O' : 'X');
        scanf("%d", &pos);
        pos--;
    } while (!make_move(&client_game_board, pos));

    char buffer[MAX_MSG_LEN + 1];
    sprintf(buffer, "%s:%d:%s", COMMAND_MOVE, pos, name);
    send(server_socket, buffer, MAX_MSG_LEN, 0);

    check_game_status();
    if (current_state != QUIT)
        current_state = WAIT_FOR_MOVE;
}

void listen_to_server() {
    bool game_thread_run = false;
    while (true) {
        recv(server_socket, buffer, MAX_MSG_LEN, 0);
        parse_command(buffer);

        pthread_mutex_lock(&mutex);
        if (strcmp(command, COMMAND_PING) == 0){
            sprintf(buffer, "%s: :%s", COMMAND_RE_PING, name);
            send(server_socket, buffer, MAX_MSG_LEN, 0);
        } else if (strcmp(command, COMMAND_ADD) == 0) {
            current_state = START;
            if (!game_thread_run) {
                pthread_t t;
                pthread_create(&t, NULL, (void *(*)(void *)) start_game, NULL);
                game_thread_run = true;
            }
        } else if (strcmp(command, COMMAND_END) == 0){
            current_state = QUIT;
            exit(0);
        } else if (strcmp(command, COMMAND_MOVE) == 0){
            current_state = OPPONENT_MOVE;
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }
}

void connect_inet(char* ip_and_port){

    struct addrinfo *info;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char* ip = strtok(ip_and_port, ":");
    char* port = strtok(NULL, ":");

    if(getaddrinfo(ip, port, &hints, &info) != 0){
        perror("Error occured\n");
        exit(1);
    }

    server_socket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

    if (connect(server_socket, info->ai_addr, info->ai_addrlen) == -1){
        fprintf(stderr, "Error while connecting to INET socket (%s)\n", strerror(errno));
        exit(1);
    }

    freeaddrinfo(info);
}

void connect_local(char* path){
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    struct sockaddr_un sock_addr;
    memset(&sock_addr, 0, sizeof(struct sockaddr_un));
    sock_addr.sun_family = AF_UNIX;
    strcpy(sock_addr.sun_path, path);

    if (connect(server_socket, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1) {
        printf("Error while connecting to LOCAL socket (%s)\n", strerror(errno));
        exit(1);
    }
}