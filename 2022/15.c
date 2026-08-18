/**
 * Advent of Code 2022
 * Day 15: Beacon Exclusion Zone
 * https://adventofcode.com/2022/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 15.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 15.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.73 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 1.19 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 2.13 µs
 */

#include <stdio.h>
#include <stdlib.h>    // abs
#include <stdint.h>    // int64_t, INT64_C
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-15-input.txt"
#define FSIZE 4096          // needed for my input: 2778
#define N 48                // sensors (needed for my input: 38)
#define M 2                 // unique beacons on line y=Y (needed for my input: 1)
#define Y 2000000           // part 1: horizontal ine of interest
#define F INT64_C(4000000)  // part 2: x-multiplier

typedef struct vec {
    int x, y;
} Vec;
typedef struct square {
    Vec l, r;
} Square;

static char input[FSIZE];
static int beacon[M];     // x-coordinates of beacons on line y=Y
static Square sensor[N];  // opposite corners of sensor ranges

static int parseint(const char **s)
{
    const int positive = **s != '-' ? 1 : ((*s)++, 0);
    int x = *(*s)++ & 15;
    while (**s >= '0' && **s <= '9')
        x = x * 10 + (*(*s)++ & 15);
    return positive ? x : -x;
}

// Rotate left by 45 deg + dilation by sqrt(2)
// [[1,1],[-1,1]].(x,y) = (x+y,-x+y) = (x+y,y-x)
static Vec rotleft(const Vec v)
{
    return (Vec){v.x + v.y, v.y - v.x};
}

// Rotate right by 45 deg + contraction by sqrt(2)
// [[1,-1],[1,1]].(x+y,y-x)/2 = (x+y+x-y,x+y-x+y)/2 = (2x,2y)/2 = (x,y)
static Vec rotright(const Vec v)
{
    return (Vec){(v.x - v.y) >> 1, (v.x + v.y) >> 1};
}

// Find distress beacon stuck between 4 sensors:
//   2 sensors 1 apart in rotated x-direction, and
//   2 sensors 1 apart in rotated y-direction
static Vec distress(const int n)
{
    Vec pos = {0};
    for (int i = 1; i < n; ++i)
        for (int j = 0; j < i; ++j) {
            if (sensor[i].l.x == sensor[j].r.x) {
                pos.x = sensor[i].l.x;
                if (pos.y) return pos;
            } else if (sensor[i].r.x == sensor[j].l.x) {
                pos.x = sensor[i].r.x;
                if (pos.y) return pos;
            }
            if (sensor[i].l.y == sensor[j].r.y) {
                pos.y = sensor[i].l.y;
                if (pos.x) return pos;
            } else if (sensor[i].r.y == sensor[j].l.y) {
                pos.y = sensor[i].r.y;
                if (pos.x) return pos;
            }
        }
    return pos;  // unreachable for good input
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

    int n = 0;  // number of sensors
    int m = 0;  // number of beacons on line y=Y
    int minx = 0, maxx = 0;  // sensor range extremes on line y=Y
    for (const char *c = input; *c; c++) {
        c += 12; const int sx = parseint(&c);  // sensor x
        c +=  4; const int sy = parseint(&c);  // sensor y
        c += 25; const int bx = parseint(&c);  // beacon x
        c +=  4; const int by = parseint(&c);  // beacon y

        if (by == Y) {  // beacon on horizontal line of interest?
            for (int i = 0; i < m; ++i)
                if (beacon[i] == bx)
                    goto dupe;
            beacon[m++] = bx;  // save unique x-coordinate
        }
    dupe:;

        const int r = abs(sx - bx) + abs(sy - by);  // sensor range as Manhattan distance
        const int x = r - abs(sy - Y);  // left over from range after moving to y=Y
        if (x >= 0) {  // does this sensor reach line y=Y?
            // Assume all sensor ranges overlap on line y=Y
            if (sx - x < minx) minx = sx - x;
            if (sx + x > maxx) maxx = sx + x;
        }

        //        Sensor                         Sensor'
        //                          |       %
        //                          |     %   .   .   .
        //   |                      |       %   #   #   r
        // - + - - % % - -   rotl   |     %   .   #   #
        //   |   % % .        =>    + - - - % - # - S - #
        //   | % % . # .            |     %   .   #   #
        //   % % . # # # .          |       %   l   #   #
        //   % . l # S # r          |     %   .   .   .
        //   |   . # # #            |       %
        //   |     . #
        //
        // (NB: y increases downwards)
        // So, the ranges of sensors s1 and s2 are 1 apart if:
        //   s1'.l.x - 1 == s2'.r.x + 1
        //   s1'.r.x + 1 == s2'.l.x - 1
        //   s1'.l.y + 1 == s2'.r.y - 1
        //   s1'.r.y - 1 == s2'.l.y + 1
        sensor[n++] = (Square){rotleft((Vec){sx - r - 1, sy}), rotleft((Vec){sx + r + 1, sy})};
    }
    const Vec d = rotright(distress(n));
    printf("%d %"PRId64"\n", maxx - minx + 1 - m, F * d.x + d.y);  // 5838453 12413999391794 (does not work for example)
    // (Assume all beacons on y=Y fall inside overlapping range)

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
