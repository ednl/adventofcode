/**
 * Advent of Code 2025
 * Day 1: Secret Entrance
 * https://adventofcode.com/2025/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 12 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 20 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 53 µs
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

#define START 50  // dial starts at 50
#define SIZE 100  // dial goes from 0 to 99

typedef struct dial {
    int dial;  // unlimited dial value (any int, not just 0-99)
    int full;  // dial div SIZE (number of full turns, negative for left)
    int zero;  // dial mod SIZE == 0 (dial is at any multiple of 100)
} Dial;

static char input[FSIZE];
static const char *const end = input + FSIZE;

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int zero1 = 0, zero2 = 0;
    Dial cur = {
        .dial = START,
        .full = START / SIZE,
        .zero = START % SIZE == 0,
    };
    for (const char *c = input; c != end; ++c) {  // skip newline
        // L = anti-clockwise = negative, R = clockwise = positive
        const int dir = (*c++ & 3) - 1;  // 'L'=-1, 'R'=1
        // Convert ascii to int
        int turn = *c++ & 15;
        while (*c != '\n')
            turn = turn * 10 + (*c++ & 15);
        // Save old dial, set new dial
        const Dial old = cur;
        cur.dial += turn * dir;  // unlimited dial value
        cur.full = cur.dial / SIZE;  // negative for net left turns
        cur.zero = cur.dial % SIZE == 0;  // is at zero?

        // Part 1
        zero1 += cur.zero;

        // Part 2
        zero2 += (cur.full - old.full) * dir;

        // special cases
        //
        //    dial :   -200 -100   0   100  200
        //           ----+----+----+----+----+---
        // div 100 : 2222211111000000000111112222
        //
        //  R     cd<0   cd=0   cd>0
        // od<0  +cz-oz  +1-oz  +1-oz
        // od=0     -      -      0
        // od>0     -      -      0
        //
        //  L     cd<0   cd=0   cd>0
        // od<0     0      -      -
        // od=0     0      -      -
        // od>0   +1-oz  +1-oz +cz-oz

        if (dir * cur.dial < 0)
            // Increasing while negative, or decreasing while positive
            // (select first column for R, last column for L)
            zero2 += cur.zero - old.zero;
        else if (dir * old.dial < 0)
            // Went to or across zero
            // (select first row for R, last row for L)
            zero2 += !old.zero;
    }
    printf("%d %d\n", zero1, zero2);  // example: 3 6, input: 1180 6892

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
