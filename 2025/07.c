/**
 * Advent of Code 2025
 * Day 7: Laboratories
 * https://adventofcode.com/2025/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  6 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-07-example.txt"
    #define N 15  // grid columns in example file
#else
    #define FNAME "../aocinput/2025-07-input.txt"
    #define N 141  // grid columns in input file
#endif
#define M (N + 1)
#define FSIZE (M * M)  // +bottom row, +'\n'
#define HALF (N >> 1)  // col index of 'S', also number of splitter rows (ex: 7, inp: 70)
#define SPLIT '^'

static char grid[M][M];

// Partial vertical sums of the Plinko game
// https://en.wikipedia.org/wiki/Galton_board
static int64_t plinko[N];

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(grid, sizeof grid, 1, f);  // read whole file at once
    fclose(f);

#if EXAMPLE
    fwrite(grid, sizeof grid, 1, stdout);
    printf("\n");
#endif

#ifdef TIMER
    starttimer();
#endif

    // Pascal's Triangle but with columns for every x-coordinate,
    // not just a hex grid for the splitter nodes (="plinko pegs")
    int split = 0, row = 2, col = HALF, end = HALF + 1;  // 'row' of grid, col/end=start/stop
    plinko[col] = 1;  // start with one tachyon beam at 'S'
    for (int i = 0; i < HALF; ++i, row += 2, --col, ++end)  // peg rows: i,next
        for (int j = col; j < end; ++j)
            if (grid[row][j] == SPLIT && plinko[j]) { // is there a splitter and at least one beam in this column?
                    ++split;  //  part 1: beam has hit a splitter
                    plinko[j - 1] += plinko[j];  // may already have value
                    plinko[j + 1] += plinko[j];
                    plinko[j] = 0;  // peg shadow
                }
    int64_t worlds = 0;
    for (int j = 0; j < N; ++j)
        worlds += plinko[j];
    printf("%d %"PRId64"\n", split, worlds);  // example: 21 40, input: 1598 4509723641302

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
