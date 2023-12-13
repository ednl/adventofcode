/**
 * Advent of Code 2023
 * Day 13: Point of Incidence
 * https://adventofcode.com/2023/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march-native -Wall 12.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march-native -Wall 12.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=50000;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini      : 104 µs
 *     Raspberry Pi 5 2.4 GHz : 158 µs
 */

#include <stdio.h>     // fopen, fclose, getline, printf, fputc, stdout
#include <stdlib.h>    // free
#include <stdbool.h>   // bool
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-13-example.txt"
#define N 9
#else
#define NAME "../aocinput/2023-13-input.txt"
#define N 17
#endif

static int map[N], rot[N];
static bool transposed;

static int stoi(const char* s)
{
    int val = 0;
    while (*s)
        val = val << 1 | (*s++ & 1);  // '.'=0, '#'=1
    return val;
}

// static void putbin(const int val, int len)
// {
//     while (len > 0)
//         fputc('0' + (val >> --len & 1), stdout);
// }

static int rev(int val, int len)
{
    int n = 0;
    while (len-- > 0) {
        n = n << 1 | (val & 1);
        val >>= 1;
    }
    return n;
}

static int min(const int a, const int b)
{
    return a < b ? a : b;
}

static int max(const int a, const int b)
{
    return a > b ? a : b;
}

// 0 < pos < len
static int imperfections(const int val, const int len, const int pos)
{
    const int shift1 = len - pos;
    const int shift2 = max(shift1 - pos, 0);
    const int minlen = min(pos, shift1);
    const int mask = (1 << minlen) - 1;
    const int a = val >> shift1;
    const int b = rev(val >> shift2, minlen);
    return __builtin_popcount((unsigned)((a ^ b) & mask));
}

static int findmirror(const int* mat, const int rows, const int cols, const int imperf)
{
    for (int j = 1; j < cols; ++j) {  // try mirror before column j
        int i = 0, count = 0;
        while (i < rows && count <= imperf)
            count += imperfections(mat[i++], cols, j);
        if (count == imperf)
            return j;
    }
    return 0;
}

static void transpose(const int* src, int* dst, const int rows, const int cols)
{
    for (int j = 0; j < cols; ++j) {
        int n = 0;
        for (int i = 0; i < rows; ++i)
            n = n << 1 | (src[i] >> (cols - j - 1) & 1);
        *dst++ = n;
    }
}

static int summarize(const int rows, const int cols, const int imperf)
{
    int m = findmirror(map, rows, cols, imperf);
    if (m)
        return m;
    if (!transposed) {
        transpose(map, rot, rows, cols);
        transposed = true;
    }
    return 100 * findmirror(rot, cols, rows, imperf);
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    int len;
    char* buf = NULL;
    size_t bufsz;
    int part1 = 0, part2 = 0, rows = 0, cols = 0;
    while ((len = (int)getline(&buf, &bufsz, f)) > 0) {
        buf[--len] = '\0';  // remove newline
        if (len) {
            cols = len;
            map[rows++] = stoi(buf);
        } else {
            transposed = false;
            part1 += summarize(rows, cols, 0);
            part2 += summarize(rows, cols, 1);
            rows = cols = 0;
        }
    }
    free(buf);
    transposed = false;
    part1 += summarize(rows, cols, 0);
    part2 += summarize(rows, cols, 1);

    printf("Part 1: %d\n", part1);  // example: 405, input: 31739
    printf("Part 2: %d\n", part2);  // example: 400, input: 31539
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
