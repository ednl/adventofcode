/**
 * Advent of Code 2021
 * Day 9: Smoke Basin
 * https://adventofcode.com/2021/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 09.c
 *    gcc   -std=gnu17 -Wall -Wextra 09.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 09.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 09.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    : 185 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define DIM        100
#define MAXHEIGHT    9
#define MAXBASINS  256
#define STACKSIZE  100

typedef struct {
    int x, y;
} Point;

static unsigned char height[DIM + 2][DIM + 2] = {0};  // extend grid to simplify edge cases
static bool done[DIM + 2][DIM + 2] = {0};  // keep track of already processed locations
static Point todo[STACKSIZE] = {0};  // keep track of locations still to be processed
static int basinsize[MAXBASINS] = {0};  // size of each basin found

// Floodfill the basin, return basin size
static int fillbasin(int x, int y)
{
    int sp = 1, size = 0;     // sp = stack pointer
    todo[0] = (Point){x, y};  // prefill stack with first location
    while (sp) {
        Point p = todo[--sp];
        if (!done[p.x][p.y]) {
            done[p.x][p.y] = true;
            if (height[p.x][p.y] != MAXHEIGHT) {
                if (sp > STACKSIZE - 4) {
                    fprintf(stderr, "Imminent stack overflow!\n");
                    return -1;
                }
                if (!done[p.x + 1][p.y]) todo[sp++] = (Point){p.x + 1, p.y};
                if (!done[p.x - 1][p.y]) todo[sp++] = (Point){p.x - 1, p.y};
                if (!done[p.x][p.y + 1]) todo[sp++] = (Point){p.x, p.y + 1};
                if (!done[p.x][p.y - 1]) todo[sp++] = (Point){p.x, p.y - 1};
                ++size;
            }
        }
    }
    return size;
}

// Sort int array in descending order
static int descending(const void *a, const void *b)
{
    const int p = *(const int*)a;
    const int q = *(const int*)b;
    return (p < q) - (q < p);
}

int main(void)
{
    FILE *f = fopen("../aocinput/2021-09-input.txt", "r");
    for (int i = 1; i <= DIM; ++i) {
        for (int j = 1; j <= DIM; ++j)
            height[i][j] = (unsigned char)(fgetc(f) - '0');
        fgetc(f);  // newline
    }
    fclose(f);

#ifdef TIMER
    // Timer starts after read from disk
    starttimer();
#endif
    // None shall pass
    for (int i = 0; i < DIM + 2; ++i)
        height[i][0] = height[0][i] = height[i][DIM + 1] = height[DIM + 1][i] = MAXHEIGHT;

    int risk = 0;
    size_t basincount = 0;
    for (int i = 1; i <= DIM; ++i) {
        for (int j = 1; j <= DIM; ++j) {
            if (height[i][j] < height[i - 1][j] &&
                height[i][j] < height[i][j - 1] &&
                height[i][j] < height[i + 1][j] &&
                height[i][j] < height[i][j + 1]) {
                risk += height[i][j] + 1;
                if (basincount == MAXBASINS) {
                    fprintf(stderr, "The number of basins is too damn high!\n");
                    return 1;
                }
                basinsize[basincount++] = fillbasin(i, j);
            }
        }
    }
    qsort(basinsize, basincount, sizeof *basinsize, descending);

    printf("Part 1: %d\n", risk);  // 506
    printf("Part 2: %d\n", basinsize[0] * basinsize[1] * basinsize[2]);  // 931200
#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
