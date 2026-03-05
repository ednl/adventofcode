/**
 * Advent of Code 2017
 * Day 5: A Maze of Twisty Trampolines, All Alike
 * https://adventofcode.com/2017/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 31.9 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? ms
 *     Raspberry Pi 5 (2.4 GHz)      : 63.3 ms
 */

#include <stdio.h>
#include <string.h>  // memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-05-input.txt"
#define N 1090

static int inp[N];  // keep original for comparison ifdef DEBUG
static int vm1[N];
static int vm2[N];
#ifdef DEBUG
    static int count1[N];
    static int count2[N];
#endif

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    for (int i = 0; i < N && fscanf(f, "%d", &inp[i]) == 1; ++i);
    fclose(f);

    // Two copies to start fresh in part 2
    memcpy(vm1, inp, N * sizeof *inp);
    memcpy(vm2, inp, N * sizeof *inp);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    unsigned tick = 0;
    for (unsigned ip = 0; ip < N; ++tick) {
    #ifdef DEBUG
        count1[ip]++;
    #endif
        ip += vm1[ip]++;
    }
    printf("%u\n", tick);  // 388611

    // Part 2
    tick = 0;
    for (unsigned ip = 0; ip < N; ++tick) {
    #ifdef DEBUG
        count2[ip]++;
    #endif
        ip += vm2[ip] < 3 ? vm2[ip]++ : vm2[ip]--;
    }
    printf("%u\n", tick);  // 27763113

#ifdef DEBUG
    for (int i = 0; i < N; ++i) {
        printf("    %4d: %5d", i, inp[i]);
        if (inp[i] != vm1[i])
            printf(" %5d (%3d) %5d (%5d)", vm1[i], count1[i], vm2[i], count2[i]);
        putchar('\n');
    }
#endif

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
