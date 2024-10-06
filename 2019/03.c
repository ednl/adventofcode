/**
 * Advent of Code 2019
 * Day 3: Crossed Wires
 * https://adventofcode.com/2019/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 03.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 03.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          : 159 µs (93 µs without reading & parsing the input file)
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)            :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#include <limits.h>  // INT_MAX
#include <stdbool.h>
#include "../startstoptimer.h"

#define WIRES 2
#define MAXSEG 500

typedef enum dir {
    HORZ, VERT
} Dir;

// dir = wire segment is HORZ or VERT
// len = total wire length from start to here
typedef struct seg {
    Dir dir;
    int x, y, len;
} Seg;

typedef struct intersect {
    int dist, time;
} Intersect;

static Seg wire[WIRES][MAXSEG];
static int count[WIRES];

static bool intersect(const int i, const int j, Intersect *const cross)
{
    if (wire[0][i].dir == wire[1][j].dir)
        return false;
    const Seg *h, *v;
    if (wire[0][i].dir == HORZ) {
        h = &wire[0][i];
        v = &wire[1][j];
    } else {
        h = &wire[1][j];
        v = &wire[0][i];
    }
    if ((((h - 1)->x < v->x && v->x <= h->x) || (h->x <= v->x && v->x < (h - 1)->x)) &&
        (((v - 1)->y < h->y && h->y <= v->y) || (v->y <= h->y && h->y < (v - 1)->y))) {
        *cross = (Intersect){abs(v->x) + abs(h->y), h->len + v->len - abs(h->x - v->x) - abs(v->y - h->y)};
        return true;
    }
    return false;
}

int main(void)
{
    starttimer();
    FILE *f = fopen("../aocinput/2019-03-input.txt", "r");
    if (!f) { fprintf(stderr, "File not found.\n"); return 1; }
    for (int i = 0; i < WIRES; ++i) {
        const Seg *const begin = &wire[i][0];
        const Seg *const end = begin + MAXSEG;
        const Seg *prev = begin;
        Seg *cur = (Seg *)begin + 1;
        for (int c = 0; c != '\n' && cur != end; prev = cur++) {
            const int dir = fgetc(f);
            int len = 0;
            while ((c = fgetc(f)) >= '0')  // ends at ',' or '\n'
                len = len * 10 + (c & 15);  // atoi
            switch (dir) {
                case 'U': *cur = (Seg){VERT, prev->x, prev->y + len, prev->len + len}; break;
                case 'D': *cur = (Seg){VERT, prev->x, prev->y - len, prev->len + len}; break;
                case 'L': *cur = (Seg){HORZ, prev->x - len, prev->y, prev->len + len}; break;
                case 'R': *cur = (Seg){HORZ, prev->x + len, prev->y, prev->len + len}; break;
            }
        }
        count[i] = cur - begin;
    }
    fclose(f);
    Intersect cross, min = (Intersect){INT_MAX, INT_MAX};
    for (int i = 1; i < count[0]; ++i)
        for (int j = 1; j < count[1]; ++j)
            if (intersect(i, j, &cross)) {
                if (cross.dist < min.dist) min.dist = cross.dist;
                if (cross.time < min.time) min.time = cross.time;
            }
    printf("%d %d\n", min.dist, min.time);  // 209 43258
    printf("Time: %.0f µs\n", stoptimer_us());
    return 0;
}
