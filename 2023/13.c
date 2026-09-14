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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 18.1 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 25.2 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 55.6 µs
*/

#include <stdio.h>
#include <stdint.h>  // uint32_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-13-input.txt"
#define FSIZE 20480  // needed for my input: 17767
#define N 24  // max map dimensions, needed for my input: 17

typedef uint32_t u32;  // convenience alias

static char input[FSIZE];
static u32 map[N], rot[N];
static bool transposed;

static u32 atobin(const char **s)
{
    u32 val = *(*s)++ & 1;
    while (**s & 32)  // until '\n'
        val = val << 1 | (*(*s)++ & 1);  // '.'=0, '#'=1
    return val;  // leave *s pointing at newline to get length
}

static u32 rev(u32 val, u32 len)
{
    u32 n = 0;
    while (len--) {
        n = n << 1 | (val & 1);
        val >>= 1;
    }
    return n;
}

// 0 < pos < len
static u32 imperfections(const u32 val, const u32 len, const u32 pos)
{
    const u32 shift1 = len - pos;
    const u32 shift2 = shift1 > pos ? shift1 - pos : 0;
    const u32 minlen = shift1 > pos ? pos : shift1;
    const u32 mask = (1u << minlen) - 1;
    const u32 a = val >> shift1;
    const u32 b = rev(val >> shift2, minlen);
    return __builtin_popcount((a ^ b) & mask);
}

static u32 findmirror(const u32 *mat, const u32 rows, const u32 cols, const u32 imperf)
{
    for (u32 j = 1; j < cols; ++j) {  // try mirror before column j
        u32 count = 0;
        for (u32 i = 0; i < rows && count <= imperf; ++i)
            count += imperfections(mat[i], cols, j);
        if (count == imperf)
            return j;
    }
    return 0;
}

static void transpose(const u32 *const restrict src, u32 *restrict dst, const u32 rows, const u32 cols)
{
    for (u32 j = 0; j < cols; ++j) {
        u32 n = 0;
        for (u32 i = 0; i < rows; ++i)
            n = n << 1 | (src[i] >> (cols - j - 1) & 1);
        *dst++ = n;
    }
}

static u32 summarize(const u32 rows, const u32 cols, const u32 imperf)
{
    const u32 m = findmirror(map, rows, cols, imperf);
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
for (u32 TIMERLOOP = 1000; TIMERLOOP--; ) {
#endif

    u32 part1 = 0, part2 = 0;
    for (const char *c = input; *c; c++) {
        const char *const start = c;
        map[0] = atobin(&c);  // first row
        const u32 cols = c++ - start;  // set cols, skip newline
        u32 rows = 1;
        for (; *c & 32; c++)  // until LF or NUL
            map[rows++] = atobin(&c);
        transposed = false;
        part1 += summarize(rows, cols, 0);
        part2 += summarize(rows, cols, 1);
    }
    printf("%u %u\n", part1, part2);  // 31739 31539

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
