/**
 * Advent of Code 2025
 * Day 5: Cafeteria
 * https://adventofcode.com/2025/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 45.5 µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t, INT64_MAX
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-05-input.txt"
#define N 256   // needed for my input: 182 +1 for sentinel
#define M 1024  // needed for my input: 1000
#define FSIZE (N * 32 + M * 16)  // all numbers max. 15 digits

typedef struct range {
    int64_t a, b;
} Range;

static char input[FSIZE];
static Range ranges[N];
static int64_t ids[M];

// Parse number, advance char pointer 1 past last digit
static int64_t readnum(const char **const s)
{
    int64_t x = 0;
    while (**s & 16)
        x = x * 10 + (*(*s)++ & 15);  // next digit
    (*s)++;  // skip '-' or '\n'
    return x;
}

// Qsort helper: sort ids[] ascending
// Also works to sort ranges[] by first field
static int cmp_u64(const void *p, const void *q)
{
    const int64_t *a = p, *b = q;
    if (*a < *b) return -1;
    if (*a > *b) return  1;
    return 0;
}

// Merge ranges in-place in array 'r' of size 'len'
// which must already be sorted by lower bound ascending
// Returns new len (index=0..len-1) of non-overlapping and non-touching ranges
static int mergeranges(Range *const r, const int len)
{
    int i = 0;
    for (int j = 1; j < len; ++j) {
        if (r[i].b >= r[j].b)  // fully contained?
            continue;          // skip range
        if (r[i].b >= r[j].a)  // overlapping or touching?
            r[i].b = r[j].b;   // merge ranges
        else
            r[++i] = r[j];     // new range
    }
    return i + 1;              // new length
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

    // Parse input file
    int n = 0, m = 0;
    const char *s = input;
    for (; *s & 16; ++n) {  // until '\n'
        ranges[n].a = readnum(&s);
        ranges[n].b = readnum(&s) + 1;  // open-ended range
    }
    s++;  // skip empty line
    for (; *s; ++m)  // until '\0'
        ids[m] = readnum(&s);

    // Sort ids, sort and merge ranges
    qsort(ranges, n, sizeof *ranges, cmp_u64);
    qsort(ids, m, sizeof *ids, cmp_u64);
    n = mergeranges(ranges, n);
    ranges[n] = (Range){INT64_MAX, INT64_MAX};  // sentinel

    // Part 1
    int fresh = 0;
    for (int i = 0, j = 0; i < M; ++i) {
        for (; ids[i] >= ranges[j].b; ++j);  // j may be n
        fresh += ids[i] >= ranges[j].a;
    }
    printf("%u ", fresh);  // 739

    // Part 2
    int64_t allfresh = 0;
    for (int i = 0; i < n; ++i)
        allfresh += ranges[i].b - ranges[i].a;
    printf("%"PRIu64"\n", allfresh);  // 344486348901788

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
