/**
 * Advent of Code 2017
 * Day 3: Spiral Memory
 * https://adventofcode.com/2017/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 03.c -lm
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c -lm
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.71 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 7.05 µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#include <unistd.h>  // isatty, fileno
#include <math.h>    // sqrt, ceil
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-03-input.txt"

// Part 2 spiral array with fixed dimensions and border
#define R 5            // max ring index (zero based), also offset for centre coordinates
#define L (R * 2 + 1)  // odd side length of the R'th ring which is a square

static int inp;
static int spiral[L][L];

// Read personalised input value from file or command line
static void parse(const int argc, char *const *const argv)
{
    if (argc > 1) {
        if (sscanf(argv[1], "%d", &inp) != 1)
            fprintf(stderr, "Argument must be a number\n");
    } else if (!isatty(fileno(stdin))) {
        if (fscanf(stdin, "%d", &inp) != 1)
            fprintf(stderr, "Piped or redirected input must be a number\n");
    } else {
        FILE *f = fopen(FNAME, "r");
        if (!f)
            fprintf(stderr, "File not found: "FNAME"\n");
        if (fscanf(f, "%d", &inp) != 1)
            fprintf(stderr, "File must contain a number: "FNAME"\n");
        fclose(f);
    }
    if (inp < 1)
        fprintf(stderr, "Invalid input: %d\n", inp);
}

// Part 1: locate value in spiral, return distance to centre
static int locate(void)
{
    const int r = (int)ceil((sqrt(inp) - 1)/2);  // radius = ring index
    const int d = r * 2;                         // diameter = ring side length - 1
    const int max = (d + 1) * (d + 1);           // value in bottom-right corner of this ring
    int mid = max - r;                           // value at midpoint of last side
    int dist, next = abs(mid - inp);             // distance from first midpoint
    do {
        dist = next;            // base value
        mid -= d;               // next midpoint (counting back)
        next = abs(mid - inp);  // next value
    } while (next < dist);      // find minimum distance from 4 midpoints
    return r + dist;            // manhattan distance from input value to centre
}

// Sum of 3 horizontal neighbours up/down while going left/right
static int nh(const int i, const int j)
{
    return spiral[i][j - 1] + spiral[i][j] + spiral[i][j + 1];
}

// Sum of 3 vertical neighbours left/right while going up/down
static int nv(const int i, const int j)
{
    return spiral[i - 1][j] + spiral[i][j] + spiral[i + 1][j];
}

// Part 2: https://oeis.org/A141481
// but return when current value greater than input
static int oeis_a141481(void)
{
    spiral[R][R] = 1;              // start of the spiral, all other values are zero thru static init
    for (int r = 1; r < R; ++r) {  // leave margin for neighbours at border
        const int d = r * 2;       // "diameter" = side length - 1
        int v, i = R + r, j = i;   // lower right corner = start of new square (R = offset to centre point)
        for (int k = 0; k < d; ++k) { v = spiral[i][j]; v += nv(--i, j-1); if (v > inp) return v; spiral[i][j] = v; }  // go up
        for (int k = 0; k < d; ++k) { v = spiral[i][j]; v += nh(i+1, --j); if (v > inp) return v; spiral[i][j] = v; }  // go left
        for (int k = 0; k < d; ++k) { v = spiral[i][j]; v += nv(++i, j+1); if (v > inp) return v; spiral[i][j] = v; }  // go down
        for (int k = 0; k < d; ++k) { v = spiral[i][j]; v += nh(i-1, ++j); if (v > inp) return v; spiral[i][j] = v; }  // go right
    }
    return 0;
}

int main(int argc, char *argv[])
{
    parse(argc, argv);
    if (inp < 1)
        return 1;

#ifdef TIMER
    starttimer();
#endif

    printf("%d\n", locate());  // 552
    printf("%d\n", oeis_a141481());  // 330785

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
