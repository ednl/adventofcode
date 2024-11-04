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
#include <stdlib.h>

#define N 8192
static char dir[N];

int main(void)
{
    FILE *f = fopen("../aocinput/2015-03-input.txt", "r");
    if (!f) return 1;
    fread(dir, N, 1, f);
    fclose(f);
    int x = 0, y = 0, minx = 0, miny = 0, maxx = 0, maxy = 0;
    for (int i = 0; i < N; ++i)
        switch (dir[i]) {
            case '<': --x; if (x < minx) minx = x; break;
            case '>': ++x; if (x > maxx) maxx = x; break;
            case '^': --y; if (y < miny) miny = y; break;
            case 'v': ++y; if (y > maxy) maxy = y; break;
        }
    const int width = maxx - minx + 1;
    const int count = width * (maxy - miny + 1);
    const int start = -minx - miny * width;
    char *grid = calloc(count, sizeof *grid);
    grid[start] = 1;
    for (int i = 0, j = start; i < N; ++i, grid[j] = 1)
        switch (dir[i]) {
            case '<': --j; break;
            case '>': ++j; break;
            case '^': j -= width; break;
            case 'v': j += width; break;
        }
    int sum = 0;
    for (int i = 0; i < count; ++i, sum += grid[i]);
    printf("%d\n", sum);  // 2565, 2639
    free(grid);
    return 0;
}
