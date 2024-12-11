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
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <search.h>    // maybe use posix hash table? maybe not though.

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

static int digits(const int64_t x)
{
    const int approx = dig10[x ? __builtin_clzll(x) : 64];  // clz undefined for x=0
    return approx + (x >= pow10[approx]);
}

static lldiv_t split(const int64_t x, const int even)
{
    return lldiv(x, pow10[even >> 1]);
}

int main(void)
{
    for (int i = 0; i < 19; ++i)
        printf("%2d %20"PRId64": %2d %2d %2d\n", i, pow10[i], digits(pow10[i] - 1), digits(pow10[i]), digits(pow10[i] + 1));
    printf("%2d %20"PRId64": %2d %2d\n", 19, INT64_MAX, digits(INT64_MAX - 1), digits(INT64_MAX));

    // const lldiv_t lr = split(UINT64_MAX, 20);
    const lldiv_t lr = split(103050709020406080LL, 18);
    printf("%"PRId64" %"PRId64"\n", lr.quot, lr.rem);
}
