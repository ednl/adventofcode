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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.33 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 19.1  µs
 */

#include <stdio.h>
#include <stdlib.h>  // calloc, free
#include <limits.h>  // INT_MAX
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-01-input.txt"
#define FSIZE 4096  // needed for my input: 3567
#define N 989  // lines in input file
#define M 3    // max equivalent frequencies, needed for my input: 3

// Buckets for frequencies in same equivalence class
typedef struct equiv {
    int count;
    int index[M];
} Equiv;

static char input[FSIZE];
static int freq[N + 1];  // start with 0

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode required for fread
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    fread(input, 1, FSIZE, f);  // discard bytes read
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    const char *c = input;
    for (int i = 0; i < N && *c; i++, c++) {
        int x = 0;
        const int sgn = 44 - *c++;  // '+': 44-43=1, '-': 44-45=-1
        while (*c & 32)
            x = x * 10 + (*c++ & 15);
        freq[i + 1] = freq[i] + sgn * x;
    }
    printf("%d\n", freq[N]);  // 439

    // Part 2
    const int shift = freq[N];  // value shift at start of new loop (freq[0]=0)
    Equiv *const freqmod = calloc(shift, sizeof *freqmod);
    for (int i = 0; i < N; ++i)
        if (freq[i] >= 0) {  // disregard negative frequencies, works for my input
            Equiv *const m = &freqmod[freq[i] % shift];  // remainder of frequency by shift after one loop
            m->index[m->count++] = i;  // save index not frequency because needed for steps
        }
    int firstdup = 0, minsteps = INT_MAX;
    const Equiv *const end = freqmod + shift;
    for (const Equiv *m = freqmod; m != end; ++m)
        // Check every pair of identical remainders
        // disregard duplicate frequencies on first loop
        // disregard possibility of shift <= 0
        for (int i = 0; i < m->count - 1; ++i) {
            const int fi = freq[m->index[i]];
            for (int j = i + 1; j < m->count; ++j) {
                // Number of steps to reach duplicate frequency
                // disregard f[i] >= f[j], but this never happens for my input
                const int fj = freq[m->index[j]];
                const int steps = (fj - fi) / shift * N + m->index[i];  // add i because it occurs before j
                if (steps < minsteps) {
                    minsteps = steps;  // lowest number of steps so far
                    firstdup = fj;     // first duplicated frequency (largest of f[i] and f[j])
                }
            }
        }
    printf("%d\n", firstdup);  // 124645

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    free(freqmod);
}
