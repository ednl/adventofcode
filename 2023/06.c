/**
 * Advent of Code 2023
 * Day 6: Wait For It
 * https://adventofcode.com/2023/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c -lm
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c -lm
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  59 ns
 *     Mac Mini 2020 (M1 3.2 GHz)    :  99 ns
 *     Raspberry Pi 5 (2.4 GHz)      : 190 ns
 *     iMac 2013 (i5 4570 3.2 GHz)   : 253 ns
 */

#include <stdio.h>
#include <math.h>      // sqrt
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-06-input.txt"
#define FSIZE 74  // 2 lines of 36 chars +newline
#define RACES 4

static char input[FSIZE];
static unsigned t[RACES];
static unsigned d[RACES];

static unsigned parseint(const char **s)
{
    unsigned x = *(*s)++ & 15;
    while (**s & 16)  // followed by space or newline
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

// Given: t = race time, d = race distance (t>0, d>0)
// Find: x = button time => remaining time = t - x
//   boat speed = v = x => travel = v . time = x . (t - x)
// Condition: travel > d
// Find roots: travel = d
// <=> x(t - x) = d
//     x^2 - tx + d = 0
//     x0,1 = (t +/- sqrt(t^2 - 4d)) / 2
//     x1 - x0 = sqrt(t^2 - 4d)
//
// From /u/musifter at https://redd.it/1w8oaya :
//
// But we need the number of integers in that size of range,
// but the range center depends on if the time is even or odd:
// |   |   |   |   |   |   |  Integers (t is even) (5)
//   |   |   |   |   |   |    Integers (t is odd)  (6)
// |-----------|-----------|
// x0         t/2          x1
// And additionally, if we truncate the difference to an integer,
// it depends if that ends up even or odd as to whether we're including
// or excluding one. And so the factor to adjust ends up being an XNOR.
static uint64_t ways2win(const uint64_t t, const uint64_t d)
{
    const uint64_t w = sqrt(t * t - 4 * (d + 1));
    return w + ((t + w + 1) & 1);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file as one block
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    uint64_t t2 = 0, d2 = 0;  // part 2
    const char *c = input + 13;  // skip to t[0] (eqv. of "Distance:" +3 space, +1 = length diff of t[0] and d[0])
    for (int i = 0; i < RACES; c += 5, ++i)  // skip 5 spaces between times
        t2 = t2 * 100 + (t[i] = parseint(&c));  // assume 10 <= t[1..] < 100
    c += 8;  // skip "\nDistance:" -5 +3
    for (int i = 0; i < RACES; c += 3, ++i)  // skip 3 spaces between distances
        d2 = d2 * 10000 + (d[i] = parseint(&c));  // assume 1000 <= d[1..] < 10000

    unsigned part1 = 1;
    for (int i = 0; i < RACES; ++i)
        part1 *= ways2win(t[i], d[i]);
    printf("%u %"PRIu64"\n", part1, ways2win(t2, d2));  // 2449062 33149631

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
