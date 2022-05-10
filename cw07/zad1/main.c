#include "utils.h"

int shm_oven_id;
int shm_table_id;
int sem_id;

void init();
void clean();
void init_oven(oven_t* oven);
void init_table(table_t* table);
void sigint_handler(){ exit(0); }

int main(int argc, char *argv[]){
    if (argc != 3){
        fprintf(stderr, "Provide N M\n");
        exit(1);
    }

    long N = strtol(argv[1], NULL, 10);
    long M = strtol(argv[2], NULL, 10);

    init();
    atexit(clean);
    signal(SIGINT, sigint_handler);

    for (int i = 0; i < N; i++){
        pid_t pid = fork();
        if (pid == 0){
            execl("./cook", "./cook", NULL);
        }
    }
    for(int i = 0; i < M; i++){
        pid_t pid = fork();
        if(pid == 0){
            execl("./supplier", "./supplier", NULL);
        }
    }

    int status;
    while (wait(&status) > 0);

    return 0;
}

void clean(){
    delete_sem(sem_id);
    delete_shm(shm_oven_id);
    delete_shm(shm_table_id);
}

void init(){

//    create sems
    sem_id = create_sem(get_sem_key());
    init_sem(sem_id);

//    create shms
    shm_oven_id = create_shm(get_oven_key(), OVEN_SEG_SIZE);
    shm_table_id = create_shm(get_table_key(), TABLE_SEG_SIZE);

    oven_t* oven = attach_shm(shm_oven_id);
    table_t* table = attach_shm(shm_table_id);
    init_oven(oven);
    init_table(table);
}

void init_oven(oven_t* oven){
    for (int i = 0; i < OVEN_SIZE; i++){
        oven->array[i] = -1;
    }
    oven->pizza_amount = 0;
    oven->put_index = 0;
    oven->take_index = 0;
}

void init_table(table_t* table){
    for (int i = 0; i < TABLE_SIZE; i++){
        table->array[i] = -1;
    }
    table->pizza_amount = 0;
    table->put_index = 0;
    table->take_index = 0;
}
