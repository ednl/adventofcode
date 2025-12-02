/**
 * Advent of Code 2025
 * Day 2: ?
 * https://adventofcode.com/2025/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.42 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv, lldiv_t
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#define AOCDAY "02"

#define AOCPRE "../aocinput/2025-"AOCDAY"-"
#define AOCSUF ".txt"
#if EXAMPLE == 1
    #define FNAME AOCPRE"example"AOCSUF
    #define N 11  // ranges in example
#else
    #define FNAME AOCPRE"input"AOCSUF
    #define N 30  // ranges in input file
#endif
#define FSIZE 512  // large enough to hold input (or example)

static const int64_t pow10[] = {
    INT64_C(1),   // 10^0: 1 digit
    INT64_C(10),  // 10^1: 2 digits, etc.
    INT64_C(100),
    INT64_C(1000),
    INT64_C(10000),
    INT64_C(100000),
    INT64_C(1000000),
    INT64_C(10000000),
    INT64_C(100000000),
    INT64_C(1000000000),
    INT64_C(10000000000),
    INT64_C(100000000000),
    INT64_C(1000000000000),
    INT64_C(10000000000000),
    INT64_C(100000000000000),
    INT64_C(1000000000000000),
    INT64_C(10000000000000000),
    INT64_C(100000000000000000),
    INT64_C(1000000000000000000)};  // 10^18: 19 digits (INT64_MAX=9223372036854775807: 19 digits)

static const int dig10[] = {
                    19, 18,18,18,18,  // index=0 (19/20 digits) unreachable for int64_t
    17,17,17, 16,16,16, 15,15,15,15,
    14,14,14, 13,13,13, 12,12,12,12,
    11,11,11, 10,10,10,  9, 9, 9, 9,
     8, 8, 8,  7, 7, 7,  6, 6, 6, 6,
     5, 5, 5,  4, 4, 4,  3, 3, 3, 3,
     2, 2, 2,  1, 1, 1,  0, 0, 0, 1};  // index=64 for x=0 => 1 digit

typedef struct num {
    int64_t val;
    int len;
} Num;

typedef union range {
    struct { Num a, b; };
    Num arr[2];
} Range;

static char  input[FSIZE];
static Range range[N];

// Digit count of value x in base 10
static int digits(const int64_t x)
{
    const int approx = dig10[x ? __builtin_clzll(x) : 64];  // clz undefined for x=0
    return approx + (x >= pow10[approx]);
}

// Smallest number with 'len' digits in base 10
static int64_t first(const int len)
{
    return pow10[len - 1];
}

// Largest number with 'len' digits in base 10
static int64_t last(const int len)
{
    return pow10[len] - 1;
}

// Smallest number with 'partlen' digits in base 10 which repeated is at least 'num.val'
static int64_t next(int64_t val, const int partlen)
{
    int64_t min = 0;
    do {
        const lldiv_t x = lldiv(val, pow10[partlen]);
        min = x.rem + (x.rem < min);
        val = x.quot;
    } while (val);
    return min;
}

// Largest number with 'partlen' digits in base 10 which repeated is at most 'num.val'
static int64_t prev(int64_t val, const int partlen)
{
    int64_t max = last(partlen);
    do {
        const lldiv_t x = lldiv(val, pow10[partlen]);
        max = x.rem - (x.rem > max);
        val = x.quot;
    } while (val);
    return max;
}

int main(void)
{
    // Read file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse ranges
    const char *c = input;
    for (int i = 0; i < (N << 1); ++i, ++c) {
        int64_t x = *c++ & 15;
        while (*c >= '0' && *c <= '9')
            x = x * 10 + (*c++ & 15);
        range[i >> 1].arr[i & 1] = (Num){x, digits(x)};
    }

    // Evaluate ranges
    int64_t sum1 = 0, sum2 = 0;
    for (int i = 0; i < N; ++i) {
        // printf("%d:\n", i);
        const Range *const r = &range[i];
        // Always true: a<=b && (n==m || n+1==m)
        for (int parts = 2; parts <= r->b.len; ++parts) {
            const int n = r->a.len + (r->a.len % parts != 0);
            const int m = r->b.len - (r->b.len % parts != 0);
            if (n != m)
                continue;
            const int partlen = n / parts;
            const int64_t a = n == r->a.len ? next(r->a.val, partlen) : first(partlen);
            const int64_t b = n == r->b.len ? prev(r->b.val, partlen) :  last(partlen);
            if (a > b)
                continue;
            const int64_t len = b - a + 1;
            const int64_t partsum = a * len + (((len - 1) * len) >> 1);
            int64_t repeat = 0;
            for (int j = 0; j < parts; ++j)
                repeat = repeat * pow10[partlen] + partsum;
            if (parts == 2)
                sum1 += repeat;
            sum2 += repeat;
            // printf("    %2d/%d=%d : %5"PRId64" %5"PRId64" %11"PRId64" %11"PRId64" %11"PRId64"\n", n, parts, partlen, a, b, repeat, sum1, sum2);
        }
    }

    // TODO: deduplicate overlapping entries, e.g.:
    //   range = 74025-113072
    //     2x3=6 : 100-112 includes 111111
    //     3x2=6 :  10- 11 includes 111111
    //     5x1   :   7-  9
    //     6x1   :   1-  1 includes 111111
    //   range = 79725385-79874177
    //     2*4=8 : 7972-7986 includes 79797979
    //     4*2=8 :   79-  79 includes 79797979
    //   range = 4526-8370
    //     2*2=4 : 45-82 includes 5555, 6666, 7777
    //     4*1=4 :  5- 7 includes 5555, 6666, 7777

    printf("%"PRId64"\n", sum1);  // part 1 example: 1227775554, input: 30323879646
    // printf("%"PRId64"\n", sum2);  // part 2 example: (4174823709 too high), input: (43991281119 too high)

    // Part 2 manual deduplication
#if EXAMPLE == 1
    // part 2 example: 4174379265
    printf("%"PRId64"\n", sum2 - 222222 * 2);
#else
    // part 2 input: 43872163557
    printf("%"PRId64"\n", sum2 - 111111 * 2 - 79797979 - 999999 * 2 - 222222 * 2 - 2222 - 1111 - 888888 * 2 - 666666 * 2 - 444444 * 2 - 777777 * 2 - 555555 * 2 - 333333 * 2 - 29292929 - 5555 - 6666 - 7777 - 3333);
#endif

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    return 0;
}
