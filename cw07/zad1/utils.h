#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

#define OVEN_SIZE 5
#define TABLE_SIZE 5

#define SEM_ID 1
#define OVEN_ID 2
#define TABLE_ID 3

#define OVEN_SEM 0
#define TABLE_SEM 1
#define NUMBER_OF_SEMS 2

#define SEM_INIT_VAL 1

#define PIZZA_PREP_TIME 2
#define PIZZA_BAKING_TIME 4
#define PIZZA_DELIVERY_TIME 4
#define RETURN_TIME 4

struct oven_t{
    int array[OVEN_SIZE];
    int put_index;
    int take_index;
    int pizza_amount;
};
typedef struct oven_t oven_t;

struct table_t{
    int array[TABLE_SIZE];
    int put_index;
    int take_index;
    int pizza_amount;
};
typedef struct table_t table_t;

#define OVEN_SEG_SIZE sizeof(oven_t)
#define TABLE_SEG_SIZE sizeof(table_t)

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
} arg;

// keys
key_t get_key(int id);
key_t get_table_key();
key_t get_oven_key();
key_t get_sem_key();

// sems
int create_sem(key_t key);
int get_sem(key_t key);
void init_sem(int sem_id);
void init_sem_num(int sem_id, int sem_num);
void lock(int sem_id, int sem_num);
void unlock(int sem_id, int sem_num);
void delete_sem(int sem_id);

// shms
int create_shm(key_t key, int size);
int get_shm(key_t key);
void* attach_shm(int shm_id);
void deattach_shm(void* addr);
void delete_shm(int shm_id);

// other
int randint(int min, int max);
char* timestamp();