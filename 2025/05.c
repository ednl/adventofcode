/**
 * Advent of Code 2025
 * Day 5: Cafeteria
 * https://adventofcode.com/2025/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 17.8 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 48.6 µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort, bsearch
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-05-input.txt"
#define N 182   // 182 ranges
#define M 1000  // 1000 IDs
#define FSIZE (N * 32 + M * 16)  // all numbers max. 15 digits

typedef struct range {
    uint64_t a, b;
} Range;

static char input[FSIZE];
static Range ranges[N];
static uint64_t ids[M];

// Parse number, advance char pointer 1 past last digit
static uint64_t readnum(const char **const s)
{
    uint64_t x = 0;
    while (**s & 16)
        x = x * 10 + (*(*s)++ & 15);  // next digit
    (*s)++;  // skip '-' or '\n'
    return x;
}

// Qsort helper: sort Range[] first by .a ascending then by .b descending
static int cmprange(const void *p, const void *q)
{
    const Range *r1 = p;
    const Range *r2 = q;
    if (r1->a < r2->a) return -1;  // .a ascending
    if (r1->a > r2->a) return  1;
    if (r1->b < r2->b) return  1;  // .b descending
    if (r1->b > r2->b) return -1;
    return 0;
}

// Bsearch helper
// return -1 if key below range, +1 if key above range, 0 if key in range
static int inrange(const void *key, const void *range)
{
    const uint64_t k = *(const uint64_t *)key;
    const Range *r = range;
    if (k < r->a) return -1;
    if (k > r->b) return  1;
    return 0;
}

// Merge ranges in-place in array 'r' of size 'len' which must
// already be sorted, first by .a ascending then by .b descending
// Returns new len (index=0..len-1) of non-overlapping and non-touching ranges
static int mergeranges(Range *const r, const int len)
{
    int i = 0;
    for (int j = 1; j < len; ++j) {
        if (r[i].b >= r[j].b)  // fully contained?
            continue;  // skip range
        if (r[i].b + 1 >= r[j].a)  // overlapping or touching?
            r[i].b = r[j].b;  // merge ranges
        else
            r[++i] = r[j];  // new range
    }
    return i + 1;
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
    const char *s = input;
    for (int i = 0; i < N; ++i) {
        ranges[i].a = readnum(&s);
        ranges[i].b = readnum(&s);
    }
    s++;  // skip empty line
    for (int i = 0; i < M; ++i)
        ids[i] = readnum(&s);

    // Sort and merge ranges
    qsort(ranges, N, sizeof *ranges, cmprange);
    int n = mergeranges(ranges, N);

    // Part 1
    int fresh = 0;
    for (int i = 0; i < M; ++i)
        fresh += bsearch(&ids[i], ranges, n, sizeof *ranges, inrange) != NULL;
    printf("%u ", fresh);  // 739

    // Part 2
    uint64_t allfresh = 0;
    for (int i = 0; i < n; ++i)
        allfresh += ranges[i].b - ranges[i].a + 1;
    printf("%"PRIu64"\n", allfresh);  // 344486348901788

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
