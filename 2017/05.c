/**
 * Advent of Code 2017
 * Day 5: A Maze of Twisty Trampolines, All Alike
 * https://adventofcode.com/2017/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable debug output:
 *     cc -DDEBUG 05.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Get input from other input file:
 *     ./a.out < input.txt
 *     cat input.txt | ./a.out
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 31.9 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? ms
 *     Raspberry Pi 5 (2.4 GHz)      : 63.3 ms
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <string.h>  // memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-05-input.txt"
#define N 1200  // needed for my input: 1090

static int n;
static int inp[N];
static int vm1[N];
static int vm2[N];
#ifdef DEBUG
    static int add[N];
    static int sub[N];
#endif

static int readinput(FILE *f)
{
    int i = 0;
    while (i < N && fscanf(f, "%d", &inp[i]) == 1)
        i++;
    return i;
}

static int part1(void)
{
    int tick = 0;
    for (int ip = 0; ip < n; ++tick)
        ip += vm1[ip]++;
    return tick;
}

static int part2(void)
{
    int tick = 0;
    for (int ip = 0; ip < n; ++tick)
        ip += vm2[ip] < 3 ? (
        #ifdef DEBUG
            add[ip]++,
        #endif
            vm2[ip]++) : (
        #ifdef DEBUG
            sub[ip]--,
        #endif
            vm2[ip]--);
    return tick;
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        FILE *f = fopen(FNAME, "r");
        if (!f) {
            fprintf(stderr, "File not found: "FNAME"\n");
            return 1;
        }
        n = readinput(f);
        fclose(f);
    } else
        n = readinput(stdin);

    if (!n) {
        fprintf(stderr, "Invalid input\n");
        return 2;
    }
#ifdef DEBUG
    printf("input length: %d\n", n);
#endif

    // Two copies to keep original for comparison
    memcpy(vm1, inp, n * sizeof *inp);
    memcpy(vm2, inp, n * sizeof *inp);

#ifdef TIMER
    starttimer();
#endif

    printf("%d\n", part1());  // 388611
    printf("%d\n", part2());  // 27763113

#ifdef TIMER
    const double us = stoptimer_us();
#endif

#ifdef DEBUG
    for (int i = 0; i < n; ++i) {
        printf("    %4d: %5d | %5d %+4d | %5d %+6d %+6d\n",
            i, inp[i], vm1[i], vm1[i] - inp[i], vm2[i], add[i], sub[i]);
    }
#endif

#ifdef TIMER
    printf("Time: %.0f us\n", us);
#endif
}
