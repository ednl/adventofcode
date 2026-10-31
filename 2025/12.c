/**
 * Advent of Code 2025
 * Day 12: Christmas Tree Farm
 * https://adventofcode.com/2025/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.85 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 8.13 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Input file params (program does not work on example file)
#define FNAME "../aocinput/2025-12-input.txt"
#define SHAPES 6    // shapes in input file
#define TREES 1000  // trees in input file
#define FSIZE ((SHAPES << 4) + TREES * 25)  // 16 char per shape, 25 char per tree

typedef struct vec {
    int x[SHAPES];
} Vec;

static char input[FSIZE];
static Vec shape, tree;

// Read 2-digit base 10 number at s
static int readnum(const char *s)
{
    return *s * 10 + *(s + 1) - 11 * '0';
}

// Sum of elements
static int sum(const Vec a)
{
    int val = 0;
    for (int i = 0; i < SHAPES; ++i)
        val += a.x[i];
    return val;
}

// Sum of element-wise product
static int dot(const Vec a, const Vec b)
{
    int val = 0;
    for (int i = 0; i < SHAPES; ++i)
        val += a.x[i] * b.x[i];
    return val;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Count parts (=minimum area) per shape
    const char *c = input;
    for (int i = 0; i < SHAPES; ++i, ++c) {
        c += 3;  // skip digit, ':', newline
        for (int j = 0; j < 12; ++j, ++c)
            shape.x[i] += *c == '#';
    }

    int always = 0, maybe = 0;
    for (int i = 0; i < TREES; ++i) {

        // Parse tree
        const int w = readnum(c);
        const int h = readnum(c + 3);
        c += 7;  // skip two digits, 'x', two digits, ':', space
        for (int j = 0; j < SHAPES; ++j, c += 3)  // skip 2 digits, space/newline
            tree.x[j] = readnum(c);

        // Evaluate tree
        const int blocks = (w / 3) * (h / 3);  // round down before multiplying
        if (sum(tree) <= blocks) {
            // All presents will fit under the tree even with 3x3 block tiling
            ++always;
        } else if (dot(shape, tree) <= w * h) {
            // Perfectly interlocked presents will fit under the tree
            // Finding that tiling is an NP-hard problem
            ++maybe;
        }// else
            // Impossible to fit even with perfect, gapless tiling
            // ++never;  // = TREES - always - maybe
    }
    printf("%u %u\n", always, maybe);  // 521 0

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
