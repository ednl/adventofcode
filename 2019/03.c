/**
 * Advent of Code 2019
 * Day 3: The Tyranny of the Rocket Equation
 * https://adventofcode.com/2019/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  8.64 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 13.6  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 38.5  µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, abs
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-03-input.txt"
#define FSIZE 3072  // needed for my input: 2930
#define WIRES 2    // 2 lines in input file
#define ORIS  2    // 2 orientations: U/D=vert, L/R=horz
#define SEGS  160  // needed for my input: 301/2+1=151

// Direction
typedef enum dir {
    UP, DOWN, LEFT, RIGHT
} Dir;

// Orientation
typedef enum ori {
    VERT, HORZ
} Ori;

// Location
typedef struct point {
    int x, y;
} Point;

// Wire segment
typedef struct seg {
    Point p0, p1;  // either .x or .y is the same, other one is sorted: p0.? <= p1.?
    int dist;  // distance at p0
    int step;  // +1 if wire direction is p0 -> p1, -1 if p0 <- p1
} Seg;

// Input file
static char input[FSIZE];

// Two wires, two orientations, up to 160 segments
static Seg wire[WIRES][ORIS][SEGS];

static int asc_x(const void *p, const void *q)
{
    const int *a = p, *b = q;  // ::p0.x must be first field of Seg
    if (*a < *b) return -1;
    if (*a > *b) return +1;
    return 0;
}

// Binary search for first index of vert line that might cross horz segment from xmin onwards
// vert must be sorted by ::p0.x (= ::p1.x)
static int first(const Seg *const vert, const int len, const int xmin)
{
    if (vert->p0.x >= xmin) return 0;  // this is the first
    if (vert[len - 1].p0.x < xmin) return -1;  // there is no first
    int l = 0, r = len - 1;
    // Now always: v[l].p0.x < xmin <= v[r].p0.x
    while (r - l > 1) {
        const int m = (l + r + 1) >> 1;  // l,r <= 150 so never overflow
        if (vert[m].p0.x < xmin) l = m;
        else                     r = m;
    }
    return r;  // v[r].p0.x >= xmin
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse input file
    int len[WIRES][ORIS] = {0};  // count of vert/horz segments of wires 0/1
    int n = 0, x1 = 0, y1 = 0, d1 = 0;
    for (const char *c = input; *c; ++c) {
        Dir dir;  // input is good so safe to not init
        switch (*c++) {
            case 'U': dir = UP;    break;
            case 'D': dir = DOWN;  break;
            case 'L': dir = LEFT;  break;
            case 'R': dir = RIGHT; break;
        }
        // Parse int
        int step = *c++ & 15;
        while (*c > ',')  // until comma or newline
            step = step * 10 + (*c++ & 15);
        // Save previous location
        const int x0 = x1, y0 = y1, d0 = d1;
        // Next location
        d1 += step;
        switch (dir) {
            // Keep (x0,y0)->(x1,y1) sorted in ascending order (not necessarily same as wire direction)
            case UP   : wire[n][VERT][len[n][VERT]++] = (Seg){{x1, (y1 -= step)}, {x0, y0}, d1, -1}; break;
            case DOWN : wire[n][VERT][len[n][VERT]++] = (Seg){{x0, y0}, {x1, (y1 += step)}, d0, +1}; break;
            case LEFT : wire[n][HORZ][len[n][HORZ]++] = (Seg){{(x1 -= step), y1}, {x0, y0}, d1, -1}; break;
            case RIGHT: wire[n][HORZ][len[n][HORZ]++] = (Seg){{x0, y0}, {(x1 += step), y1}, d0, +1}; break;
        }
        // Next wire
        if (*c == '\n') {
            n++;
            x1 = y1 = d1 = 0;
        }
    }

    int mindist = 1 << 30;
    int minpath = 1 << 30;
    for (int i = 0, j = 1; i < 2; ++i, --j) {
        Seg *h = wire[i][HORZ];
        const Seg *const hend = &h[len[i][HORZ]];
        Seg *const v = wire[j][VERT];
        const int vlen = len[j][VERT];
        qsort(v, vlen, sizeof (Seg), asc_x);
        for (; h != hend; ++h)
            for (int k = first(v, vlen, h->p0.x); k >= 0 && k < vlen && v[k].p0.x <= h->p1.x; ++k)
                // Fixed x of vertical line crosses horizontal segment
                if (v[k].p0.y <= h->p0.y && h->p0.y <= v[k].p1.y) {
                    // Fixed y of horizontal line crosses vertical segment
                    const int dist = abs(v[k].p0.x) + abs(h->p0.y);
                    if (dist < mindist)
                        mindist = dist;
                    const int path = h->dist + h->step * (v[k].p0.x - h->p0.x)
                        + v[k].dist + v[k].step * (h->p0.y - v[k].p0.y);
                    if (path < minpath)
                        minpath = path;
                }
    }
    printf("%d %d\n", mindist, minpath);  // part 1: 209, part 2: 43258

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
