/**
 * Advent of Code 2024
 * Day 4: Ceres Search
 * https://adventofcode.com/2024/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 04.c
 *    gcc   -std=gnu17 -Wall -Wextra 04.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 04.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 04.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <string.h>  // memcpy
#include <stdint.h>  // int32_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE
    #define FNAME "../aocinput/2024-04-example.txt"
    #define N 10  // square char matrix in example file
#else
    #define FNAME "../aocinput/2024-04-input.txt"
    #define N 140  // square char matrix in input file
#endif

// Interpret 4 chars as 32-bit int (little-endian)
#define XMAS INT32_C(0x53414d58)  // *(int *)"XMAS"
#define SAMX INT32_C(0x584d4153)  // *(int *)"SAMX"

// Tell compiler int pointer is unaligned
typedef int32_t unaligned_int32_t __attribute__((aligned(1)));

static char input[N * (N + 1)];
static char grid1[N][N];
static char grid2[N][N];

// Rotate left by 90 degrees: (x,y) -> (139-y,x)
static char *rot(char *const restrict dst, const char *const restrict src)
{
    for (int y = 0; y < N; ++y) {
        const int dx = N - 1 - y;
        const int dy = N * y;
        for (int x = 0; x < N; ++x)
            dst[N * x + dx] = src[dy + x];
    }
    return dst;
}

static int count(const char *a)
{
    int n = 0;
    for (int i = 0; i < N; ++i) {  // for each row
        const char *const end = a + N - 4;  // end of row
        while (a < end) {
            switch (*(unaligned_int32_t *)a) {
                case XMAS:
                case SAMX:
                    ++n;
                    a += 3;
                    break;
                default:
                    ++a;
            }
        }
        a = end + 4;
    }
    return n;
}

static void show(char a[N][N])
{
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            putchar(a[i][j]);
        putchar('\n');
    }
    putchar('\n');
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Read input file
    FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

    for (int i = 0; i < N; ++i)
        memcpy(&grid1[i][0], &input[i * (N + 1)], N);

    // Part 1
    int sum = count(&grid1[0][0]);
    sum += count(rot(&grid2[0][0], &grid1[0][0]));
    show(grid1);
    show(grid2);
    printf("Part 1: %d\n", sum);  // example: 18, input: ?

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
