/**
 * Advent of Code 2024
 * Day 25: Code Chronicle
 * https://adventofcode.com/2024/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 25.c
 *    gcc   -std=gnu17 -Wall -Wextra 25.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 25.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 25.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  48 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
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
#define PAT 5          // 5x5 grid of '#' or '.'
#define ROW (PAT + 2)  // +2 rows at top/bottom
#define COL (PAT + 1)  // +newline
#define FSIZE (N * ROW * COL + N - 1)  // +empty lines in between
#define M ((N >> 1) + EXAMPLE)  // odd count in example

static char input[FSIZE];
static int lock[M][PAT];
static int key [M][PAT];
static int locks, keys;

#if EXAMPLE
static void show(const int *const a)
{
    printf("%d", a[0]);
    for (int i = 1; i < PAT; ++i)
        printf(",%d", a[i]);
}
#endif

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
        int *const a = *c == '#' ? lock[locks++] : key[keys++];  // pattern array
        c += COL;  // skip row of '#' (lock) or '.' (key)
        for (int j = 0; j < PAT; ++j, ++c)  // pattern rows, skip newline
            for (int k = 0; k < PAT; ++k)   // pattern cols
                a[k] += *c++ == '#';
        c += COL + 1;  // skip row of '.' (lock) or '#' (key), skip empty line
    }

#if EXAMPLE
    for (int i = 0; i < locks; ++i) {
        printf("lock %d: ", i);
        show(lock[i]);
        putchar('\n');
    }
    putchar('\n');
    for (int i = 0; i < keys; ++i) {
        printf("key  %d: ", i);
        show(key[i]);
        putchar('\n');
    }
    putchar('\n');
#endif

    int fit = 0;
    for (int i = 0; i < locks; ++i)
        for (int j = 0; j < keys; ++j) {
            int pin = 0;
            for (int k = 0; k < PAT; ++k)
                pin += lock[i][k] + key[j][k] <= PAT;
            fit += pin == PAT;
        #if EXAMPLE
            show(lock[i]); printf(" + "); show(key[j]); printf(" = %d (%d)\n", pin, fit);
        #endif
        }
#if EXAMPLE
    putchar('\n');
#endif
    printf("Part 1: %d\n", fit);  // example: 3, input: 3284

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
