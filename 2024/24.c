/**
 * Advent of Code 2024
 * Day 24: Crossed Wires
 * https://adventofcode.com/2024/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 24.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 24.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? ms
 *     Raspberry Pi 5 (2.4 GHz)                         : ? ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? ms
 *     Raspberry Pi 4 (1.8 GHz)                         : ? ms
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-24-example.txt"
    #define N 10
#else
    #define FNAME "../aocinput/2024-24-input.txt"
    #define N 1000
#endif
#define FSIZE (N * 10)

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file in one go
    fclose(f);

#ifdef TIMER
    starttimer();
#endif


#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
