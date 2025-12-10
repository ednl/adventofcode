/**
 * Advent of Code 2025
 * Day 9: Movie Theater
 * https://adventofcode.com/2025/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 09.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 32 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // abs
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <limits.h>    // INT_MAX
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-09-example.txt"
    #define N 8
#else
    #define FNAME "../aocinput/2025-09-input.txt"
    #define N 496
#endif

typedef struct vec {
    int x, y;
} Vec;

typedef struct box {
    Vec min, max;
} Box;

// Red tiles are consecutive nodes of a closed, non-intersecting path
// with alternating horizontal and vertical edges
static Vec red[N + 1];  // +1 to close the loop with value 0 repeated

// Integer rectangle area with borders, from any two points
static int64_t rectarea(const Vec a, const Vec b)
{
    return (int64_t)(abs(a.x - b.x) + 1) * (abs(a.y - b.y) + 1);
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 0, x, y; i < N && fscanf(f, "%d,%d", &x, &y) == 2; ++i)
        red[i] = (Vec){x, y};
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    int64_t largest = 0;
    for (int i = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j) {  // every unique combination of red tiles
                const int64_t area = rectarea(red[i], red[j]);
                if (area > largest)
                    largest = area;
            }
    printf("Part 1: %"PRId64"\n", largest);  // example: 50, input: 4739623064

#if !EXAMPLE
    // Part 2
    // Looked at data: large jump in x for two points, making a gap into a circle
    // Vec corner1 = red[248];  // large jump to the right from prev to here
    // Vec corner2 = red[249];  // large jump to the left from here to next, dy<0 from corner1
    largest = 0;
    {
        const int k = 249;
        const Vec a = red[k];
        int i = k + 200;  // opposite vertical quadrant
        for (; red[i].x < a.x; ++i);
        int ymin = red[i].y;
        int j = k + 2;  // first with dy!=0 from corner
        int xmin = red[j].x;
        bool movedcloser = false;
        while (red[j].y >= ymin) {
            if (red[j].x >= xmin) {
                const int64_t area = rectarea(a, red[j]);
                if (area > largest)
                    largest = area;
            }
            ++j;
            if ((!movedcloser && red[j].x < xmin) || (movedcloser && red[j].x > xmin))
                xmin = red[j].x;
        }
    }
    {
        const int k = 248;
        const Vec a = red[k];
        int i = k - 200;  // opposite vertical quadrant
        for (; red[i].x < a.x; --i);
        int ymax = red[i].y;
        int j = k - 2;  // first with dy!=0 from corner
        int xmin = red[j].x;
        bool movedcloser = false;
        while (red[j].y <= ymax) {
            if (red[j].x >= xmin) {
                const int64_t area = rectarea(a, red[j]);
                if (area > largest)
                    largest = area;
            }
            --j;
            if ((!movedcloser && red[j].x < xmin) || (movedcloser && red[j].x > xmin))
                xmin = red[j].x;
        }
    }
    printf("Part 2: %"PRId64"\n", largest);  // example: 24, input: 1654141440
#endif

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
