#include "dtc_sleep.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>


void 
dtc_sleep(struct timespec *ts_old, struct timespec *ts_wait)
{
    struct timespec ts_now;
    struct timespec ts_end;
    struct timespec ts_remaining;
    struct timespec ts_interrupt_remain;

    ts_end.tv_sec = ts_old->tv_sec + ts_wait->tv_sec;
    ts_end.tv_nsec = ts_old->tv_nsec + ts_wait->tv_nsec;

    if(clock_gettime(CLOCK_MONOTONIC, &ts_now) != 0){
        printf("***Error\n clock_gettime in dtc_sleep()\n");
        exit(-1);
    }

    ts_remaining.tv_sec = ts_end.tv_sec - ts_now.tv_sec;
    ts_remaining.tv_nsec = ts_end.tv_nsec - ts_now.tv_nsec;

    while (ts_remaining.tv_nsec > 1E9){
        ts_remaining.tv_sec++;
        ts_remaining.tv_nsec -= 1E9;
    }
    while (ts_remaining.tv_nsec < 0){
        ts_remaining.tv_sec--;
        ts_remaining.tv_nsec += 1E9;
    }

    if (ts_remaining.tv_sec < 0){
        printf("*** time elasped already !!!\n");
        return;
    }

    while (nanosleep(&ts_remaining, &ts_interrupt_remain) != 0){
        if (errno == EINTR) {
            // interrupted, continue here
            ts_remaining.tv_sec = ts_interrupt_remain.tv_sec;
            ts_remaining.tv_nsec = ts_interrupt_remain.tv_nsec;
        } else {
            printf("*** Error\n nanosleep in dtc_sleep()\n");
            exit(-1);
        }
    }

}

