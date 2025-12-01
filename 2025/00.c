/**
 * Advent of Code 2025
 * Day ?: ?
 * https://adventofcode.com/2025/day/?
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -lm -std=c17 -Wall -Wextra -pedantic ??.c
 * Enable timer:
 *    cc -lm -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c ??.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#define AOCDAY "??"

#define AOCPRE "../aocinput/2025-"AOCDAY"-"
#define AOCSUF ".txt"
#if EXAMPLE == 1
    #define FNAME AOCPRE"example"AOCSUF
    #define N 10  // adjust for actual input
#else
    #define FNAME AOCPRE"input"AOCSUF
    #define N 1000  // adjust for actual input
#endif
#define FSIZE (N * 10)  // adjust for actual input

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int part1 = 0;
    // TODO
    printf("Part 1: %d\n", part1);

    int64_t part2 = 0;
    // TODO
    printf("Part 2: %"PRId64"\n", part2);

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
