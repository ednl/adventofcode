/**
 * Advent of Code 2025
 * Day 7: Laboratories
 * https://adventofcode.com/2025/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.87 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 2.41 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 4.31 µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-07-input.txt"
#define N 141          // grid columns
#define M (N + 1)      // grid rows
#define HALF (N >> 1)  // col index of 'S', also number of splitter rows (ex: 7, inp: 70)
#define SPLIT '^'

typedef uint32_t u32;
typedef uint64_t u64;

// M rows, M-1 columns +newline
static char grid[M][M];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(grid, sizeof grid, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Partial vertical sums of the Plinko game
    // https://en.wikipedia.org/wiki/Galton_board
    u64 galton[N] = {0};
    galton[HALF] = 1;  // start with one tachyon beam at 'S'

    // Pascal's Triangle but with columns for every x-coordinate,
    // not just a hex grid for the nodes (splitters or plinko pegs)
    u32 splits = 0;                                  // part 1: number of splitters hit with a beam
    u32 col = HALF, end = HALF + 1;                  // start/stop columns of Pascal's triangle
    for (u32 i = 2; i < M; col--, end++, i += 2)     // peg row on grid
        for (u32 j = col; j < end; j += 2)           // only look at triangle, not whole square
            if (grid[i][j] == SPLIT && galton[j]) {  // splitter and beam in this column?
                splits++;                            // part 1: beam has hit a splitter
                galton[j - 1] += galton[j];          // (may already have value)
                galton[j + 1] += galton[j];          // (may already have value)
                galton[j] = 0;                       // peg shadow
            }

    // Part 2: all possible tachyon beam paths
    u64 worlds = 0;
    for (u32 j = 0; j < N; ++j)
        worlds += galton[j];
    printf("%u %"PRIu64"\n", splits, worlds);  // 1598 4509723641302

    // Part 2: bottom-up, collect beams at splitters, sum will be in S
    // Simpler and faster but impossible to combine with part 1
    // for (int i = 0; i < N; ++i)
    //     galton[i] = 1;
    // for (int i = N - 1, col = 1, end = N - 1; i > 0; col++, end--, i -= 2)
    //     for (int j = col; j < end; j += 2)
    //         if (grid[i][j] == SPLIT)
    //             galton[j] = galton[j - 1] + galton[j + 1];
    // printf("%"PRIu64"\n", galton[HALF]);  // 4509723641302

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
