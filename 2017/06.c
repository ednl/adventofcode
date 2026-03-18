/**
 * Advent of Code 2017
 * Day 6: Memory Reallocation
 * https://adventofcode.com/2017/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 140 µs (brent: 142)
 *     Mac Mini 2020 (M1 3.2 GHz)    : 283 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 429 µs
 */

#include <stdio.h>
#include "06.h"  // resetnib[16], addafter[16][16]
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Use Floyd's algorithm for cycle detection, if not defined: Brent's
#define FLOYD

#define FNAME "../aocinput/2017-06-input.txt"
#define PRE0 100u    // assumes no bank ever holds more than 15 blocks after 100 cycles
#define PRE1 10000u  // assumes lambda (= part1 - part2) >= 10000 cycles
#define LEN  16u     // 16 banks
#define MASK 15u     // mod 16
#define BITS 4u      // div 16

typedef unsigned u32;
typedef unsigned long long u64;
static u32 bank[LEN];

static void settle(void)
{
    u32 kmax = 0;
    u32 vmax = bank[0];
    for (u32 i = 1; i < LEN; ++i)
        if (bank[i] > vmax)
            vmax = bank[(kmax = i)];  // max value and index
    bank[kmax] = 0;              // reset first highest value
    const u32 k = kmax + 1;      // start redistributing here
    const u32 r = vmax & MASK;   // first r=vmax%16 banks +1
    const u32 q = vmax >> BITS;  // every bank +q=vmax/16
    if (q) {
        for (u32 i = k; i < k + r; ++i)
            bank[i & MASK] += q + 1;
        for (u32 i = k + r; i < k + LEN; ++i)
            bank[i & MASK] += q;
    } else
        for (u32 i = k; i < k + r; ++i)
            bank[i & MASK]++;
}

static u64 state(void)
{
    u64 x = 0;
    for (u32 i = LEN; i--; ) {
        if (bank[i] >> BITS)
            return 0;  // too big for 4 bits
        x = x << BITS | bank[i];
    }
    return x;
}

static u64 next(const u64 x)
{
    u32 kmax = 0, vmax = 0;
    u64 tmp = x;
    for (u32 i = 0; tmp; i++, tmp >>= BITS) {
        const u32 nib = tmp & MASK;
        if (nib > vmax) {
            vmax = nib;
            kmax = i;
        }
    }
    return x + addafter[kmax][vmax] & resetnib[kmax];
}

static void detect(const u64 x0)
{
#ifdef FLOYD
    // Floyd's algorithm for cycle detection
    // https://en.wikipedia.org/wiki/Cycle_detection#Floyd%27s_tortoise_and_hare
    u64 slow = next(x0);
    u64 fast = next(next(x0));
    while (slow != fast) {
        slow = next(slow);
        fast = next(next(fast));
    }
    u32 mu = 0;
    for (slow = x0; slow != fast; ++mu) {
        slow = next(slow);
        fast = next(fast);
    }
    u32 lam = 1;
    for (fast = next(slow); slow != fast; ++lam)
        fast = next(fast);
#else
    // Brent's algorithm for cycle detection
    // https://en.wikipedia.org/wiki/Cycle_detection#Brent%27s_algorithm
    u64 slow = x0, fast = next(x0);
    u32 lam = 1;
    for (u32 pow = 1; slow != fast; ++lam) {
        if (pow == lam) {
            slow = fast;
            pow <<= 1;
            lam = 0;
        }
        fast = next(fast);
    }
    slow = fast = x0;
    for (u32 i = 0; i < lam; ++i)
        fast = next(fast);
    u32 mu = 0;
    for (; slow != fast; ++mu) {
        slow = next(slow);
        fast = next(fast);
    }
#endif
    printf("%u %u\n", mu + lam + PRE1, lam);
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    for (u32 n = 0; n < LEN && fscanf(f, "%u", &bank[n]) == 1; ++n);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Get rid of blocks >= 16 in any bank
    for (u32 i = 0; i < PRE0; ++i)
        settle();

    // Long run-in with no repeated patterns yet
    u64 x = state();
    for (u32 i = PRE0; i < PRE1; ++i)
        x = next(x);

    // Detect cycle from current state forward
    detect(x);  // 14029 2765

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
