/**
 * Advent of Code 2025
 * Day 6:
 * https://adventofcode.com/2025/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 22 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 52 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 79 µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-06-example.txt"
    #define H 4   // line count
    #define W 16  // line length incl. newline
    #define N 4   // problem count
    #define M 3   // problem size (3x3)
#else
    #define FNAME "../aocinput/2025-06-input.txt"
    #define H 5     // line count
    #define W 3740  // line length incl. newline
    #define N 1000  // problem count
    #define M 4     // problem size (4x4)
#endif
#define FSIZE (H * W)
#define MUL '*'
#define ADD '+'

static char input[H][W];
static uint64_t problem[N];
static bool mul[N];
static int col[N + 1];

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

    // Find operators
    const char *const op = &input[H - 1][0];
    for (int j = 0, n = 0; n < N; ++j)
        if (op[j] != ' ') {
            if (op[j] == MUL)
                problem[n] = mul[n] = 1;
            col[n++] = j;
        }
    col[N] = W;

    // Part 1: horizontal numbers
    for (int i = 0; i < M; ++i) {
        const char *const s = input[i];
        for (int n = 0; n < N; ++n) {
            int x = 0;
            const char *t = s + col[n];
            for (; *t == ' '; ++t);
            for (; *t >= '0' && *t <= '9'; ++t)
                x = x * 10 + (*t & 15);
            if (mul[n])
                problem[n] *= x;
            else
                problem[n] += x;
        }
    }
    uint64_t total = 0;
    for (int n = 0; n < N; ++n)
        total += problem[n];
    printf("%"PRIu64"\n", total);  // example: 4277556, input: 4648618073226

    // Part 2: vertical numbers
    total = 0;
    const char *s = &input[0][0];
    for (int n = 0; n < N; ++n) {
        uint64_t prob = mul[n];
        for (;;) {
            int x = 0;
            const char *const end = s + W * M;
            for (const char *t = s; t != end; t += W)
                if (*t >= '0' && *t <= '9')
                    x = x * 10 + (*t & 15);
            ++s;
            if (!x)
                break;
            if (mul[n])
                prob *= x;
            else
                prob += x;
        }
        total += prob;
    }
    printf("%"PRIu64"\n", total);  // example: 3263827, input: 7329921182115

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
