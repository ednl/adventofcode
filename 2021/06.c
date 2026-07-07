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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.405 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
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
#define FISH  300  // 300 numbers in input file
#define FSIZE (FISH * 2)  // CSV +newline
#define LIFE  9
#define DAYS1 80   // part 1
#define DAYS2 256  // part 2
#define LOOP1 (DAYS1 / LIFE)  // 80 div 9 = 8
#define LOOP2 ((DAYS2 - DAYS1) / LIFE)  // (256 - 80) div 9 = 19

static char input[FSIZE];

static uint64_t sum(const uint64_t *const arr)
{
    uint64_t x = 0;
    for (int i = 0; i < LIFE; ++i)
        x += arr[i];
    return x;
}

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
    uint64_t fish[LIFE] = {0};

    // Parse
    for (const char *c = input; c != end; c += 8) {
        uint64_t chunk;
        memcpy(&chunk, c, 8);  // safe way instead of UB from pointer alignment
        fish[chunk       & 0xf]++;
        fish[chunk >> 16 & 0xf]++;
        fish[chunk >> 32 & 0xf]++;
        fish[chunk >> 48 & 0xf]++;
    }

    // Part 1
    for (int k = 0; k < LOOP1; ++k) {  // 80 div 9 = 8
        fish[7] += fish[0];  // spawn every 7 days
        fish[8] += fish[1];
        fish[0] += fish[2];  // add 2 days to the first spawn cycle
        fish[1] += fish[3];
        fish[2] += fish[4];
        fish[3] += fish[5];
        fish[4] += fish[6];
        fish[5] += fish[7];
        fish[6] += fish[8];
    }
    fish[7] += fish[0];
    fish[8] += fish[1];
    fish[0] += fish[2];
    fish[1] += fish[3];
    fish[2] += fish[4];
    fish[3] += fish[5];
    fish[4] += fish[6];
    fish[5] += fish[7];  // 80 mod 9 = 8
    printf("%"PRIu64" ", sum(fish));  // 374927

    // Part 2
    for (int k = 0; k < LOOP2; ++k) {  // (256 - 80) div 9 = 19
        fish[6] += fish[8];  // continue where part 1 stopped
        fish[7] += fish[0];
        fish[8] += fish[1];
        fish[0] += fish[2];
        fish[1] += fish[3];
        fish[2] += fish[4];
        fish[3] += fish[5];
        fish[4] += fish[6];
        fish[5] += fish[7];
    }
    fish[6] += fish[8];
    fish[7] += fish[0];
    fish[8] += fish[1];
    fish[0] += fish[2];
    fish[1] += fish[3];  // (256 - 80) mod 9 = 5
    printf("%"PRIu64"\n", sum(fish));  // 1687617803407

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
