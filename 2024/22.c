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
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 33 ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  ? ms
 *     Raspberry Pi 5 (2.4 GHz)                         :  ? ms
 *     Raspberry Pi 4 (1.8 GHz)                         :  ? ms
 */

#include <stdio.h>
#include <string.h>    // memset
#include <stdint.h>    // uint64_t, uint32_t, uint16_t, uint8_t, UINT32_C
#include <inttypes.h>  // PRIu64, PRIu16
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

#define LEN 2000  // series of consecutive secret numbers generated
#define M24 ((UINT32_C(1) << 24) - 1)  // modulo 16777216 = 24 bit
#define M20 ((UINT32_C(1) << 20) - 1)  // 4x5=20 bit

static char inp[FSIZE];
static bool seq[1 << 20];  // index 32^4 = 4x index -9..+9 in 5 bit
static uint16_t add[1 << 20];

static uint32_t xorshift(uint32_t x)
{
    x ^= x << 6;     // mul by 64, mix
    x &= M24;        // prune
    x ^= x >> 5;     // div by 32, mix
    x ^= x << 11;    // (prune not needed) mul by 2048, mix
    return x & M24;  // prune, return
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(inp, sizeof inp, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    uint64_t sum = 0;
    uint16_t max = 0;
    const char *c = inp;
    for (int n = 0; n < N; ++n) {
        memset(seq, 0, sizeof seq);
        uint32_t key = 0, x = *c++ & 15;
        while (*c != '\n')
            x = x * 10 + (*c++ & 15);
        ++c;
        uint8_t prev = x % 10;
        for (int i = 0; i < 3; ++i) {
            x = xorshift(x);
            const uint8_t val = x % 10;  // value 0..9
            key = key << 5 | (9 + val - prev);  // index 0..18 fits in 5 bit
            prev = val;
        }
        for (int i = 3; i < LEN; ++i) {
            x = xorshift(x);
            const uint8_t val = x % 10;  // value 0..9
            key = (key << 5 | (9 + val - prev)) & M20;  // index 0..18 fits in 5 bit, keep at 4x5=20 bit
            if (!seq[key]) {
                seq[key] = true;
                add[key] += val;
                if (add[key] > max)
                    max = add[key];
            }
            prev = val;
        }
        sum += x;  // part 1
    }
    printf("Part 1: %"PRIu64"\n", sum);  // example1: 37327623, example2: 37990510, input: 19150344884
    printf("Part 2: %"PRIu16"\n", max);  // example1: 24, example2: 23, input: 2121

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
