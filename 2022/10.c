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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.52 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 1.72 µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-10-input.txt"
#define FSIZE 1024  // needed for my input: 965
#define H 6
#define W 40
#define M ((W >> 1) - 1)  // one less than half because first div then inc

static char input[FSIZE];
static char crt[H][W + 1];  // +newline

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

    int x = 1, cycle = 0, signal = 0;
    memset(crt, ' ', sizeof crt);
    for (int i = 0; i < H; ++i)
        crt[i][W] = '\n';
    for (const char *c = input; *c; ) {
        const div_t pos = div(cycle++, 40);
        if (pos.rem == M)  // value 19 means 20th cycle
            signal += cycle * x;
        if (abs(x - pos.rem) < 2)  // beam hits 3px wide sprite?
            crt[pos.quot][pos.rem] = '#';
        switch (*c) {
            case 'a': /* fall-through */
            case 'n': c += 5;                 break;
            case '-': c++; x -= parseint(&c); break;
            default :      x += parseint(&c); break;
        }
    }
    printf("%d\n", signal);  // 15020
    fwrite(crt, sizeof crt, 1, stdout);  // EFUGLPAP

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
