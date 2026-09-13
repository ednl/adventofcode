/**
 * Advent of Code 2023
 * Day 13: Point of Incidence
 * https://adventofcode.com/2023/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 13.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 13.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 19.9 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 59.2 µs
*/

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-13-input.txt"
#define FSIZE 20480  // needed for my input: 17767
#define N 24  // max map dimensions, needed for my input: 17

static char input[FSIZE];
static int map[N], rot[N];
static bool transposed;

static int atobin(const char **s)
{
    unsigned val = *(*s)++ & 1;
    while (**s & 32)  // until '\n'
        val = val << 1 | (*(*s)++ & 1);  // '.'=0, '#'=1
    return val;  // leave *s pointing at newline to get length
}

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

static int findmirror(const int *mat, const int rows, const int cols, const int imperf)
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

static void transpose(const int *src, int *dst, const int rows, const int cols)
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
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int part1 = 0, part2 = 0;
    for (const char *c = input; *c; c++) {
        const char *const start = c;
        map[0] = atobin(&c);  // first row
        const int cols = c++ - start;  // set cols, skip newline
        int rows = 1;
        for (; *c & 32; c++)  // until LF or NUL
            map[rows++] = atobin(&c);
        transposed = false;
        part1 += summarize(rows, cols, 0);
        part2 += summarize(rows, cols, 1);
    }
    printf("%d %d\n", part1, part2);  // 31739 31539

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
