#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>

#define FPS 10

// tenhle struct bude slouzit k prenosu dat mezi thready
// pri volani funkce na spusteni thread mu muzes predat jeden void pointer (v tomhle pripade jsem tam dal struct data_passer)
typedef struct data_passer{
    int id;
    int run;
} data_passer_t;

void gameLoop(data_passer_t * passed_data, struct timespec *start, struct timespec *end, struct timespec *res){
    // az tady nadefinujem vsechny promenny
    unsigned i = 0;

    while (passed_data->run){
        clock_gettime(CLOCK_MONOTONIC, start);


        // sem prijed celej nas main loop
        //ty funkce nad a po printem drzej konstantni FPS
        printf("loop: %i\n", i++);
        // tady konci nas main loop


        clock_gettime(CLOCK_MONOTONIC, end);
        res->tv_nsec = 1000000000/FPS - (end->tv_nsec - start->tv_nsec);
        nanosleep(res, NULL);
    }
    // do passed_data vidi oba thready (to je ten voio pointer na struct)
    // oba while cykly se timto zastavi a program se vypne
    // passed_data->run = 0; pak muzem nabindovat treba na esc
    passed_data->run = 0;
}

void updateDisplay(data_passer_t * passed_data){
    unsigned i = 0;
    while (passed_data->run){
        // nonstop update displaye (uvidime jestli bude potreba prohazovani dvou bufferu, myslim ze spis ne)
    }
}

// funkce kterou predavam gameLoopThread
void* gameLoopThread(void * passed_data_void){
    data_passer_t * passed_data = (data_passer_t *) passed_data_void;
    struct timespec start = {0,0};
    struct timespec end = {0,0};
    struct timespec res;
    gameLoop(passed_data, &start, &end, &res);
    return NULL;
}

// funkce kterou predavam displayUpdateThread
void* displayUpdateThread(void * passed_data_void){
    data_passer_t * passed_data = (data_passer_t *) passed_data_void;
    updateDisplay(passed_data);
    return NULL;
}

int main (){
    data_passer_t * passed_data = malloc(sizeof(data_passer_t));
    passed_data->id = 0;
    passed_data->run = 1;

    pthread_t tid0;
    pthread_t tid1;

    // musime pointery pretypovat na void a pak zas zpatky na data_passer_t
    pthread_create(&tid0, NULL, gameLoopThread, (void *)passed_data);
    pthread_create(&tid1, NULL, displayUpdateThread, (void *)passed_data);

    pthread_exit(NULL);
    return 0;
}
