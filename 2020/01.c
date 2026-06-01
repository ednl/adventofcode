/**
 * Advent of Code 2020
 * Day 1: Report Repair
 * https://adventofcode.com/2020/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.83 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 2.50 µs
 */

#include <stdio.h>
#include <stdint.h>  // uint64_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-01-input.txt"
#define FSIZE 992  // must be divisible by 8. Needed for my input: 990=>992
#define N     200  // numbers in input file
#define SUM  2020  // from puzzle description

static char input[FSIZE];
static bool seen[SUM];
static int data[N];

static void parse(void)
{
    // Convert ASCII digits to values, leave newlines intact
    uint64_t *inp64 = (uint64_t *)input;
    for (int i = 0; i < (FSIZE >> 3); ++i)         // 8 bytes per u64
        inp64[i] &= UINT64_C(0x0f0f0f0f0f0f0f0f);  // every byte &= 15
    // Convert groups of 3 or 4 digits to numbers, index into 'seen'
    const char *c = input;
    for (int i = 0; i < N; ++i)
        if (*(c + 4) == '\n') {                    // 4-digit number
            seen[((*c * 10 + *(c + 1)) * 10 + *(c + 2)) * 10 + *(c + 3)] = true;
            c += 5;                                // also skip newline
        } else {                                   // 3-digit number
            seen[(*c * 10 + *(c + 1)) * 10 + *(c + 2)] = true;
            c += 4;                                // also skip newline
        }
    // Assemble sorted list
    for (int i = 0, j = 0; i < SUM; ++i)
        if (seen[i])
            data[j++] = i;
}

static int twosum(void)
{
    for (int i = 0; i < N; ++i) {
        const int res = SUM - data[i];  // residu
        if (seen[res])
            return data[i] * res;
    }
    return 0;
}

static int threesum(void)
{
    for (int i = 0; i < N - 2; ++i) {
        const int res1 = SUM - data[i];
        for (int j = i + 1; j < N - 1; ++j) {
            const int res2 = res1 - data[j];
            if (res2 < data[j + 1])  // impossible from here?
                break;  // next i
            if (seen[res2])
                return data[i] * data[j] * res2;
        }
    }
    return 0;
}

int main(void)
{
    // Read input file
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    parse();
    printf("%d %d\n", twosum(), threesum());  // 802011 248607374

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
