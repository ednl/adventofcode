/**
 * Advent of Code 2018
 * Day 3: No Matter How You Slice It
 * https://adventofcode.com/2018/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 179 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 684 µs
*/

#include <stdio.h>   // fopen, fclose, fscanf, printf
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-03-input.txt"
#define N 1293  // number of claims (lines in input file)
#define M 1000  // fabric dimension MxM "at least 1000x1000"

typedef struct pos {
    int x, y;
} Pos;

typedef struct claim {
    int id;
    Pos a, b;  // a = top left corner, b = 1 outside bottom right corner
} Claim;

static Claim claim[N];
static unsigned char fabric[M][M];

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    for (int i = 0, id, x, y, w, h; i < N &&
        fscanf(f, "#%d @ %d,%d: %dx%d ", &id, &x, &y, &w, &h) == 5; ++i)
        claim[i] = (Claim){id, {x, y}, {x + w, y + h}};
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Lay claim
    int overlap = 0;
    const Claim *c = claim;
    for (int i = 0; i < N; ++i, ++c)
        for (int x = c->a.x; x < c->b.x; ++x)
            for (int y = c->a.y; y < c->b.y; ++y)
                if (fabric[x][y]++ == 1)
                    overlap++;
    printf("%d\n", overlap);  // 110827

    // Find claim without overlap
    c = claim;
    for (int i = 0; i < N; ++i, ++c) {
        for (int x = c->a.x; x < c->b.x; ++x)
            for (int y = c->a.y; y < c->b.y; ++y)
                if (fabric[x][y] > 1)
                    goto nexti;
        printf("%d\n", c->id);  // 116
    #ifdef TIMER
        printf("Time: %.0f us\n", stoptimer_us());
    #endif
        return 0;
    nexti:;
    }
    return 1;  // claim without overlap not found
}
