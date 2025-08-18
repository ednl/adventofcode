/**
 * Advent of Code 2015
 * Day 14: Reindeer Olympics
 * https://adventofcode.com/2015/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 14.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 14.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                  :  44 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz)         :   ? µs
 *     ThinkPad Ubuntu (i5 8250U 1.8 GHz)          :   ? µs
 *     Macbook Air 2013 (i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                    :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // div_t
#include "../startstoptimer.h"

#define N 9     // 9 reindeer in input file
#define T 2503  // total time given in puzzle

typedef struct reindeer {
    int speed, fly, period, dist, points;
} Reindeer;

static Reindeer reindeer[N];

static int distance(const Reindeer *const r, const int t)
{
    const div_t count = div(t, r->period);
    return r->speed * (count.quot * r->fly + (count.rem < r->fly ? count.rem : r->fly));
}

int main(void)
{
    FILE *f = fopen("../aocinput/2015-14-input.txt", "r");
    if (!f) return 1;
    const Reindeer *const end = &reindeer[N];
    for (Reindeer *r = reindeer; r != end; ++r) {
        int speed, fly, rest;
        if (fscanf(f, "%*s can fly %d km/s for %d seconds, but then must rest for %d seconds.", &speed, &fly, &rest) == 3)
            *r = (Reindeer){speed, fly, fly + rest, 0, 0};
    }
    fclose(f);

    starttimer();
    int lead = 0;
    for (int t = 1; t <= T; ++t) {
        for (Reindeer *r = reindeer; r != end; ++r)
            if ((r->dist = distance(r, t)) > lead)
                lead = r->dist;
        for (Reindeer *r = reindeer; r != end; ++r)
            r->points += r->dist == lead;
    }

    int maxdist = 0;
    int maxpoints = 0;
    for (const Reindeer *r = reindeer; r != end; ++r) {
        if (r->dist   > maxdist  ) maxdist   = r->dist;
        if (r->points > maxpoints) maxpoints = r->points;
    }
    printf("%d %d\n", maxdist, maxpoints);  // 2660 1256
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
