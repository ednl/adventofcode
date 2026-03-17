/**
 * Advent of Code 2017
 * Day 17: Spinlock
 * https://adventofcode.com/2017/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=gnu17 -O3 -march=native -mtune=native ../startstoptimer.c 17.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 151 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? ms
 *     Raspberry Pi 5 (2.4 GHz)      :   ? ms
 */

#include <stdio.h>
#include "../startstoptimer.h"

#define STEP 303  // puzzle input
#define BUF 2018  // "one after 2017"
#define M50 (50 * 1000 * 1000)  // 50 million

static int buf[BUF];

int main(void)
{
    starttimer();
    // Part 1
    int pos = 0;
    for (int len = 1; len < BUF; ++len) {
        pos = (pos + STEP) % len + 1;
        for (int i = len; i > pos; --i)
            buf[i] = buf[i - 1];
        buf[pos] = len;
    }
    printf("%d\n", buf[++pos % BUF]);  // 1971
    // Part 2
    int last = 0;
    pos = 0;
    for (int len = 1; len <= M50; ++len) {
        pos = (pos + STEP) % len + 1;
        if (pos == 1)
            last = len;
    }
    printf("%d\n", last);  // 17202899
    printf("Time: %.0f ms\n", stoptimer_ms());
}
