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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.04 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  7.79 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 17.4  µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Input file params (program does not work on example file)
#define FNAME "../aocinput/2025-12-input.txt"
#define SHAPES 6  // number of shapes in input file
#define TREES 1000  // number of trees in input file
#define FSIZE ((SHAPES << 4) + TREES * 25)  // 16 char per shape, 25 char per tree

typedef struct vec {
    int x[SHAPES];
} Vec;

static char input[FSIZE];
static Vec shape, tree;

// Read 2-digit base 10 number at s
static int readnum(const char *s)
{
    return (*s & 15) * 10 + (*(s + 1) & 15);
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
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
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
            // ++never;
    }
    printf("%d %d\n", always, maybe);  // input: 521 0

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());  // NB: nanoseconds
#endif
}
