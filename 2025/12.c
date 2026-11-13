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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.66 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 3.75 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 7.86 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Input file params (program does not work on example file)
#define FNAME "../aocinput/2025-12-input.txt"
#define SHAPES 6    // shapes in input file
#define TREES 1000  // trees in input file
// 16 chars per shape in input file, incl. newlines
// 25 chars per tree in input file, incl. newline
#define FSIZE (SHAPES * 16 + TREES * 25)

typedef struct vec {
    unsigned x[SHAPES];
} Vec;

static char input[FSIZE];

// Read 2-digit base 10 number at s
static unsigned readnum(const char *s)
{
    return *s * 10 + *(s + 1) - 11 * '0';
}

// Sum of elements
static unsigned sum(const Vec a)
{
    unsigned val = 0;
    for (unsigned i = 0; i < SHAPES; ++i)
        val += a.x[i];
    return val;
}

// Sum of element-wise product
static unsigned dot(const Vec a, const Vec b)
{
    unsigned val = 0;
    for (unsigned i = 0; i < SHAPES; ++i)
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
    Vec shape = {0};
    const char *s = input + 3;  // skip digit, ":\n"
    for (unsigned i = 0; i < SHAPES; s += 5, ++i)  // skip "\n\n", digit, ":\n"
        for (unsigned j = 0; j < (3U * 3 + 2); ++j)  // shape size 3x3 + 2x newline
            shape.x[i] += *s++;
    for (unsigned i = 0; i < SHAPES; ++i)
        shape.x[i] = ((9U * '.' + 2 * '\n') - shape.x[i]) / ('.' - '#');  // (434 - x) / 11

    Vec tree;
    int always = 0, never = 0, maybe = 0;
    s -= 3;  // correct for overshooting
    for (int i = 0; i < TREES; ++i) {

        // Parse tree
        const unsigned w = readnum(s);
        const unsigned h = readnum(s + 3);
        s += 7;  // skip two digits, 'x', two digits, ':', space
        for (unsigned j = 0; j < SHAPES; s += 3, ++j)  // skip 2 digits, space/newline
            tree.x[j] = readnum(s);

        // Evaluate tree
        const unsigned blocks = (w / 3) * (h / 3);  // round down before multiplying
        if (sum(tree) <= blocks) {
            // All presents will fit under the tree even with 3x3 block tiling
            always++;
        } else if (dot(shape, tree) > w * h) {
            // Impossible to fit even with perfect, gapless tiling
            never++;
        } else
            // Perfectly interlocked presents will fit under the tree
            // Finding that tiling is an NP-hard problem
            maybe++;  // maybe = TREES - always - never
    }
    printf("%u %u %u\n", always, maybe, never);  // 521 0 479

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
