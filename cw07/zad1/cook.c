#include "utils.h"

int sem_id;
int shm_oven_id;
int shm_table_id;

oven_t *oven;
table_t *table;

void init();
void clean();
void put_pizza_to_oven(int pizza_type);
void place_pizza_on_table(int pizza_type);
int take_pizza_out();
void sigint_handler(){ exit(0); }

int main() {
    printf("Nowy kucharz: %d\n", getpid());

    init();
    atexit(clean);
    signal(SIGKILL, sigint_handler);

    while (1) {
        int pizza_type = randint(0, 9);
        printf("-KUCHARZ: (pid: %d timestamp: %s) Przygotowuje pizze: %d\n", getpid(), timestamp(), pizza_type);
        sleep(PIZZA_PREP_TIME);

        bool put = false;
        while (!put) {
            lock(sem_id, OVEN_SEM);
            if (oven->pizza_amount < OVEN_SIZE) {
                put_pizza_to_oven(pizza_type);
                put = true;
            }
            unlock(sem_id, OVEN_SEM);
        }

        printf("-KUCHARZ: (pid: %d timestamp: %s) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), timestamp(), pizza_type, oven->pizza_amount);
        sleep(PIZZA_BAKING_TIME);

        bool placed = false;
        while (!placed) {
            lock(sem_id, TABLE_SEM);
            if (table->pizza_amount < TABLE_SIZE) {

                lock(sem_id, OVEN_SEM);
                pizza_type = take_pizza_out(oven);
                unlock(sem_id, OVEN_SEM);

                place_pizza_on_table(pizza_type);
                placed = true;
            }

            unlock(sem_id, TABLE_SEM);
        }

        printf("-KUCHARZ (pid: %d timestamp: %s) Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), timestamp(), pizza_type, oven->pizza_amount, table->pizza_amount);
    }
    return 0;
}

void init(){
    sem_id = get_sem(get_sem_key());
    shm_oven_id = get_shm(get_oven_key());
    shm_table_id = get_shm(get_table_key());

    oven = attach_shm(shm_oven_id);
    table = attach_shm(shm_table_id);

    srand(getpid());
}

void clean(){
    deattach_shm(oven);
    deattach_shm(table);
}

void put_pizza_to_oven(int pizza_type)
{
    oven->array[oven->put_index] = pizza_type;
    oven->put_index++;
    oven->put_index %= OVEN_SIZE;
    oven->pizza_amount++;
}

void place_pizza_on_table(int pizza_type)
{
    table->array[table->put_index] = pizza_type;
    table->put_index++;
    table->put_index %= TABLE_SIZE;
    table->pizza_amount++;
}

int take_pizza_out()
{
    int pizza_type = oven->array[oven->take_index];
    oven->array[oven->take_index] = -1;
    oven->take_index++;
    oven->take_index %= OVEN_SIZE;
    oven->pizza_amount--;
    return pizza_type;
}

