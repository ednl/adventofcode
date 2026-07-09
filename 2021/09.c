/**
 * Advent of Code 2021
 * Day 9: Smoke Basin
 * https://adventofcode.com/2021/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic ../topn.c 09.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c ../topn.c 09.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  84 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 134 µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <stdbool.h>
#include "../topn.h"
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define DIM        100
#define MAXHEIGHT    9
#define MAXBASINS  256
#define STACKSIZE  100

typedef struct point {
    int x, y;
} Point;

static char height[DIM + 2][DIM + 2];  // extend grid to simplify edge cases
static bool done[DIM + 2][DIM + 2];    // keep track of already processed locations
static Point todo[STACKSIZE];          // keep track of locations still to be processed
static int basinsize[MAXBASINS];       // size of each basin found

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
static int descending(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    return (a < b) - (b < a);
}

int main(void)
{
    FILE *f = fopen("../aocinput/2021-09-input.txt", "r");
    for (int i = 1; i <= DIM; ++i) {
        for (int j = 1; j <= DIM; ++j)
            height[i][j] = fgetc(f) - '0';
        fgetc(f);  // skip newline
    }
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // None shall pass
    for (int i = 1; i < DIM + 1; ++i)
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
                basinsize[basincount++] = fillbasin(i, j);
            }
        }
    }
    topn(basinsize, 3, basincount, sizeof *basinsize, descending);
    printf("%d %d\n", risk, basinsize[0] * basinsize[1] * basinsize[2]);  // 506 931200

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
