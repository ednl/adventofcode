/**
 * Advent of Code 2015
 * Day 3: Perfectly Spherical Houses in a Vacuum
 * https://adventofcode.com/2015/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 44 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // calloc, free
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2015-03-input.txt"
#define N 8192  // characters in input file (no newline)

static char move[N];

typedef struct vec {
    int x, y;
} Vec;

typedef struct box {
    Vec min, max;
} Box;

static int min(const int a, const int b)
{
    return a < b ? a : b;
}

static int max(const int a, const int b)
{
    return a > b ? a : b;
}

static Box walksize(const int skip, const int step)
{
    Vec pos = {0};
    Vec min = {0};
    Vec max = {0};
    for (int i = skip; i < N; i += step)
        switch (move[i]) {
            case '<': if (--pos.x < min.x) min.x = pos.x; break;
            case '>': if (++pos.x > max.x) max.x = pos.x; break;
            case '^': if (--pos.y < min.y) min.y = pos.y; break;
            case 'v': if (++pos.y > max.y) max.y = pos.y; break;
        }
    return (Box){min, max};
}

static int sum(const char *const a, const int len)
{
    int x = 0;
    for (int i = 0; i < len; ++i)
        x += a[i];
    return x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(move, sizeof move, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Calc grid size as outer bounding box of 3 different walks
    const Box a = walksize(0, 1);  // part 1
    const Box b = walksize(0, 2);  // part 2a
    const Box c = walksize(1, 2);  // part 2b
    const Box d = {
        {min(a.min.x, min(b.min.x, c.min.x)), min(a.min.y, min(b.min.y, c.min.y))},
        {max(a.max.x, max(b.max.x, c.max.x)), max(a.max.y, max(b.max.y, c.max.y))}
    };
    const int cols = d.max.x - d.min.x + 1;
    const int size = cols * (d.max.y - d.min.y + 1);  // 143x129=18447 for my input
    char *const grid = calloc(size, sizeof *grid);  // allocate and set to zero
    if (!grid)
        return 2;
    char *const origin = grid - d.min.x - d.min.y * cols;  // starting position (min.x and min.y are <0)

    // Part 1
    char *p = origin;
    *p = 1;
    for (int i = 0; i < N; ++i, *p = 1)
        switch (move[i]) {
            case '<': p--; break;
            case '>': p++; break;
            case '^': p -= cols; break;
            case 'v': p += cols; break;
        }
    printf("Part 1: %d\n", sum(grid, size));  // 2565

    // Part 2
    memset(grid, 0, size);  // reset to zero
    char *q = p = origin;
    *p = 1;
    for (int i = 0; i < N; i += 2, *p = *q = 1) {
        switch (move[i]) {
            case '<': p--; break;
            case '>': p++; break;
            case '^': p -= cols; break;
            case 'v': p += cols; break;
        }
        switch (move[i + 1]) {
            case '<': q--; break;
            case '>': q++; break;
            case '^': q -= cols; break;
            case 'v': q += cols; break;
        }
    }
    printf("Part 2: %d\n", sum(grid, size));  // 2639

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif

    free(grid);
    return 0;
}
