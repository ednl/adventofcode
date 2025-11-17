/**
 * Advent of Code 2024
 * Day 25: Code Chronicle
 * https://adventofcode.com/2024/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 25.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 25.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  6.29 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 10 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 27 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 83 µs
 */

#include <stdio.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-25-example.txt"
    #define N 5
#else
    #define FNAME "../aocinput/2024-25-input.txt"
    #define N 500
#endif

#define N2 ((N >> 1) + EXAMPLE)  // half locks, half keys (odd count in example)
#define M 5  // 5x5 grid of '#' or '.': fits in 32-bit integer as bit pattern
// +2 rows at top/bottom, +1 col newline, +empty lines in between
#define FSIZE (N * (M + 2) * (M + 1) + N - 1)

static char input[FSIZE];
static uint32_t lock[N2], key[N2];
static int locks, keys;

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file in one go
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    const char *c = input;
    for (int i = 0; i < N; ++i) {
        uint32_t *const pat = *c == '#' ? &lock[locks++] : &key[keys++];  // grid as bit pattern in lock or key
        c += M + 1;  // skip row of all '#' (lock) or all '.' (key)
        for (int j = 0; j < M; ++j, ++c)  // pattern rows, +newline
            for (int k = 0; k < M; ++k)   // pattern cols
                *pat = *pat << 1 | (*c++ == '#');
        c += M + 2;  // skip row of all '.' (lock) or all '#' (key), +newline, +empty line
    }

    int fit = 0;
    for (int i = 0; i < locks; ++i)
        for (int j = 0; j < keys; ++j)
            fit += !(lock[i] & key[j]);
    printf("%d\n", fit);  // example: 3, input: 3284

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    return 0;
}
