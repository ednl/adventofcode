/**
 * Advent of Code 2025
 * Day 12: Christmas Tree Farm
 * https://adventofcode.com/2025/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.23 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 1.76 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 4.70 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-12-input.txt"
#define SHAPES 6    // shapes in input file
#define TREES 1000  // trees in input file
#define FSIZE (SHAPES * 16 + TREES * 25)  // 16 char per shape, 25 char per tree

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME"\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    unsigned sure = 0;
    const char *s = input + SHAPES * 16;
    for (int i = 0; i < TREES; s += 25, ++i) {
        const unsigned blocks =
              (s[0] * 10 + s[1] - 11 * '0')
            * (s[3] * 10 + s[4] - 11 * '0');
        const unsigned shapes =
              (s[7] + s[10] + s[13] + s[16] + s[19] + s[22]) * 90
            + (s[8] + s[11] + s[14] + s[17] + s[20] + s[23]) * 9
            - 11 * '0' * 6 * 9;
        sure += blocks >= shapes;
    }
    printf("%u\n", sure);  // 521

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
