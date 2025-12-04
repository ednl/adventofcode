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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 709 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2025-04-example.txt"
    #define N 10
#else
    #define FNAME "../aocinput/2025-04-input.txt"
    #define N 140
#endif

static char grid[N + 2][N + 3];  // +border, +'\0'

#if EXAMPLE
static void show(void)
{
    for (int i = 1; i <= N; ++i)
        printf("%*s", N + 1, &grid[i][1]);  // includes newline
}
#endif

static int forklift(const bool remove)
{
    int count = 0;
    for (int i = 1; i < N + 1; ++i)
        for (int j = 1; j < N + 1; ++j)
            if (grid[i][j] == '@') {
                const int y0 = i - 1, y1 = i + 2;
                const int x0 = j - 1, x1 = j + 2;
                int sum = 0;
                for (int y = y0; y < y1; ++y)
                    for (int x = x0; x < x1; ++x)
                        sum += grid[y][x] == '@';
                if (sum < 5) {  // includes centre roll
                    ++count;
                    if (remove)
                        grid[i][j] = '.';
                }
            }
    return count;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 1; i <= N && fgets(&grid[i][1], N + 2, f); ++i);  // +"\n\0"
    fclose(f);
#if EXAMPLE
    show();
#endif

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    printf("%d\n", forklift(false));  // example: 13, input: 1495

    // Part 2
    int total = 0, prev;
    do {
        prev = total;
        total += forklift(true);
    } while (total != prev);
#if EXAMPLE
    show();
#endif
    printf("%d\n", total);  // example: 43, input: 8768

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
