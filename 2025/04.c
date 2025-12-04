/**
 * Advent of Code 2025
 * Day 4: Printing Department
 * https://adventofcode.com/2025/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE
    #define FNAME "../aocinput/2025-04-example.txt"
    #define N 10
#else
    #define FNAME "../aocinput/2025-04-input.txt"
    #define N 140
#endif

static char grid[N][N + 1];  // +'\n'

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(grid, sizeof grid, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int count1 = 0, count2 = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (grid[i][j] == '@') {
                int sum = 0;
                for (int y = i - 1; y < i + 2; ++y)
                    if (y >= 0 && y < N)
                        for (int x = j - 1; x < j + 2; ++x)
                            if (x >= 0 && x < N)
                                sum += grid[y][x] == '@';
                count1 += sum < 5;  // includes centre roll
            }
    printf("%d %d\n", count1, count2);  // example: 13 ?, input: 1495 ?

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
