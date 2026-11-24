/**
 * Advent of Code 2023
 * Day 24: Never Tell Me The Odds
 * https://adventofcode.com/2023/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 24.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 24.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 38.3 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t, INT64_C
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-24-input.txt"
#define FSIZE ((1<<14)|(1<<12))  // needed for my input: 19684
#define N 300  // lines in input file
#define MIN INT64_C(200000000000000)
#define MAX INT64_C(400000000000000)

typedef int64_t i64;

static char input[FSIZE];
static i64 x[N], y[N], z[N];
static int dx[N], dy[N], dz[N];
static double a[N], b[N];

static i64 pos(const char **s)
{
    i64 x = 0;
    while (**s & 16)  // until ','=44 or ' '=32
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

static int vel(const char **s)
{
    const int neg = **s == '-' ? ((*s)++, 1) : 0;
    int x = 0;
    while (**s & 16)  // until ','=44 or '\n'=10
        x = x * 10 + (*(*s)++ & 15);
    return neg ? -x : x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, sizeof input, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    for (int i = 0; *c; ++i, ++c) {
        x[i] = pos(&c); c += 2;
        y[i] = pos(&c); c += 2;
        z[i] = pos(&c); c += 3;
        dx[i] = vel(&c); c += 2;
        dy[i] = vel(&c); c += 2;
        dz[i] = vel(&c);
        a[i] = (double)dy[i] / dx[i];
        b[i] = y[i] - a[i] * x[i];
    }
    int part1 = 0;
    i64 part2 = 0;
    for (int i = 1; i < N; ++i)
        for (int j = 0; j < i; ++j) {
            const double xx = (b[j] - b[i]) / (a[i] - a[j]);
            const double yy = a[i] * xx + b[i];
            const double ti = (xx - x[i]) / dx[i];
            const double tj = (xx - x[j]) / dx[j];
            part1 += xx >= MIN && xx <= MAX && yy >= MIN && yy <= MAX && ti > 0 && tj > 0;
        }
    printf("%u %"PRIu64"\n", part1, part2);  // 20434 ?

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
