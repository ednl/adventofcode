/**
 * Advent of Code 2021
 * Day 1:
 * https://adventofcode.com/2021/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native ../startstoptimer.c 01.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 4.03 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include "../startstoptimer.h"

#define FNAME "../aocinput/2021-01-input.txt"
#define FSIZE 10000  // needed for my input: 9690
#define N 2000

static char input[FSIZE];
static int depth[N];

static int inc(const int win)
{
    int n = 0;
    for (int i = win; i < N; ++i)
        n += depth[i] > depth[i - win];
    return n;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read char-by-char to max size
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    for (int i = 0; i < N; ++i) {
        register int a = *(const int *)c;
        register int n = a & 15;
        a >>= 8; n = n * 10 + (a & 15);
        a >>= 8; n = n * 10 + (a & 15);
        a >>= 8;
        if (a != '\n') {
            n = n * 10 + (a & 15);
            c += 5;
        } else
            c += 4;
        depth[i] = n;
    }
    printf("%d %d\n", inc(1), inc(3));  // 1466 1491

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
