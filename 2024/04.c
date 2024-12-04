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
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 332 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 357 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 360 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 820 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 */

#include <stdio.h>
#include <string.h>  // memcpy
#include <stdint.h>  // int32_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-04-example.txt"
    #define N 10  // square char matrix in example file
#else
    #define FNAME "../aocinput/2024-04-input.txt"
    #define N 140  // square char matrix in input file
#endif
#define FSIZE (N * (N + 1))   // input file size in bytes
#define SLEN 4                // search string length ("XMAS" or "SAMX")
#define SKIP (SLEN - 1)       // safety border
#define DIAGROWS (N * 2 - 1)  // row count of grid rotated by 45 degrees
#define DIAGSKIP (DIAGROWS - SKIP * 2)  // rows in diag that can hold search string

// Interpret 4 chars as 32-bit int (little-endian)
#define XMAS INT32_C(0x53414d58)  // *(int *)"XMAS"
#define SAMX INT32_C(0x584d4153)  // *(int *)"SAMX"
#define MM   INT32_C(0x004d004d)  // *(int *)"M_M_"
#define SM   INT32_C(0x004d0053)  // *(int *)"S_M_"
#define MS   INT32_C(0x0053004d)  // *(int *)"M_S_"
#define SS   INT32_C(0x00530053)  // *(int *)"S_S_"
#define MASK INT32_C(0x00ff00ff)  // blank out don't-care bytes

// Unaligned int pointer
typedef int32_t unaligned_int32_t __attribute__((aligned(1)));

static char input[FSIZE];
static char grid[N][N];
static char copy[N][N];
static char diag[DIAGROWS][N];

// Discrete rotation of 'src' (=grid or copy) left by 45 degrees: src(x,y) -> diag(x+y,N-1-x+y)
// Return pointer to first row with enough data to hold search string
static char *rot45(const char *const src)
{
    for (int y = 0; y < N; ++y) {
        const int dy = N * y;
        for (int x = 0; x < N; ++x)
            diag[N - 1 + y - x][(x + y) >> 1] = src[dy + x];
    }
    return &diag[SKIP][0];
}

// Sum of "XMAS" and "SAMX" count in rows of arr[rows][cols]
static int count(const char *arr, const int rows, const int cols)
{
    int sum = 0;
    for (int i = 0; i < rows; ++i) {  // for each row
    #if EXAMPLE
        int part = 0;  // count rows separately
        for (int j = 0; j < cols; ++j)  // print row
            putchar(arr[j] ? arr[j] : '_');
    #endif
        // End of row with room for 4 chars from a, including a
        const char *const end = arr + cols - SKIP;
        while (arr < end) {
            switch (*(unaligned_int32_t *)arr) {
            case XMAS:
            case SAMX:
                arr += 3;  // words can overlap by 1
                ++sum;
            #if EXAMPLE
                ++part;
            #endif
                break;
            default:
                ++arr;
            }
        }
        arr = end + SKIP;  // next row
    #if EXAMPLE
        if (part)
            printf(" %d", part);
        putchar('\n');
    #endif
    }
#if EXAMPLE
    printf("\n%d\n\n", sum);
#endif
    return sum;
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

    // Copy input to grid, skip newlines
    for (int i = 0; i < N; ++i)
        memcpy(&grid[i][0], &input[i * (N + 1)], N);
    // Rotate left by 90 degrees: grid(x,y) -> copy(N-1-y,x)
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            copy[j][N - 1 - i] = grid[i][j];

    // Part 1
    // Horizontal and vertical
    int sum = count(&grid[0][0], N, N) + count(&copy[0][0], N, N);
    // Rotate 'grid' left by 45 degrees, skip rows that are too short
    sum += count(rot45(&grid[0][0]), DIAGSKIP, N);
    // Rotate 'copy' left by 45 degrees, skip rows that are too short
    sum += count(rot45(&copy[0][0]), DIAGSKIP, N);
    printf("Part 1: %d\n", sum);  // example: 18, input: 2414

    // Part 2
    sum = 0;
    for (int i = 1; i < N - 1; ++i)
        for (int j = 1; j < N - 1; ++j)
            if (grid[i][j] == 'A') {  // search for central character
                // Read one byte past total size of 'grid' at last pass, which is
                // technically global-buffer-overflow but it's masked out anyway.
                const int32_t top = *(unaligned_int32_t *)&grid[i - 1][j - 1] & MASK;
                const int32_t btm = *(unaligned_int32_t *)&grid[i + 1][j - 1] & MASK;
                sum += (top == MM && btm == SS)
                    || (top == MS && btm == MS)
                    || (top == SM && btm == SM)
                    || (top == SS && btm == MM);
            }
    printf("Part 2: %d\n", sum);  // example: 9, input: 1871

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
