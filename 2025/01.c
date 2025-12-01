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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 19 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define AOCYEAR 2025
#define AOCDAY  01
#define EXAMPLE 1

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define AOCPRE "../aocinput/"STR(AOCYEAR)"-"STR(AOCDAY)"-"
#define AOCSUF ".txt"

#if EXAMPLE == 0
    #define FNAME AOCPRE"example"AOCSUF
    #define FSIZE 38  // example file size in bytes
#else
    #define FNAME AOCPRE"input"AOCSUF
    #define FSIZE 19650  // input file size in bytes
#endif

#define START 50
#define SIZE 100

typedef struct dial {
    int val, div, mod;
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
    Dial cur = {.val=START, .div=START / SIZE, .mod=START % SIZE};
    for (const char *c = input; c != end; ++c) {
        // L = anti-clockwise = negative, R = clockwise = positive
        const int dir = *c++ == 'L' ? -1 : 1;
        // Convert ascii to int
        int turn = *c++ & 15;
        while (*c != '\n')
            turn = turn * 10 + (*c++ & 15);
        // Save old dial, set new dial
        const Dial prev = cur;
        cur.val += turn * dir;
        cur.div = cur.val / 100;
        cur.mod = cur.val % 100;

        // Part 1
        zero1 += !cur.mod;

        //    -200  -100    0    100   200
        // -----+-----+-----+-----+-----+----
        // 2222221111110000000000011111122222

        // Part 2
        zero2 += (cur.div - prev.div) * dir;
        if ((dir == 1 && cur.val <= 0) || (dir == -1 && cur.val >= 0))
            // increasing non-positive, or decreasing non-negative
            zero2 += !cur.mod - !prev.mod;
        else if ((prev.val < 0 && cur.val > 0) || (cur.val < 0 && prev.val > 0))
            // across zero
            zero2 += 1 - !prev.mod;
    }
    printf("%d %d\n", zero1, zero2);  // example: 3 6, input: 1180 6892

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
