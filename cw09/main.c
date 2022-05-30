#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#define REINDEER_AMOUNT 9
#define MINMAX_REINDEERS_WAITING 9
#define MIN_REINDEER_HOLIDAY_TIME 5
#define MAX_REINDEER_HOLIDAY_TIME 10

#define MIN_SANTA_DELIVERING_PRESENTS_TIME 1
#define MAX_SANTA_DELIVERING_PRESENTS_TIME 2

#define DELIVERIES_AMOUNT 3
int deliveries = 0;

int reindeers_waiting_amount = REINDEER_AMOUNT;
pthread_mutex_t mutex_reindeers_amount = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_reindeers_amount = PTHREAD_COND_INITIALIZER;

bool reindeers_delivering = false;
pthread_mutex_t mutex_delivering = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_delivering = PTHREAD_COND_INITIALIZER;

bool reindeers_waiting = true;
pthread_mutex_t mutex_waiting = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_waiting = PTHREAD_COND_INITIALIZER;


int rand_int_in_range(int min, int max);
void* santa(void* arg);
void* reindeer(void* arg);

void increase_reindeer_amount();
void decrease_reindeer_amount();
void notify_reindeer_amount_reached();

void set_reindeers_delivering(bool);
void notify_reindeers_delivering();

void set_reindeers_waiting(bool);
void notify_reindeers_waiting();

int main(){
    srand(time(NULL));

//    santa thread
    pthread_t santa_pthread;
    pthread_create(&santa_pthread, NULL, &santa, NULL);

//    reindeer thread
    int* reindeersIDs;
    pthread_t* reindeers_pthreads;
    if ((reindeersIDs = calloc(REINDEER_AMOUNT, sizeof(int)))==NULL ||
        (reindeers_pthreads = calloc(REINDEER_AMOUNT, sizeof(pthread_t))) == NULL){
        perror("Error allocating memory\n");
        exit(1);
    }
    for (int i = 0; i < REINDEER_AMOUNT; ++i) {
        reindeersIDs[i] = i;
        pthread_create(&reindeers_pthreads[i], NULL, &reindeer, &reindeersIDs[i]);
    }

//    wait for threads to finish
    pthread_join(santa_pthread, NULL);

    for (int i = 0; i < REINDEER_AMOUNT; i++)
        pthread_join(reindeers_pthreads[i], NULL);

    return 0;
}

void* santa(void* arg){

    while (true){

//        sleep until reindeers come back
        printf("Mikołaj: zasypiam\n");
        pthread_mutex_lock(&mutex_reindeers_amount);
        while (reindeers_waiting_amount < REINDEER_AMOUNT){
            pthread_cond_wait(&cond_reindeers_amount, &mutex_reindeers_amount);
        }
        pthread_mutex_unlock(&mutex_reindeers_amount);
        printf("Mikołaj: budzę się\n");

//        deliver toys
        set_reindeers_delivering(true);
        set_reindeers_waiting(false);

        printf("Mikołaj: dostarczam zabawki\n");
        sleep(rand_int_in_range(MIN_SANTA_DELIVERING_PRESENTS_TIME, MAX_SANTA_DELIVERING_PRESENTS_TIME));
        deliveries++;

        if (deliveries >= 3) break;

        set_reindeers_waiting(true);
        set_reindeers_delivering(false);

    }

    exit(0);
}

void* reindeer(void* arg){
//    todo
    int selfID = *(int*)arg;

    while (true){

//        go on holiday
        decrease_reindeer_amount();
        sleep(rand_int_in_range(MIN_REINDEER_HOLIDAY_TIME, MAX_REINDEER_HOLIDAY_TIME));

//        come back to north pole and wait for santa
//        increase_reindeer_amount();
        pthread_mutex_lock(&mutex_reindeers_amount);
        reindeers_waiting_amount++;
        printf("Renifer: czeka %d reniferów na mikołaja, ID=%d\n", reindeers_waiting_amount, selfID);
        if (reindeers_waiting_amount == REINDEER_AMOUNT){
            printf("Renifer: wybudzam Mikołaja, ID=%d\n", selfID);
            notify_reindeer_amount_reached();
        }
        pthread_mutex_unlock(&mutex_reindeers_amount);

        pthread_mutex_lock(&mutex_waiting);
        while (reindeers_waiting){
            pthread_cond_wait(&cond_waiting, &mutex_waiting);
        }
        pthread_mutex_unlock(&mutex_waiting);

//        deliver presents with santa
        pthread_mutex_lock(&mutex_delivering);
        while (reindeers_delivering){
            pthread_cond_wait(&cond_delivering, &mutex_delivering);
        }
        pthread_mutex_unlock(&mutex_delivering);

    }
}

void increase_reindeer_amount(){
    pthread_mutex_lock(&mutex_reindeers_amount);
    reindeers_waiting_amount++;
    pthread_mutex_unlock(&mutex_reindeers_amount);
}

void decrease_reindeer_amount(){
    pthread_mutex_lock(&mutex_reindeers_amount);
    reindeers_waiting_amount--;
    pthread_mutex_unlock(&mutex_reindeers_amount);
}

void notify_reindeer_amount_reached(){
    pthread_cond_broadcast(&cond_reindeers_amount);
}

void set_reindeers_delivering(bool value){
    pthread_mutex_lock(&mutex_delivering);
    reindeers_delivering = value;
    notify_reindeers_delivering();
    pthread_mutex_unlock(&mutex_delivering);
}

void notify_reindeers_delivering(){
    pthread_cond_broadcast(&cond_delivering);
}

void set_reindeers_waiting(bool value){
    pthread_mutex_lock(&mutex_waiting);
    reindeers_waiting = value;
    notify_reindeers_waiting();
    pthread_mutex_unlock(&mutex_waiting);
}

void notify_reindeers_waiting(){
    pthread_cond_broadcast(&cond_waiting);
}

int rand_int_in_range(int min, int max){
    return rand() % (max - min + 1) + min;
}