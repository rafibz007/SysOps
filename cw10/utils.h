#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 2*8
#define MAX_MSG_LEN 1024

#define COMMAND_MOVE "make_move"
#define COMMAND_END "close_program"
#define COMMAND_ADD "add"
#define COMMAND_RE_PING "re_ping"
#define COMMAND_PING "ping_clients"

#define ARG_NAME_USED "name_used"
#define ARG_NO_OPPONENT "no_opponent"

enum tile_state{
    O,
    X,
    FREE
};
typedef enum tile_state tile_state;

struct game_board{
    bool player_move_O;
    tile_state objects[3][3];
};
typedef struct game_board game_board;

enum game_state{
    GAME_INIT,
    NO_OPPONENT_YET,
    OPPONENT_MOVE,
    WAIT_FOR_OPPONENT_MOVE,
    YOUR_MOVE,
    EXIT
};
typedef enum game_state game_state;

struct client {
    char *username;
    bool is_responding;
    int fd;
};
typedef struct client client;
