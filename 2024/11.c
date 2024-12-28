/**
 * Advent of Code 2024
 * Day 11: Plutonian Pebbles
 * https://adventofcode.com/2024/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 11.c
 *    gcc   -std=gnu17 -Wall -Wextra 11.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 11.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 11.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :    ? ms
 *     Raspberry Pi 5 (2.4 GHz)                         :    ? ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 8.75 ms
 *     Raspberry Pi 4 (1.8 GHz)                         :    ? ms
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-11-example.txt"
    #define N 2
#else
    #define FNAME "../aocinput/2024-11-input.txt"
    #define N 8
#endif
#define BLINK1 25
#define BLINK2 75
#define CACHESIZE (1024 * 1024)

typedef struct vec {
    int64_t x, y;
} Vec;

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

static int64_t input[N];
static int64_t cache[CACHESIZE][BLINK2];

static int digits(const int64_t x)
{
    const int approx = dig10[x ? __builtin_clzll(x) : 64];  // clz undefined for x=0
    return approx + (x >= pow10[approx]);
}

static lldiv_t split(const int64_t x, const int even)
{
    return lldiv(x, pow10[even >> 1]);
}

static int64_t count(const int64_t stone, const int blink)
{
    if (!blink)
        return 1;
    if (stone < CACHESIZE && cache[stone][blink])
        return cache[stone][blink];
    int64_t n = 0;
    int even;
    if (!stone)
        n = count(1, blink - 1);
    else if (!((even = digits(stone)) & 1)) {
        const lldiv_t lr = split(stone, even);
        n = count(lr.quot, blink - 1) + count(lr.rem, blink - 1);
    } else
        n = count(stone * 2024, blink - 1);
    if (stone < CACHESIZE)
        cache[stone][blink] = n;
    return n;
}

static int64_t change(const int blink)
{
    int64_t sum = 0;
    for (int i = 0; i < N; ++i)
        sum += count(input[i], blink);
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 0; i < N && fscanf(f, "%"PRId64, &input[i]) == 1; ++i);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    printf("Part 1: %"PRId64"\n", change(BLINK1));  // example: 55312, input: 199986
    printf("Part 2: %"PRId64"\n", change(BLINK2));  // example: 65601038650482, input: 236804088748754

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
