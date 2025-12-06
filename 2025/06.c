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
 *     Mac Mini 2020 (M1 3.2 GHz)    : 45 µs
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
    #define ROWS 3   // 3 rows of numbers
    #define COLS 4   // 4 cols of numbers
    #define LLEN 16  // line length
#else
    #define FNAME "../aocinput/2025-06-input.txt"
    #define ROWS 4     // 4 rows of numbers
    #define COLS 1000  // 1000 cols of numbers
    #define LLEN 3740  // line length
#endif
#define DATASIZE (ROWS * LLEN)
#define FSIZE ((ROWS + 1) * LLEN)  // +1 row of operations

static char input[FSIZE];
static const char *grp[COLS + 1];  // start of grouped data, +1 for endpoint
static bool mul[COLS];

// Parse number horizontally (step=1) or vertically (step=LLEN)
static int readnum(const char *s, const int step)
{
    for (; *s == ' '; s += step);  // skip spaces
    int x = 0;
    for (; *s >= '0' && *s <= '9'; s += step)  // terminated by space or newline or operator
        x = x * 10 + (*s & 15);
    return x;
}

// Part 1: data grouped in columns, numbers horizontal
// Part 2: data grouped in columns, numbers vertical
static int64_t pivot(const bool ispart2)
{
    const int numberstep = ispart2 ? 1 : LLEN;
    const int digitstep  = ispart2 ? LLEN : 1;
    uint64_t total = 0;
    for (int i = 0; i < COLS; ++i) {
        uint64_t part = mul[i];  // 1 for mul=true (op='*'), 0 for mul=false (op='+')
        const char *pc = grp[i];  // start on row 0 of grouped data
        const char *const end = ispart2 ? grp[i + 1] - 1 : pc + DATASIZE;
        if (mul[i])
            for (; pc != end; pc += numberstep) part *= readnum(pc, digitstep);
        else
            for (; pc != end; pc += numberstep) part += readnum(pc, digitstep);
        total += part;
    }
    return total;
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

    // Find operators and grouped data column start
    {
        const char *pc = input + DATASIZE;  // skip data
        grp[0] = input;
        mul[0] = *pc++ == '*';
        for (int i = 1; i < COLS; ++i) {
            for (; *pc == ' '; ++pc);  // skip spaces
            grp[i] = pc - DATASIZE;
            mul[i] = *pc++ == '*';
        }
        grp[COLS] = input + LLEN;
    }

    // example | input
    // --------+--------------
    // 4277556 | 4648618073226
    // 3263827 | 7329921182115
    printf("%"PRIu64"\n%"PRIu64"\n", pivot(0), pivot(1));

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
