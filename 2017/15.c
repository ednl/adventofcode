/**
 * Advent of Code 2017
 * Day 15: Dueling Generators
 * https://adventofcode.com/2017/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=gnu17 -O3 -march=native -mtune=native ../startstoptimer.c 11.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 153 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? ms
 *     Raspberry Pi 5 (2.4 GHz)      : 316 ms
 */

#include <stdio.h>
#include <stdint.h>
#include "../startstoptimer.h"

static uint64_t m31mod(uint64_t g)
{
    while (g >= 0x7fffffffU)
        g = (g & 0x7fffffffU) + (g >> 31);
    return g;
}

static int duel1(uint64_t a, uint64_t b)
{
    int k = 0;
    for (int n = 40000000; n--; ) {
        a = m31mod(a * 16807U);
        b = m31mod(b * 48271U);
        k += !((a ^ b) & 0xffff);
    }
    return k;
}

static int duel2(uint64_t a, uint64_t b)
{
    int k = 0;
    for (int n = 5000000; n--; ) {
        do a = m31mod(a * 16807U); while (a & 0x3U);
        do b = m31mod(b * 48271U); while (b & 0x7U);
        k += !((a ^ b) & 0xffff);
    }
    return k;
}

int main(void)
{
    starttimer();
    printf("%d\n", duel1(783, 325));  // 650
    printf("%d\n", duel2(783, 325));  // 336
    printf("Time: %.0f ms\n", stoptimer_ms());
}
