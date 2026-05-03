#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>
#include "../startstoptimer.h"

#define MAXTHREADS 2

typedef uint32_t u32;

typedef struct data {
    u32 start, stop, step, result;
} Data;

// Signal for other threads to exit when the desired value is found in one thread.
static atomic_bool found = false;

// Count up decimal number
// *first points to  most significant digit
//   last points to least significant digit
// Prereq: *first - 1 must always be valid address and contain '0'
static void incr(char **first, char *last, const u32 step)
{
    (*last) += step;       // start by increasing unit
    if (*last <= '9')
        return;
    *last -= 10;
    while (*last > '9') {  // cascade through nines
        *last-- = '0';     // set to zero and go to next power of ten
        (*last)++;         // add one (=ten)
    }
    if (last < *first)     // new digit?
        *first = last;
}

static void *loop(void *arg)
{
    Data *data = arg;
    char buf[8] = "0000000";
    char *first = &buf[sizeof buf - 2];
    char *last = first;
    incr(&first, last, data->start);
    for (u32 i = data->start; i < data->stop; i += data->step)
        incr(&first, last, data->step);
    data->result = atoi(buf);
    return NULL;
}

int main(void)
{
    starttimer();

    pthread_t tid[MAXTHREADS];  // thread IDs
    Data arg[MAXTHREADS];  // thread arguments going in and out

    for (u32 i = 0; i < MAXTHREADS; ++i)
        arg[i] = (Data){.start = i, .stop = 10 * 1000 * 1000, .step = MAXTHREADS};
    for (int i = 0; i < MAXTHREADS; ++i)
        pthread_create(&tid[i], NULL, loop, &arg[i]);
    for (int i = 0; i < MAXTHREADS; ++i)
        pthread_join(tid[i], NULL);

    printf("Time: %.0f us\n", stoptimer_us());  // incr: 19.7 ms, sprintf: 371 ms
}
