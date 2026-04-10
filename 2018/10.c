/**
 * Advent of Code 2018
 * Day 10:
 * https://adventofcode.com/2018/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.83 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ?    µs
 *     Raspberry Pi 5 (2.4 GHz)      : ?    µs
*/

#include <stdio.h>
#include <string.h>  // memset
#include <limits.h>  // INT_MIN, INT_MAX
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Number of points in my input file
#define N 324

// Matrix display dimensions with hindsight
#define W (8 * (6 + 2))  // 8 characters of 6 dots wide with 2 spaces inbetween, +\n\0
#define H 10  // 10 dots high

typedef struct {
    int x, y;
} Vec;

// Old and new positions, constant velocity
static Vec pos0[N];
static Vec pos1[N];
static Vec vel[N];

// Matrix display
static char matrix[H][W];

// Read input file, return number of lines read
static int read(void)
{
    FILE *f = fopen("../aocinput/2018-10-input.txt", "r");
    if (!f)
        return 0;  // error: zero lines read
    int n = 0;
    Vec p, v;
    for (; n < N && fscanf(f, "position=< %d, %d> velocity=< %d, %d> ", &p.x, &p.y, &v.x, &v.y) == 4; n++) {
        pos0[n] = p;
        vel[n] = v;
    }
    fclose(f);
    return n;
}

int main(void)
{
    // Read input file into globals pos0[] and vel[]
    if (read() < N)
        return 1;

#ifdef TIMER
    starttimer();
#endif

    // Intersection: x0 + vx0.t = x1 + vx1.t
    //   => t = (x0 - x1) / (vx1 - vx0)
    // will be close approx of final t
    int i = 1;
    while (pos0[i].x == pos0[0].x || vel[i].x == vel[0].x)
        i++;  // skip parallel lines or (unlikely) already overlapping points
    int t = (pos0[0].x - pos0[i].x) / (vel[i].x - vel[0].x);

    // Go to initial position in one big step
    // and determine spread of the points
    Vec pmin = {INT_MAX, INT_MAX};
    Vec pmax = {INT_MIN, INT_MIN};
    for (int i = 0; i < N; ++i) {
        pos0[i].x += vel[i].x * t;
        pos0[i].y += vel[i].y * t;
        if (pos0[i].x < pmin.x) pmin.x = pos0[i].x;
        if (pos0[i].x > pmax.x) pmax.x = pos0[i].x;
        if (pos0[i].y < pmin.y) pmin.y = pos0[i].y;
        if (pos0[i].y > pmax.y) pmax.y = pos0[i].y;
    }
    Vec dim0, dim1 = (Vec){pmax.x - pmin.x, pmax.y - pmin.y};

    // First try going forward (single time steps)
    // p keeps track of the best coordinates, q is the next set
    Vec *p = pos0, *q = pos1, *tmp;
    bool better;
    do {
        dim0 = dim1;
        pmin = (Vec){INT_MAX, INT_MAX};
        pmax = (Vec){INT_MIN, INT_MIN};
        for (i = 0; i < N; ++i) {
            q[i].x = p[i].x + vel[i].x;
            q[i].y = p[i].y + vel[i].y;
            if (q[i].x < pmin.x) pmin.x = q[i].x;
            if (q[i].x > pmax.x) pmax.x = q[i].x;
            if (q[i].y < pmin.y) pmin.y = q[i].y;
            if (q[i].y > pmax.y) pmax.y = q[i].y;
        }
        dim1 = (Vec){pmax.x - pmin.x, pmax.y - pmin.y};
        if ((better = dim1.x <= dim0.x && dim1.y <= dim0.y)) {
            t++;
            tmp = p;
            p = q;
            q = tmp;
        }
    } while (better);

    // Then try going backward (single time steps)
    // while remembering the previous minimal coordinates
    Vec pmin0;
    do {
        dim0 = dim1;
        pmin0 = pmin;  // save minimum
        pmin = (Vec){INT_MAX, INT_MAX};
        pmax = (Vec){INT_MIN, INT_MIN};
        for (int i = 0; i < N; ++i) {
            q[i].x = p[i].x - vel[i].x;
            q[i].y = p[i].y - vel[i].y;
            if (q[i].x < pmin.x) pmin.x = q[i].x;
            if (q[i].x > pmax.x) pmax.x = q[i].x;
            if (q[i].y < pmin.y) pmin.y = q[i].y;
            if (q[i].y > pmax.y) pmax.y = q[i].y;
        }
        dim1 = (Vec){pmax.x - pmin.x, pmax.y - pmin.y};
        if ((better = dim1.x <= dim0.x && dim1.y <= dim0.y)) {
            t--;
            tmp = p;
            p = q;
            q = tmp;
        }
    } while (better);

    // Rebase to zero using the saved minimal coordinates
    for (int i = 0; i < N; ++i) {
        p[i].x -= pmin0.x;
        p[i].y -= pmin0.y;
    }

    memset(matrix, ' ', sizeof matrix);
    for (int i = 0; i < N; ++i)
        matrix[p[i].y][p[i].x] = '#';  // ERCXLAJL
    for (int i = 0; i < H; ++i) {
        matrix[i][W - 2] = '\n';
        matrix[i][W - 1] = '\0';
        printf("%s", matrix[i]);
    }
    printf("%d\n", t);  // 10813

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
