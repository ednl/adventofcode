/**
 * Advent of Code 2022
 * Day 10: Cathode-Ray Tube
 * https://adventofcode.com/2022/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Test output with timer enabled:
 *     ./a.out | tail -n7
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.26 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-10-input.txt"
#define FSIZE 1024  // needed for my input: 965
#define WIDTH   40
#define HEIGHT   6
#define MID (WIDTH >> 1)

static char input[FSIZE];
static char tube[WIDTH * HEIGHT + 1];  // +1 because cycle is 1-based

static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s != '\n')  // until newline
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip newline
    return x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int x = 1, cycle = 0, beam = 0, trip = MID, signal = 0;
    memset(tube, ' ', sizeof tube);
    for (const char *c = input; *c; ) {
        cycle++;
        if (cycle == trip) {  // midpoint?
            signal += cycle * x;  // part 1
            trip += WIDTH;
        }
        beam++;
        if (beam - x >= 0 && beam - x < 3)  // beam hits 3px-wide sprite?
            tube[cycle] = '#';
        if (beam == WIDTH)
            beam = 0;
        switch (*c) {
            case 'a': /* fall-through */
            case 'n': c += 5;                 break;
            case '-': c++; x -= parseint(&c); break;
            default :      x += parseint(&c); break;
        }
    }
    printf("%d\n", signal);  // 15020
    tube[WIDTH] = tube[WIDTH * 2] = tube[WIDTH * 3] = tube[WIDTH * 4] = tube[WIDTH * 5] = tube[WIDTH * 6] = '\n';
    fwrite(tube + 1, sizeof tube - 1, 1, stdout);  // EFUGLPAP

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
