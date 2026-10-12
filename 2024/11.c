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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 591 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-11-input.txt"
#define N 8
#define BLINK1 25
#define BLINK2 75
#define CACHESIZE 1024

static const uint64_t pow10[] = {
    UINT64_C(1),   // 10^0: 1 digit
    UINT64_C(10),  // 10^1: 2 digits, etc.
    UINT64_C(100),
    UINT64_C(1000),
    UINT64_C(10000),
    UINT64_C(100000),
    UINT64_C(1000000),
    UINT64_C(10000000),
    UINT64_C(100000000),
    UINT64_C(1000000000),
    UINT64_C(10000000000),
    UINT64_C(100000000000),
    UINT64_C(1000000000000),
    UINT64_C(10000000000000),
    UINT64_C(100000000000000),
    UINT64_C(1000000000000000),
    UINT64_C(10000000000000000),
    UINT64_C(100000000000000000),
    UINT64_C(1000000000000000000)};  // 10^18: 19 digits (INT64_MAX=9223372036854775807: 19 digits)

static const unsigned dig10[] = {
                    19, 18,18,18,18,  // index=0 (19/20 digits) unreachable for int64_t
    17,17,17, 16,16,16, 15,15,15,15,
    14,14,14, 13,13,13, 12,12,12,12,
    11,11,11, 10,10,10,  9, 9, 9, 9,
     8, 8, 8,  7, 7, 7,  6, 6, 6, 6,
     5, 5, 5,  4, 4, 4,  3, 3, 3, 3,
     2, 2, 2,  1, 1, 1,  0, 0, 0, 1};  // index=64 for x=0 => 1 digit

static uint64_t input[N];
static uint64_t cache[BLINK2][CACHESIZE];

static unsigned digits(const uint64_t x)
{
    const unsigned approx = dig10[x ? __builtin_clzll(x) : 64];  // clz undefined for x=0
    return approx + (x >= pow10[approx]);
}

static lldiv_t split(const uint64_t x, const unsigned evendigitcount)
{
    return lldiv(x, pow10[evendigitcount >> 1]);
}

static uint64_t count(const uint64_t stone, unsigned blink)
{
    if (blink-- == 0)
        return 1;
    if (stone < CACHESIZE && cache[blink][stone])
        return cache[blink][stone];
    uint64_t n;
    const unsigned numdig = digits(stone);
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

static uint64_t change(const unsigned blink)
{
    uint64_t sum = 0;
    for (unsigned i = 0; i < N; ++i)
        sum += count(input[i], blink);
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    for (unsigned i = 0; i < N && fscanf(f, "%"PRIu64, &input[i]) == 1; ++i);
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
