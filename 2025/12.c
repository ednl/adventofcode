/**
 * Advent of Code 2025
 * Day 12: Christmas Tree Farm
 * https://adventofcode.com/2025/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 8.3 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ?   µs
 *     Raspberry Pi 5 (2.4 GHz)      : ?   µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-12-example.txt"
    #define FSIZE 149
    #define TREES 3
#else
    #define FNAME "../aocinput/2025-12-input.txt"
    #define FSIZE 25096
    #define TREES 1000
#endif
#define SHAPES 6

// Threshold percentage of area for shapes to fit trees
// I mean, c'mon!!
#define BAR 85

static char input[FSIZE];
static int shape[SHAPES];

// Read 1- or 2-digit number, advance char pointer
static int readnum(const char **s)
{
    int x = *(*s)++ & 15;
    if (**s >= '0' && **s <= '9')
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int fits = 0;
    const char *c = input;
    for (int i = 0; i < SHAPES; ++i) {
        c += 3;  // skip digit, ':', newline
        for (int j = 0; j < 12; ++j)
            shape[i] += *c++ == '#';
        c++;  // skip blank line
    }
    for (int i = 0; i < TREES; ++i) {
        int avail = readnum(&c);
        c++;  // skip 'x'
        avail *= readnum(&c);
        c += 2;  // skip ':' and space
        int need = 0;
        for (int j = 0; j < SHAPES; ++j, ++c)  // also skip space or newline
            need += readnum(&c) * shape[j];
        fits += need * 100 / avail < BAR;
    }
    printf("%d\n", fits);  // example: 2, input: 521

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
