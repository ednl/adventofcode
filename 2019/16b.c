/**
 * Advent of Code 2019
 * Day 16: Flawed Frequency Transmission, part 2
 * https://adventofcode.com/2019/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 16b.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16b.c
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  69 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? ms
 *     Raspberry Pi 5 (2.4 GHz)      :   ? ms
 */

#include <stdio.h>
#ifdef TIMER
#include "../startstoptimer.h"
#define TIMERLOOPS 10
#endif

#define FNAME "../aocinput/2019-16-input.txt"
#define N 650        // digits on one line in input file
#define M 10000      // size multiplier for part 2
#define PHASES 100   // repeat process 100x
#define SIZE 600000  // 6.5M - 5M + margin

typedef int VecType;  // char is slower

static char input[N];
static VecType vec[SIZE];

static const VecType mod10[] = {
    0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8
};

int main(void)
{
#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < TIMERLOOPS; ++TIMERLOOP) {
#endif

    FILE *f = fopen(FNAME, "rb");
    fread(input, N, 1, f);
    fclose(f);

    // atoi
    for (int i = 0; i < N; ++i)
        input[i] &= 15;

    // Get offset from input (first 7 digits)
    int offset = 0;
    for (int i = 0; i < 7; ++i)
        offset = offset * 10 + input[i];  // 5977567

    // Storage space from offset onwards to 10000 * length of input
    const int size = N * M - offset;  // 6500000 - 5977567 = 522433 (must be <= SIZE)

    // Initialise vector with input, from offset
    // Can't use memcpy because char->int
    {
        int i = 0;
        for (int j = offset % N; j < N; ++i, ++j)  // 5977567 % 650 = 167
            vec[i] = input[j];
        while (i < size)
            for (int j = 0; j < N; ++i, ++j)
                vec[i] = input[j];
    }

    // 100 Repeated phases
    for (int phase = 0; phase < PHASES; ++phase)
        // Reverse to avoid O(N!)
        for (int i = size - 2; i >= 0; --i)
            // Each element is the sum for the next one
            vec[i] = mod10[vec[i] + vec[i + 1]];

    for (int i = 0; i < 8; ++i)
        putchar('0' + vec[i]);  // 53201602
    putchar('\n');

#ifdef TIMER
}
printf("Time: %.0f ms\n", stoptimer_ms() / TIMERLOOPS);
#endif
}
