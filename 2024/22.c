/**
 * Advent of Code 2024
 * Day 22: Monkey Market
 * https://adventofcode.com/2024/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 22.c
 *    gcc   -std=gnu17 -Wall -Wextra 22.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 22.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 22.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 16 ms
 *     Raspberry Pi 5 (2.4 GHz)                         : 28 ms
 *     Raspberry Pi 4 (1.8 GHz)                         : 58 ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  ? ms
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t, uint32_t, UINT32_C
#include <inttypes.h>  // PRIu64, PRIu32
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-22-example1.txt"
    #define FSIZE 14
    #define N 4
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2024-22-example2.txt"
    #define FSIZE 11
    #define N 4
#else
    #define FNAME "../aocinput/2024-22-input.txt"
    #define FSIZE 18965
    #define N 2268
#endif

#define LEN 2000  // secret numbers generated by buyers
#define PRUNE ((UINT32_C(1) << 24) - 1)  // mod 16777216 = 24 LSBits

// Cache indexing
#define BASE  UINT32_C(20)    // faster than base 19 because it has factors of 2
#define BASE2 (BASE * BASE)   // base^2
#define BASE3 (BASE2 * BASE)  // base^3
#define CACHE (BASE3 * BASE)  // base^4 = cache size

// Bitfield
#define BFDIV 6                             // bitfield in 64-bit chunks: 6-bit bit index
#define BFLEN (CACHE >> BFDIV)              // bitfield length in chunks
#define BFMOD ((UINT32_C(1) << BFDIV) - 1)  // bitfield bit index mask

static char input[FSIZE];
static uint32_t aggr[CACHE];

// https://en.wikipedia.org/wiki/Xorshift
static uint32_t xorshift(uint32_t x)
{
    x ^= x << 6;       // mul by 64, mix
    x &= PRUNE;        // prune
    x ^= x >> 5;       // div by 32, mix
    x ^= x << 11;      // (prune not needed) mul by 2048, mix
    return x & PRUNE;  // prune, return
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file in one go
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    const char *c = input;
    uint64_t sum = 0;
    for (int n = 0; n < N; ++n) {
        // Parse unsigned integer followed by newline
        uint32_t x = *c++ & 15;
        while (*c != '\n')
            x = x * 10 + (*c++ & 15);
        ++c;  // skip newline
        uint32_t prev = x % 10, a = 0, b = 0, c = 0, d = 0;
        // First 3 differences can't form a sequence yet
        for (int i = 0; i < 3; ++i) {
            x = xorshift(x);              // next secret
            const uint32_t val = x % 10;  // last digit
            b = c;                        // rotate cache index ('a' not used yet)
            c = d;
            d = 9 + val - prev;
            prev = val;
        }
        // Check sequences from difference 4 onward
        uint64_t seen[BFLEN] = {0};  // bitfield (init to zero still takes a lot of time)
        for (int i = 3; i < LEN; ++i) {
            x = xorshift(x);
            const uint32_t val = x % 10;
            a = b;
            b = c;
            c = d;
            d = 9 + val - prev;
            const uint32_t key = a * BASE3 + b * BASE2 + c * BASE + d;
            const uint32_t bix = key >> BFDIV;  // bitfield unit index
            const uint64_t bit = UINT64_C(1) << (key & BFMOD);  // bit inside bitfield unit
            if (!(seen[bix] & bit)) {  // only count first occurence (val may be zero)
                seen[bix] |= bit;  // set bit in bitfield
                aggr[key] += val;  // sum (aggregate)
            }
            prev = val;
        }
        sum += x;  // part 1: sum of 2000th secrets
    }
    uint32_t max = 0;
    for (uint32_t i = 0; i < CACHE; ++i)
        if (aggr[i] > max)
            max = aggr[i];
    printf("Part 1: %"PRIu64"\n", sum);  // ex1: 37327623, ex2: 37990510, inp: 19150344884
    printf("Part 2: %"PRIu32"\n", max);  // ex1: 24      , ex2: 23      , inp: 2121

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
