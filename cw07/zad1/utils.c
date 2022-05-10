#include "utils.h"

// keys
key_t get_key(int id){
    key_t key;
    if ((key = ftok(getenv("HOME"), id)) == -1){
        perror("Error getting key");
        exit(1);
    }
    return key;
}

key_t get_table_key(){
    return get_key(TABLE_ID);
}

key_t get_oven_key(){
    return get_key(OVEN_ID);
}

key_t get_sem_key(){
    return get_key(SEM_ID);
}

// sems
int create_sem(key_t key){
    int sem_id;
    if ((sem_id = semget(key, NUMBER_OF_SEMS, IPC_CREAT | 0600)) == -1){
        perror("Error creating sem");
        exit(1);
    }
    return sem_id;
}

int get_sem(key_t key){
    int sem_id;
    if ((sem_id = semget(key, 0, 0)) == -1){
        perror("Error getting sem");
        exit(1);
    }
    return sem_id;
}

void init_sem(int sem_id){
    for (int i = 0; i < NUMBER_OF_SEMS; ++i) {
        init_sem_num(sem_id, i);
    }
}

void init_sem_num(int sem_id, int sem_num){
    arg.val = SEM_INIT_VAL;
    if (semctl(sem_id, sem_num, SETVAL, arg) == -1){
        perror("Error initializing sem");
        exit(1);
    }
}

void lock(int sem_id, int sem_num){
    struct sembuf sembuf;
    sembuf.sem_flg = 0;
    sembuf.sem_num = sem_num;
    sembuf.sem_op = -1;
    if (semop(sem_id, &sembuf, 1) == -1){
        perror("Error locking sem");
        exit(1);
    }
}

void unlock(int sem_id, int sem_num){
    struct sembuf sembuf;
    sembuf.sem_flg = 0;
    sembuf.sem_num = sem_num;
    sembuf.sem_op = 1;
    if (semop(sem_id, &sembuf, 1) == -1){
        perror("Error unlocking sem");
        exit(1);
    }
}

void delete_sem(int sem_id){
    semctl(sem_id, 0, IPC_RMID);
}

// shms
int create_shm(key_t key, int size){
    int shm_id;
    if ((shm_id = shmget(key, size, IPC_CREAT | 0600)) == -1){
        perror("Error creating shm");
        exit(1);
    }
    return shm_id;
}

int get_shm(key_t key){
    int shm_id;
    if ((shm_id = shmget(key, 0, 0)) == -1){
        perror("Error getting shm");
        exit(1);
    }
    return shm_id;
}

void* attach_shm(int shm_id){
    void* addr;
    if ((addr = shmat(shm_id, NULL, 0)) == (void*)-1){
        perror("Error attaching shm");
        exit(1);
    }
    return addr;
}

void deattach_shm(void* addr){
    shmdt(addr);
}

void delete_shm(int shm_id){
    shmctl(shm_id, IPC_RMID, NULL);
}

// other
int randint(int min, int max){
    return (rand() % (max-min)) + min;
}

char* timestamp(){
    char *result_time = calloc(140, sizeof(char));
    char time_str[128];
    int milliseconds;
    double fractional_seconds;
    struct timeval tv;

    gettimeofday(&tv, NULL);

    struct tm* tm = localtime(&tv.tv_sec);

    fractional_seconds = (double) tv.tv_usec;
    fractional_seconds /= 1e3;
    milliseconds = (int) fractional_seconds;

    strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%S", tm);
    sprintf(result_time, "%s.%03d", time_str, milliseconds);

    return result_time;
}