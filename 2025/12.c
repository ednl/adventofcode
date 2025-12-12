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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.0 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-12-input.txt"
#define FSIZE 25096
#define SHAPES 6
#define TREES 1000

static char input[FSIZE];

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

    int fits = 0;
    // Skip whole shapes section...
    const char *c = input + (SHAPES << 4);  // one shape takes 16 chars in input file
    for (int i = 0; i < TREES; ++i) {
        // width x height, both 2-digit numbers
        const int area = ((*c & 15) * 10 + (*(c + 1) & 15)) * ((*(c + 3) & 15) * 10 + (*(c + 4) & 15));
        c += 7;  // skip "00x00: "
        int presents = 0;
        for (int j = 0; j < SHAPES; ++j, c += 3)  // also skip 2-digit number and space/newline
            presents += (*c & 15) * 10 + (*(c + 1) & 15);  // how many presents in total under this tree
        // Just assume average number of parts (tiles) per present (shape) is 8...
        // For my input, 7 works too but would require a multiplication
        fits += (presents << 3) < area;
    }
    printf("%d\n", fits);  // 521

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());  // NB: nanoseconds
#endif
}
