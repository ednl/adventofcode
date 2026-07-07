/**
 * Advent of Code 2021
 * Day 7: The Treachery of Whales
 * https://adventofcode.com/2021/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native ../startstoptimer.c 07.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :    ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 1.83 µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-07-input.txt"
#define FSIZE 4000  // needed for my input: 3896
#define N 1000  // 1000 values in input file
#define M 2000  // value range 0..1999 (needed for my input: 1879)

static char input[FSIZE];
static int pos[N];
static char crab[M];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    // const char *const end = input + fread(input, 1, FSIZE, f);
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(crab, 0, sizeof crab);
#endif

    // Parse, index, sum
    int sum = 0;
    const char *c = input;
    for (int i = 0; i < N; ++c, ++i) {
        int x = *c++ & 15;
        while (*c > ',')
            x = x * 10 + (*c++ & 15);
        pos[i] = x;
        crab[x]++;
        sum += x;
    }
    // Quadratic cost => take mean
    // Could go 1 either way because of rounding, but this happens to work for me
    const int mean = sum / N;

    // Linear cost => take median
    int i = 0;
    for (int count = 0; count < 500; ++i)
        count += crab[i];
    const int median = i - 1;  // pos 500 and 501 are identical, so overshot by 1

    // Cost functions
    int fuel1 = 0, fuel2 = 0;
    for (int i = 0; i < N; ++i) {
        fuel1 += abs(pos[i] - median);
        const int x = abs(pos[i] - mean);
        fuel2 += x * (x + 1) >> 1;
    }
    printf("%d %d\n", fuel1, fuel2);  // 349357 96708205

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
