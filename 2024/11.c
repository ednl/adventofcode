/**
 * Advent of Code 2024
 * Day 11: Plutonian Pebbles
 * https://adventofcode.com/2024/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 11.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 598 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-11-input.txt"
#define N 8
#define BLINK1 25
#define BLINK2 75
#define CACHESIZE 1024

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
static int64_t cache[BLINK2][CACHESIZE];

static int digits(const int64_t x)
{
    const int approx = dig10[x ? __builtin_clzll(x) : 64];  // clz undefined for x=0
    return approx + (x >= pow10[approx]);
}

static lldiv_t split(const int64_t x, const int even)
{
    return lldiv(x, pow10[even >> 1]);
}

static int64_t count(const int64_t stone, int blink)
{
    if (blink-- == 0)
        return 1;
    if (stone < CACHESIZE && cache[blink][stone])
        return cache[blink][stone];
    int64_t n;
    const int numdig = digits(stone);
    if ((numdig & 1) == 0) {  // even number of digits
        const lldiv_t lr = split(stone, numdig);
        n = count(lr.quot, blink) + count(lr.rem, blink);
    } else if (stone == 0)
        n = count(1, blink);
    else  // odd number of digits (except 0)
        n = count(stone * 2024, blink);
    if (stone < CACHESIZE)
        cache[blink][stone] = n;
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
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    for (int i = 0; i < N && fscanf(f, "%"PRId64, &input[i]) == 1; ++i);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(cache, 0, sizeof cache);
#endif

    // 199986 236804088748754
    printf("%"PRIu64" %"PRIu64"\n", change(BLINK1), change(BLINK2));

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
