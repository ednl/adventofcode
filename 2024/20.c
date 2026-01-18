/**
 * Advent of Code 2024
 * Day 20: Race Condition
 * https://adventofcode.com/2024/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 20.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 20.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-20-example.txt"
    #define N 15
#else
    #define FNAME "../aocinput/2024-20-input.txt"
    #define N 141
#endif
#define FSIZE (N * (N + 1))

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
    printf("Time: %.0f ns\n", stoptimer_us());
#endif
    return 0;
}
