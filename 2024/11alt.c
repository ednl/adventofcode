/**
 * Advent of Code 2024
 * Day 11: Plutonian Pebbles
 * https://adventofcode.com/2024/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 11alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv
#include <stdint.h>    // uint64_t, uint32_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-11-input.txt"
#define N 8
#define BLINK1 25
#define BLINK2 75
#define HBITS 13
#define HSIZE (1U << HBITS)
#define HMASK (HSIZE - 1)

typedef uint64_t u64;
typedef uint32_t u32;

static const u64 pow10[] = {
    1,   // 10^0: 1 digit
    10,  // 10^1: 2 digits, etc.
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
    10000000000000000,
    100000000000000000,
    1000000000000000000};  // 10^18: 19 digits (INT64_MAX=9223372036854775807: 19 digits)

static const u32 dig10[] = {
                    19, 18,18,18,18,  // index=0 (19/20 digits) unreachable for int64_t
    17,17,17, 16,16,16, 15,15,15,15,
    14,14,14, 13,13,13, 12,12,12,12,
    11,11,11, 10,10,10,  9, 9, 9, 9,
     8, 8, 8,  7, 7, 7,  6, 6, 6, 6,
     5, 5, 5,  4, 4, 4,  3, 3, 3, 3,
     2, 2, 2,  1, 1, 1,  0, 0, 0, 1};  // index=64 for x=0 => 1 digit

static u64 input[N];
static u64 keys[HSIZE];
static u64 vals[HSIZE];

// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
static u64 fnv1a(u64 x)
{
    u64 h = 0xcbf29ce484222325;
    for (; x; x >>= 8) {
        h ^= x & 255;
        h *= 0x100000001b3;
    }
    return h;
}

static int digits(const u64 x)
{
    const u32 approx = dig10[x ? __builtin_clzll(x) : 64];  // clz undefined for x=0
    return approx + (x >= pow10[approx]);
}

static lldiv_t split(const u64 x, const u32 evendigitcount)
{
    return lldiv(x, pow10[evendigitcount >> 1]);
}

static u64 *lookup(const u64 stone)
{
    const u64 hash = fnv1a(stone);
    const u32 step = hash >> (64 - HBITS) | 1;
    for (u32 i = hash;;) {
        i = (i + step) & HMASK;
        if (!keys[i]) {
            keys[i] = stone;
            return &vals[i];
        } else if (keys[i] == stone)
            return &vals[i];
    }
}

static u64 count(const u64 stone, u32 blink)
{
    if (!blink)
        return 1;
    if (!stone)
        return count(1, blink - 1);
    const u32 numdig = digits(stone);
    if (numdig & 1)
        return count(stone * 2024, blink - 1);
    const lldiv_t lr = split(stone, numdig);
    return count(lr.quot, blink - 1) + count(lr.rem, blink - 1);
}

static u64 change(const u32 blink)
{
    u64 sum = 0;
    for (u32 i = 0; i < N; ++i)
        sum += count(input[i], blink);
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    for (u32 i = 0; i < N && fscanf(f, "%"PRIu64, &input[i]) == 1; ++i);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(keys, 0, sizeof keys);
#endif

    // 199986 236804088748754
    // printf("%"PRIu64" %"PRIu64"\n", change(BLINK1), change(BLINK2));
    printf("%"PRIu64"\n", change(BLINK1));

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
