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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.00 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-12-input.txt"
#define SHAPES 6  // number of shapes in input file
#define TREES 1000  // number of trees in input file

#define SHAPETEXT (SHAPES << 4)  // 16 char per shape
#define FSIZE (SHAPETEXT + TREES * 25)  // 25 char per tree

static char input[FSIZE];  // input file (does not work for example file)

// Read 2-digit base 10 number at s
static int readnum(const char *s)
{
    return (*s & 15) * 10 + (*(s + 1) & 15);
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

    int fit = 0;
    const char *c = input + SHAPETEXT;  // skip all shapes
    for (int i = 0; i < TREES; ++i) {
        const int area = (readnum(c) / 3) * (readnum(c + 3) / 3);  // WxH per block of 3x3
        c += 7;  // skip to first shape count
        int presents = 0;
        for (int j = 0; j < SHAPES; ++j, c += 3)
            presents += readnum(c);
        fit += presents <= area;  // only count ones that will fit without interlocking
    }
    printf("%d\n", fit);  // input: 521 (does not work on example)

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
