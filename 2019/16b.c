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
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 65.3 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? ms
 *     Raspberry Pi 5 (2.4 GHz)      : ? ms
 */

#include <stdio.h>
#include <stdint.h>
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
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, N, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < TIMERLOOPS; ++TIMERLOOP) {
#endif

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
fprintf(stderr, "Time: %.0f us\n", stoptimer_us() / TIMERLOOPS);
#endif
}
