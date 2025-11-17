/**
 * Advent of Code 2024
 * Day 4: Ceres Search
 * https://adventofcode.com/2024/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    : 108 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 193 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 259 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 334 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 571 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 */

#include <stdio.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define F "../aocinput/2024-04-example.txt"
    #define N 10
#else
    #define F "../aocinput/2024-04-input.txt"
    #define N 140
#endif
#define M (N + 1)  // +newline
#define X (('M' + 'S') << 1)  // X-MAS corners for part 2

static char g[N][M];  // grid

int main(void)
{
    FILE *f = fopen(F, "rb");  // fread() requires binary mode
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(g, sizeof g, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();  // timing excludes file read from disk
#endif

    // Part 1
    int sum = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (g[i][j] == 'X') {
                sum +=            j > 2   && g[i  ][j-1] == 'M' && g[i  ][j-2] == 'A' && g[i  ][j-3] == 'S';
                sum +=            j < N-3 && g[i  ][j+1] == 'M' && g[i  ][j+2] == 'A' && g[i  ][j+3] == 'S';
                sum += i > 2   &&            g[i-1][j  ] == 'M' && g[i-2][j  ] == 'A' && g[i-3][j  ] == 'S';
                sum += i > 2   && j > 2   && g[i-1][j-1] == 'M' && g[i-2][j-2] == 'A' && g[i-3][j-3] == 'S';
                sum += i > 2   && j < N-3 && g[i-1][j+1] == 'M' && g[i-2][j+2] == 'A' && g[i-3][j+3] == 'S';
                sum += i < N-3 &&            g[i+1][j  ] == 'M' && g[i+2][j  ] == 'A' && g[i+3][j  ] == 'S';
                sum += i < N-3 && j > 2   && g[i+1][j-1] == 'M' && g[i+2][j-2] == 'A' && g[i+3][j-3] == 'S';
                sum += i < N-3 && j < N-3 && g[i+1][j+1] == 'M' && g[i+2][j+2] == 'A' && g[i+3][j+3] == 'S';
            }
    printf("%d\n", sum);  // example: 18, input: 2414

    // Part 2
    sum = 0;
    for (int i = 1; i < N - 1; ++i)
        for (int j = 1; j < N - 1; ++j)
            if (g[i][j] == 'A')
                sum += g[i-1][j-1] + g[i-1][j+1] + g[i+1][j-1] + g[i+1][j+1] == X && g[i-1][j-1] != g[i+1][j+1];
    printf("%d\n", sum);  // example: 9, input: 1871

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
