/**
 * Advent of Code 2021
 * Day 6: Lanternfish
 * https://adventofcode.com/2021/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native ../startstoptimer.c 06.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 0.78 µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include <string.h>    // memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-06-input.txt"
#define FISH 300  // 300 numbers in input file
#define FSIZE (FISH * 2)  // CSV +newline

#define CYCLE   7  // spawn every 7 days
#define DELAY   2  // add 2 days to the first spawn cycle
#define LIFE (CYCLE + DELAY)  // the whole circle of life
#define DAYS1  80  // part 1
#define DAYS2 256  // part 2

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Histogram of fish population count per age mod 9
    uint64_t age[LIFE] = {0};

    // Parse
    for (const char *c = input; c != end; c += 8) {
        uint64_t data;
        memcpy(&data, c, 8);  // safe way instead of UB from pointer alignment
        age[data       & 0xf]++;
        age[data >> 16 & 0xf]++;
        age[data >> 32 & 0xf]++;
        age[data >> 48 & 0xf]++;
    }

    int day = 0, i = 0, j = CYCLE;
    {
        // Faster if mod isn't very fast
        // for (; day < DAYS1; ++day, ++i == LIFE && (i = 0), ++j == LIFE && (j = 0))
        //     age[j] += age[i];  // start new spawn cycle
        // Faster if mod is very fast (like on Apple M-series)
        for (; day < DAYS1; ++day)
            // Leaving current bin age[i] untouched = each fishie produces one kiddo
            age[j++ % LIFE] += age[i++ % LIFE];  // start new spawn cycle
        uint64_t sum = 0;  // population
        for (int k = 0; k < LIFE; ++k)
            sum += age[k];
        printf("%"PRIu64" ", sum);  // 374927
    }
    {
        for (; day < DAYS2; ++day)
            age[j++ % LIFE] += age[i++ % LIFE];
        uint64_t sum = 0;
        for (int k = 0; k < LIFE; ++k)
            sum += age[k];
        printf("%"PRIu64"\n", sum);  // 1687617803407
    }

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
