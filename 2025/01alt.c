/**
 * Advent of Code 2025
 * Day 1: Secret Entrance
 * https://adventofcode.com/2025/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 01alt.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01alt.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 12 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-01-example.txt"
    #define FSIZE 38  // example file size in bytes
#else
    #define FNAME "../aocinput/2025-01-input.txt"
    #define FSIZE 19650 // input file size in bytes
#endif

// Dial starts at 50 and goes from 0 to 99
#define START 50
#define SIZE 100

static char input[FSIZE];
static const char *const end = input + FSIZE;

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int dial = START, zero1 = 0, zero2 = 0;
    for (const char *c = input; c != end; ++c) {
        const int prev = dial;
        const int dir = (*c++ & 3) - 1;  // 'L'=-1, 'R'=1
        int turn = *c++ & 15;
        while (*c != '\n')
            turn = turn * 10 + (*c++ & 15);
        zero2 += turn / SIZE;
        dial += turn % SIZE * dir;
        if (dial >= SIZE) {
            dial -= SIZE;
            zero2 += prev != 0;
        } else if (dial < 0) {
            dial += SIZE;
            zero2 += prev != 0;
        } else if (dial == 0) {
            zero2 += prev != 0;
        }
        zero1 += dial == 0;
    }
    printf("%d %d\n", zero1, zero2);  // example: 3 6, input: 1180 6892

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
