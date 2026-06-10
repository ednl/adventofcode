/**
 * Advent of Code 2020
 * Day 10: Adapter Array
 * https://adventofcode.com/2020/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.48 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 0.69 µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-10-input.txt"
#define FSIZE 400  // needed for my input: 352
#define JOLTS 200  // needed for my imput: 180+1+3 = 184
#define MAXGAP  3

static char input[FSIZE];
static bool adapter[JOLTS];
static int64_t arrangements[JOLTS];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(arrangements, 0, sizeof arrangements);
#endif

    const uint64_t *const end64 = (const uint64_t *)input + ((end - input) >> 3) + 1;
    for (uint64_t *digit = (uint64_t *)input; digit != end64; ++digit)
        *digit &= UINT64_C(0x0f0f0f0f0f0f0f0f);
    int gaps = 1, maxjolts = 0;
    for (const char *c = input; c != end; ++gaps, ++c) {
        int jolts = *c++;
        while (*c != '\n')
            jolts = jolts * 10 + *c++;
        adapter[jolts] = true;
        if (jolts > maxjolts)
            maxjolts = jolts;
    }
    adapter[0] = true;  // charging outlet at 0 jolts
    adapter[maxjolts += MAXGAP] = true;  // device being charged at the other end

    // Part 1
    // Assume no gaps of length 2:
    //   x + 3y = M (maxjolts)
    //   x +  y = N (gaps)
    //       2y = (M-N)
    //        y = (M-N)/2
    //   x      = N - (M-N)/2
    const int gap3 = (maxjolts - gaps) >> 1;
    printf("%d", (gaps - gap3) * gap3);  // part 1: 2574

    // Part 2
    arrangements[0] = 1;  // charging outlet is fixed
    for (int j = 1; j < MAXGAP; ++j)
        for (int i = 0; i < j; ++i)
            arrangements[j] += arrangements[i] * adapter[i];  // only add if adapter[i] exists (=true)
    for (int j = MAXGAP; j <= maxjolts; ++j)
        for (int i = j - MAXGAP; i < j; ++i)
            arrangements[j] += arrangements[i] * adapter[i];
    printf(" %"PRId64"\n", arrangements[maxjolts]);  // part 2: 2644613988352

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
