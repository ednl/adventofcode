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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  5.4 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-12-input.txt"
#define FSIZE 25096
#define SHAPES 6
#define BASE (('.' * 3 + '\n') * 3)  // shape with zero parts
#define DIFF ('.' - '#')  // one part
#define TREES 1000

static char input[FSIZE];
static int shape[SHAPES];
static int area[TREES];
static int list[TREES][SHAPES];

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

    // Parse input
    const char *c = input;
    for (int i = 0; i < SHAPES; ++i) {
        c += 3;  // skip digit, ':', newline
        int sum = 0;
        for (int j = 0; j < 12; ++j)
            sum += *c++;
        shape[i] = (BASE - sum) / DIFF;
        c++;  // skip blank line
    }
    for (int i = 0; i < TREES; ++i) {
        area[i] = ((*c & 15) * 10 + (*(c + 1) & 15)) * ((*(c + 3) & 15) * 10 + (*(c + 4) & 15));
        c += 7;  // skip "00x00: "
        for (int j = 0; j < SHAPES; ++j, c += 3)
            list[i][j] = (*c & 15) * 10 + (*(c + 1) & 15);
    }

    //
    int fits = 0;
    for (int i = 0; i < TREES; ++i) {
        int sum = 0;
        for (int j = 0; j < SHAPES; ++j)
            sum += list[i][j] * shape[j];
        // I mean, c'mon!!
        fits += sum < area[i];
    }
    printf("%d\n", fits);  // 521

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
