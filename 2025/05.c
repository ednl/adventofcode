/**
 * Advent of Code 2025
 * Day 5: Cafeteria
 * https://adventofcode.com/2025/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 70 µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-05-example.txt"
    #define N 4
    #define M 6
    #define FSIZE (N * 6 + M * 3)
#else
    #define FNAME "../aocinput/2025-05-input.txt"
    #define N 182
    #define M 1000
    #define FSIZE (N * 32 + M * 16)
#endif

typedef struct range {
    uint64_t a, b;
} Range;

static char input[FSIZE];
static Range range[N];
static uint64_t id[M];

static int cmprange(const void *p, const void *q)
{
    const Range *r1 = p;
    const Range *r2 = q;
    if (r1->a < r2->a) return -1;
    if (r1->a > r2->a) return  1;
    if (r1->b < r2->b) return -1;
    if (r1->b > r2->b) return  1;
    return 0;
}

static int cmpid(const void *p, const void *q)
{
    const uint64_t a = *(const uint64_t *)p;
    const uint64_t b = *(const uint64_t *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static uint64_t readnum(const char **const s)
{
    uint64_t x = 0;
    while (**s >= '0' && **s <= '9')
        x = x * 10 + (*(*s)++ & 15);  // next digit
    (*s)++;  // skip '-' or '\n'
    return x;
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse input file
    const char *c = input;
    for (int i = 0; i < N; ++i) {
        range[i].a = readnum(&c);
        range[i].b = readnum(&c);
    }
    c++;  // skip empty line
    for (int i = 0; i < M; ++i)
        id[i] = readnum(&c);

    qsort(range, N, sizeof *range, cmprange);
    qsort(id   , M, sizeof *id   , cmpid   );

    // Merge ranges
    int n = N;
    {
        int i = 0;
        for (int j = 1; j < n; ) {
            for (; j < n && range[i].b + 1 >= range[j].a; ++j)
                if (range[j].b > range[i].b)
                    range[i].b = range[j].b;
            if (j < n)
                range[++i] = range[j++];
        }
        n = i + 1;  // n is new size, index 0..n-1, non-overlapping and non-touching ranges
    }

    int fresh = 0;
    for (int i = 0, j = 0; i < M && j < n; ++j) {
        for (; i < M && id[i] <  range[j].a; ++i);
        for (; i < M && id[i] <= range[j].b; ++i)
            ++fresh;
    }
    printf("%d\n", fresh);  // example: 3, input: 739

    uint64_t allfresh = 0;
    for (int i = 0; i < n; ++i)
        allfresh += range[i].b - range[i].a + 1;
    printf("%"PRIu64"\n", allfresh);  // example: 14, input: 344486348901788

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
