/**
 * Advent of Code 2018
 * Day 1: Chronal Calibration
 * https://adventofcode.com/2018/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :   ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 178 µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <limits.h>  // INT_MAX
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-01-input.txt"
#define FSIZE 4096  // needed for my input: 3567
#define N 989  // lines in input file

static char input[FSIZE];
static int freq[N + 1];  // start with 0
static int modf[N];  // part 1: steps, part 2: freq mod shift

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse, part 1
    const char *c = input;
    for (int i = 0; i < N && *c; i++, c++) {
        int x = 0;
        const int sgn = *c++ == '+' ? 1 : -1;
        while (*c & 32)
            x = x * 10 + (*c++ & 15);
        freq[i + 1] = freq[i] + (modf[i] = sgn * x);
    }
    printf("%d\n", freq[N]);  // 439

    // Part 2
    const int shift = freq[N];  // value shift at start of new loop
    for (int i = 0; i < N; ++i)
        modf[i] = freq[i] % shift;  // remainder of cumulative frequency by overall shift
    // Check every pair of identical remainders
    // (disregards possibility of duplicate frequency on the very first loop)
    // (also disregards possibility of shift <= 0)
    int firstdup = 0, minsteps = INT_MAX;
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            if (modf[i] == modf[j]) {
                // Number of steps to reach the duplicate frequency
                // (disregards possibility of f[i] >= f[j], but this never happens for my input)
                const int steps = (freq[j] - freq[i]) / shift * N + i;
                if (steps < minsteps) {
                    minsteps = steps;    // lowest number of steps so far
                    firstdup = freq[j];  // first duplicated frequency (largest of f[i] and f[j])
                }
            }
    printf("%d\n", firstdup);  // 124645

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
