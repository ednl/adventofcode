/**
 * Advent of Code 2025
 * Day 1: Secret Entrance
 * https://adventofcode.com/2025/day/1
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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 8.99 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-01-input.txt"
#define FSIZE (5<<12) // =20480, needed for my input: 19650 +1

#define START 50  // dial starts at 50
#define SIZE 100  // dial goes from 0 to 99

typedef struct dial {
    int dial;  // unconstrained dial value (any int, not just 0-99)
    int full;  // dial div SIZE (number of full turns, negative for left)
    int zero;  // dial mod SIZE == 0 (dial is at any multiple of 100)
} Dial;

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int zero1 = 0, zero2 = 0;
    Dial cur = {
        .dial = START,
        .full = START / SIZE,
        .zero = START % SIZE == 0,
    };
    for (const char *s = input; *s; ++s) {  // skip newline
        // L = anti-clockwise = negative, R = clockwise = positive
        const int dir = (*s & 3) - 1;  // 'L'=-1, 'R'=1
        int turn = 0;
        while (*++s & 16)  // until newline
            turn = turn * 10 + (*s & 15);
        // Save old dial, set new dial
        const Dial old = cur;
        cur.dial += turn * dir;  // unconstrained dial value
        cur.full = cur.dial / SIZE;  // negative for net left turns
        cur.zero = cur.dial % SIZE == 0;  // is at zero?

        // Part 1
        zero1 += cur.zero;

        // Part 2
        zero2 += (cur.full - old.full) * dir;

        // Special cases (od=old.dial, cd=cur.dial, oz=old.zero, cz=cur.zero)
        //
        //    dial :   -200 -100   0   100  200
        //           ----+----+----+----+----+---
        // div 100 : 2222211111000000000111112222 (neg for neg dial)
        //
        //  R   |  cd<0   cd=0   cd>0
        // -----+---------------------
        // od<0 | +cz-oz  +1-oz  +1-oz
        // od=0 |    -      -      0
        // od>0 |    -      -      0
        //
        //  L   |  cd<0   cd=0   cd>0
        // -----+---------------------
        // od<0 |    0      -      -
        // od=0 |    0      -      -
        // od>0 |  +1-oz  +1-oz +cz-oz

        if (dir * cur.dial < 0)
            // Increasing while negative, or decreasing while positive
            // (=select first column for R, last column for L)
            zero2 += cur.zero - old.zero;
        else if (dir * old.dial < 0)
            // Went to or across zero
            // (=select first row for R, last row for L)
            zero2 += !old.zero;
    }
    printf("%u %u\n", zero1, zero2);  // 1180 6892

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
