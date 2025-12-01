/**
 * Advent of Code 2025
 * Day 1: ?
 * https://adventofcode.com/2025/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -lm -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *    cc -lm -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define AOCYEAR 2025
#define AOCDAY  01
#define EXAMPLE 1  // 0=input, 1=example1, 2=example2 etc.

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#if EXAMPLE == 1
    #define FNAME "../aocinput/"STR(AOCYEAR)"-"STR(AOCDAY)"-example.txt"
    #define N 10
#else
    #define FNAME "../aocinput/"STR(AOCYEAR)"-"STR(AOCDAY)"-input.txt"
    #define N 1000
#endif
#define FSIZE (N * 10)

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
