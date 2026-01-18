/**
 * Advent of Code 2025
 * Day 5: Cafeteria
 * https://adventofcode.com/2025/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05thr.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05thr.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 49 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>     // qsort, bsearch, size_t
#include <stdint.h>     // uint64_t
#include <inttypes.h>   // PRIu64
#include <pthread.h>    // pthread_create, pthread_join
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-05-input.txt"
#define RANGECOUNT 182  // 182 ranges in input file
#define IDCOUNT   1000  // 1000 IDs in input file
#define FSIZE (RANGECOUNT * 32 + IDCOUNT * 16)  // all numbers have <=15 digits

#define THREADS 2  // 2 is optimum; more is only slower
#define STEP (IDCOUNT / THREADS)  // can't have remainder

typedef struct range {
    uint64_t a, b;
} Range;

static char input[FSIZE];
static Range ranges[RANGECOUNT];
static uint64_t ids[IDCOUNT];
static pthread_t tid[THREADS];  // thread IDs
static int rangecount;

// Parse number, advance char pointer 1 past last digit
static uint64_t readnum(const char **const s)
{
    uint64_t x = 0;
    while (**s >= '0' && **s <= '9')
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

// Parallel execution in separate threads
static void *loop(void *arg)
{
    const size_t end = (size_t)arg + STEP;
    size_t fresh = 0;
    for (size_t i = (size_t)arg; i < end; ++i)
        fresh += bsearch(&ids[i], ranges, rangecount, sizeof *ranges, inrange) != NULL;
    return (void *)fresh;
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
    for (int i = 0; i < RANGECOUNT; ++i) {
        ranges[i].a = readnum(&c);
        ranges[i].b = readnum(&c);
    }
    c++;  // skip empty line
    for (int i = 0; i < IDCOUNT; ++i)
        ids[i] = readnum(&c);

    // Sort and merge ranges
    qsort(ranges, RANGECOUNT, sizeof *ranges, cmprange);
    rangecount = mergeranges(ranges, RANGECOUNT);  // 78

    // Part 1
    for (size_t i = 0; i < THREADS; ++i)
        pthread_create(&tid[i], NULL, loop, (void *)(i * STEP));
    size_t sum = 0;
    void *fresh;
    for (size_t i = 0; i < THREADS; ++i) {
        pthread_join(tid[i], &fresh);
        sum += (size_t)fresh;
    }
    printf("%zu\n", sum);  // 739

    // Part 2
    uint64_t allfresh = 0;
    for (int i = 0; i < rangecount; ++i)
        allfresh += ranges[i].b - ranges[i].a + 1;
    printf("%"PRIu64"\n", allfresh);  // 344486348901788

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
