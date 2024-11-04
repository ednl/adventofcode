/**
 * Advent of Code 2015
 * Day 3: Perfectly Spherical Houses in a Vacuum
 * https://adventofcode.com/2015/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 03.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 03.c
 */

#include <stdio.h>
#include <stdlib.h>  // calloc, free
#include <string.h>  // memset

#define N 8192
static char dir[N];

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

static Box getsize(const int start, const int step)
{
    Vec pos = (Vec){0};
    Vec min = (Vec){0};
    Vec max = (Vec){0};
    for (int i = start; i < N; i += step)
        switch (dir[i]) {
            case '<': if (--pos.x < min.x) min.x = pos.x; break;
            case '>': if (++pos.x > max.x) max.x = pos.x; break;
            case '^': if (--pos.y < min.y) min.y = pos.y; break;
            case 'v': if (++pos.y > max.y) max.y = pos.y; break;
        }
    return (Box){min, max};
}

int main(void)
{
    FILE *f = fopen("../aocinput/2015-03-input.txt", "r");
    if (!f) return 1;
    fread(dir, N, 1, f);
    fclose(f);

    // Calc grid size as outer bounding box of 3 different walks
    Box a = getsize(0, 1);  // part 1
    Box b = getsize(0, 2);  // part 2a
    Box c = getsize(1, 2);  // part 2b
    Box d = (Box){
        (Vec){min(a.min.x, min(b.min.x, c.min.x)), min(a.min.y, min(b.min.y, c.min.y))},
        (Vec){max(a.max.x, max(b.max.x, c.max.x)), max(a.max.y, max(b.max.y, c.max.y))}
    };

    const int cols = d.max.x - d.min.x + 1;
    const int size = cols * (d.max.y - d.min.y + 1);
    char *grid = calloc(size, sizeof *grid);  // set to zero
    char *p, *q, *const start = grid - d.min.x - d.min.y * cols;

    // Part 1
    p = start;
    *p = 1;
    for (int i = 0; i < N; ++i, *p = 1)
        switch (dir[i]) {
            case '<': p--; break;
            case '>': p++; break;
            case '^': p -= cols; break;
            case 'v': p += cols; break;
        }
    int sum = 0;
    for (int i = 0; i < size; ++i, sum += grid[i]);
    printf("Part 1: %d\n", sum);  // 2565

    // Part 2
    memset(grid, 0, size);  // reset to zero
    p = q = start;
    *p = 1;
    for (int i = 0; i < N; i += 2, *p = *q = 1) {
        switch (dir[i]) {
            case '<': p--; break;
            case '>': p++; break;
            case '^': p -= cols; break;
            case 'v': p += cols; break;
        }
        switch (dir[i + 1]) {
            case '<': q--; break;
            case '>': q++; break;
            case '^': q -= cols; break;
            case 'v': q += cols; break;
        }
    }
    sum = 0;
    for (int i = 0; i < size; ++i, sum += grid[i]);
    printf("Part 2: %d\n", sum);  // 2639

    free(grid);
    return 0;
}
