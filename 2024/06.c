/**
 * Advent of Code 2024
 * Day 6: ?
 * https://adventofcode.com/2024/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 06.c
 *    gcc   -std=gnu17 -Wall -Wextra 06.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 06.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 06.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE
    #define FNAME "../aocinput/2024-06-example.txt"
    #define N 10  // rows and cols of example grid
#else
    #define FNAME "../aocinput/2024-06-input.txt"
    #define N 130  // rows and cols of input grid
#endif
#define FSIZE N * (N + 1)  // square grid +newline
#define DIRSIZE 4

typedef struct vec {
    int x, y;
} Vec;

typedef enum dir {
    UP, RIGHT, DOWN, LEFT
} Dir;

// Same order as enum dir
static const Vec step[DIRSIZE] = {{0,-1}, {1,0}, {0,1}, {-1,0}};

// Grid, extra newline per row, size in bytes = FSIZE
static char map[N][N + 1];

static bool ismap(const Vec pos)
{
    return pos.x >= 0 && pos.x < N && pos.y >= 0 && pos.y < N;
}

static char peek(const Vec pos)
{
    return map[pos.y][pos.x];
}

static void poke(const Vec pos, const char c)
{
    map[pos.y][pos.x] = c;
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

static bool findstart(Vec *const pos)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (map[i][j] == '^') {
                *pos = (Vec){j, i};
                return true;
            }
    return false;
}

// Turn right
static void turn(Dir *dir)
{
    *dir = (*dir + 1) & DIRSIZE;

}

// Rotate in positive direction with inverted y-axis = turn right
// up(0,-1) -> rt(1,0) -> dn(0,1) -> lf(-1,0) -> up(0,-1)
// static void turn(Vec *const a)
// {
//     *a = (Vec){-a->y, a->x};
// }

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Read input file
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(map, FSIZE, 1, f);
    fclose(f);

    // Find starting position
    Vec pos = {0};
    if (!findstart(&pos)) { fputs("Starting position not found.\n", stderr); return 2; }

    // Part 1
    Dir dir = UP;  // start heading up
    Vec next;
    next = add(pos, step[dir]);
    // for (int i = 0; i < 3 && peek(next) == '#'; ++i) {
    //     turn(&dir);
    //     next = add(pos, dir);
    // }

    int visited = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            visited += map[i][j] == 'X';
    printf("Part 1: %d\n", visited);  // example: 41, input: ?

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
