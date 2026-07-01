/**
 * Advent of Code 2021
 * Day 2: Dive!
 * https://adventofcode.com/2021/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native ../startstoptimer.c 02.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.61 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-02-input.txt"
#define FSIZE 8192  // needed for my input: 7811
#define N 1000

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read char-by-char to EOF or max size
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    unsigned fwd = 0, dp1 = 0, dp2 = 0, aim = 0;
    const char *c = input;
    for (int i = 0; i < N; ++i)
        switch (*c) {
            case 'd': {
                const unsigned x = *(c + 5) & 15;
                dp1 += x;
                aim += x;
                c += 7;
                break;
            }
            case 'f': {
                const unsigned x = *(c + 8) & 15;
                fwd += x;
                dp2 += x * aim;
                c += 10;
                break;
            }
            case 'u': {
                const unsigned x = *(c + 3) & 15;
                dp1 -= x;
                aim -= x;
                c += 5;
                break;
            }
        }
    printf("%d %d\n", fwd * dp1, fwd * dp2);  // 2150351 1842742223

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
