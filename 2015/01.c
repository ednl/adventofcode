/**
 * Advent of Code 2015
 * Day 1: Not Quite Lisp
 * https://adventofcode.com/2015/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  1.63 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ?    µs
 *     Raspberry Pi 5 (2.4 GHz)      : 16.8  µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2015-01-input.txt"
#define FSIZE 7000  // input file size (no newline)

static char input[FSIZE];
static const char *const end = input + FSIZE;

// Up: '(' = 40 => +1
// Dn: ')' = 41 => -1
static int updown(const char *dir)
{
    return 1 - ((*dir & 1) << 1);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int floor = 0, basement = 0;         // elevator analogies
    const char *dir = input;             // direction: '('= up, ')'=down
    for (; !basement; ++dir)
        if ((floor += updown(dir)) < 0)
            basement = dir - input + 1;  // 1-based position
    for (; dir != end; ++dir)
        floor += updown(dir);
    printf("%d %d\n", floor, basement);  // input: 138 1771

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
