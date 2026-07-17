/**
 * Advent of Code 2021
 * Day 17: Trick Shot
 * https://adventofcode.com/2021/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 17.c -lm
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 17.c -lm
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :    ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 33.5 µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <stdbool.h>
#include <math.h>  // sqrt
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-17-input.txt"
#define FSIZE 64  // needed for my input: 36

static char input[FSIZE];

static int parseint(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s >= '0')
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

static int dist(const int v0, const int t)
{
    return v0 * t - t * (t - 1) / 2;
}

static int maxdist(const int v0)
{
    return v0 > 0 ? dist(v0, v0) : 0;
}

static int height(const int vy0, const int t)
{
    return t > 0 ? dist(vy0, t) : 0;
}

static int width(const int vx0, const int t)
{
    return t > 0 ? (vx0 > t ? dist(vx0, t) : maxdist(vx0)) : 0;
}

// y[t] = vy0*t - t*(t-1)/2 >= TY0
// t^2 - (2*vy0 + 1)*t + 2*TY0 <= 0
// t1,2 = vy0 + 0.5 ± sqrt[(vy0 + 0.5)^2 - 2*TY0]
// => t <= vy0 + 0.5 + sqrt[(vy0 + 0.5)^2 - 2*TY0]

// y[t] = vy0*t - t*(t-1)/2 <= TY1
// t^2 - (2*vy0 + 1)*t + 2*TY1 >= 0
// t1,2 = vy0 + 0.5 ± sqrt[(vy0 + 0.5)^2 - 2*TY1]
// => t >= vy0 + 0.5 + sqrt[(vy0 + 0.5)^2 - 2*TY1]

// maxdist[v] = v*v - v*(v - 1)/2
// => v*v + v - 2*137 = 0
// => v = (-1 ± sqrt[1 + 8*137])/2
// => v = sqrt[2*137 + 0.25] - 0.5 = 16.06 = 17

static double timebound(const int v, const int b)
{
    const double f = v + 0.5;
    const double g = f * f - b * 2;
    return g >= 0 ? sqrt(g) + f : (double)NAN;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read single bytes until EOF or size=FSIZE
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input + 15;  // skip "target area: x="
    const int TX0 = parseint(&c);
    c += 2;  // skip ".."
    const int TX1 = parseint(&c);
    c += 5; // skip ", y=-"
    const int TY0 = -parseint(&c);
    c += 3;  // skip "..-"
    const int TY1 = -parseint(&c);

    int ymax = 0;
    int hits = 0;
    const int minvx0 = ceil(sqrt(2*TX0 + 0.25) - 0.5);  // see above
    for (int vy0 = TY0; vy0 <= -TY0; ++vy0) {
        const int t1 = (int)ceil (timebound(vy0, TY1));
        const int t2 = (int)floor(timebound(vy0, TY0));
        for (int vx0 = minvx0; vx0 <= TX1; ++vx0) {
            bool hit = false;
            for (int t = t1, x, y; t <= t2 && (y = height(vy0, t)) >= TY0 && (x = width(vx0, t)) <= TX1; ++t)
                if (y <= TY1) {
                    int ym = maxdist(vy0);
                    if (ym > ymax)
                        ymax = ym;
                    if (x >= TX0 && !hit) {
                        hit = true;
                        ++hits;
                    }
                }
        }
    }
    printf("%d %d\n", ymax, hits);  // 4753 1546

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
