#include "utils.h"

int sem_id;
int shm_table_id;

table_t *table;

void init();
void clean();
int take_pizza();
void sigint_handler(){ exit(0); }


int main() {
    printf("Nowy dostawca %d\n", getpid());

    init();
    atexit(clean);
    signal(SIGINT, sigint_handler);

    while (1) {

        lock(sem_id, TABLE_SEM);
        if (table->pizza_amount > 0) {

            int pizza_type = take_pizza();
            printf("-DOSTAWCA: (pid: %d timestamp: %s) Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), timestamp(), pizza_type, table->pizza_amount);
            unlock(sem_id, TABLE_SEM);

            sleep(PIZZA_DELIVERY_TIME);
            printf("-DOSTAWCA: (pid: %d timestamp: %s) Dostarczam pizze: %d\n", getpid(), timestamp(), pizza_type);
            sleep(RETURN_TIME);

        } else {
            unlock(sem_id, TABLE_SEM);
        }

    }
    return 0;
}

void init(){
    sem_id = get_sem(get_sem_key());
    shm_table_id = get_shm(get_table_key());

    table = attach_shm(shm_table_id);

    srand(getpid());
}

void clean(){
    deattach_shm(table);
}

int take_pizza()
{
    int pizza_type = table->array[table->take_index];
    table->array[table->take_index] = -1;
    table->take_index++;
    table->take_index %= TABLE_SIZE;
    table->pizza_amount--;
    return pizza_type;
}