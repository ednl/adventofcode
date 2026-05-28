/**
 * Advent of Code 2020
 * Day 5: Binary Boarding
 * https://adventofcode.com/2020/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 39.1 µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-05-input.txt"
#define N 761  // lines in input file
#define L 10   // line length = 10x F/B/L/R (excl. newline)
#define FSIZE (N * (L + 1))  // +newline

static char input[FSIZE];
static unsigned seat[N];

// Qsort helper function: sort u32 ascending
static int cmp_u32_asc(const void *p, const void *q)
{
    const unsigned a = *(const unsigned *)p;
    const unsigned b = *(const unsigned *)q;
    if (a < b) return -1;
    if (a > b) return +1;
    return 0;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);  // known fixed size
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse
    const char *c = input;
    for (int i = 0; i < N; ++c, ++i) {  // skip newline
        unsigned id = 0;
        while (*c != '\n')
            id = id << 1 | !(*c++ & 4);  // F or L = 0, B or R = 1
        seat[i] = id;
    }

    // Part 1
    qsort(seat, N, sizeof *seat, cmp_u32_asc);
    printf("%u\n", seat[N - 1]);  // 861

    // Part 2
    for (int i = 1; i < N; ++i)
        if (seat[i] - 1 != seat[i - 1]) {  // gap before this seat?
            printf("%u\n", seat[i] - 1);  // 633
            break;
        }

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
