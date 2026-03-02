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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.75 µs
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

// Part 2 spiral array fixed dimensions
#define R 5            // max ring index (centre=0), also offset for spiral centre
#define L (R * 2 + 1)  // side length of the R'th ring which is a square

static int input = -1;    // read personalised value from file or command line
static int spiral[L][L];  // used in part 2

// Sum of 3 horizontal neighbours up/down while going left/right
static int nh(const int x, const int y)
{
    return spiral[y][x - 1] + spiral[y][x] + spiral[y][x + 1];
}

// Sum of 3 vertical neighbours left/right while going up/down
static int nv(const int x, const int y)
{
    return spiral[y - 1][x] + spiral[y][x] + spiral[y + 1][x];
}

// https://oeis.org/A141481
// but return when current value greater than input
static int oeis_a141481(const int m)
{
    spiral[R][R] = 1;              // start of the spiral, all other values are zero thru static init
    for (int r = 1; r < R; ++r) {  // leave margin for neighbours at border
        const int d = r * 2;       // "diameter" = side length - 1
        int n, x = R + r, y = x;   // lower right corner = start of new square (R = offset to centre point)
        for (int i = 0; i < d; ++i) { n = spiral[y][x]; n += nv(x-1, --y); if (n > m) return n; spiral[y][x] = n; }  // go up
        for (int i = 0; i < d; ++i) { n = spiral[y][x]; n += nh(--x, y+1); if (n > m) return n; spiral[y][x] = n; }  // go left
        for (int i = 0; i < d; ++i) { n = spiral[y][x]; n += nv(x+1, ++y); if (n > m) return n; spiral[y][x] = n; }  // go down
        for (int i = 0; i < d; ++i) { n = spiral[y][x]; n += nh(++x, y-1); if (n > m) return n; spiral[y][x] = n; }  // go right
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc > 1) {
        if (sscanf(argv[1], "%d", &input) != 1) {
            fprintf(stderr, "Argument must be a number\n");
            return 1;
        }
    } else if (!isatty(fileno(stdin))) {
        if (fscanf(stdin, "%d", &input) != 1) {
            fprintf(stderr, "Piped or redirected input must be a number\n");
            return 1;
        }
    } else {
        FILE *f = fopen(FNAME, "r");
        if (!f) {
            fprintf(stderr, "File not found: "FNAME"\n");
            return 1;
        }
        if (fscanf(f, "%d", &input) != 1) {
            fprintf(stderr, "File must contain a number: "FNAME"\n");
            return 1;
        }
        fclose(f);
    }
    if (input < 0) {
        fprintf(stderr, "Invalid input: %d\n", input);
        return 1;
    }

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    const int r = (int)ceil((sqrt(input) - 1)/2);  // radius
    const int d = 2 * r;                           // diameter
    const int max = (d + 1) * (d + 1);             // value in bottom-right corner of this ring
    int mid = max - r;                             // value at mid point of side
    int step, next = abs(mid - input);             // deviation from midpoint
    do {
        step = next;
        mid -= d;
        next = abs(mid - input);
    } while (next < step);  // find minimum deviation from consecutive midpoints
    printf("%d\n", r + step);  // 552

    // Part 2
    printf("%d\n", oeis_a141481(input));  // 330785

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
