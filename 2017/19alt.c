/**
 * Advent of Code 2017
 * Day 19: A Series of Tubes
 * https://adventofcode.com/2017/day/19
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=gnu17 -O3 -march=native -mtune=native ../startstoptimer.c 19alt.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 14 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 28 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 69 µs
 */

#include <stdio.h>
#include "../startstoptimer.h"

#define FNAME "../aocinput/2017-19-input.txt"
#define DIM 201  // width and height of grid
#define LET 16   // needed for my input: 10 (+\0 = 11)

typedef enum dir {
    U, D, L, R
} Dir;

typedef struct vec2 {
    int x, y;
} Vec2;

static const Vec2 step[] = {[U]={0,-1}, [D]={0,1}, [L]={-1,0}, [R]={1,0}};
static char grid[DIM][DIM + 1];  // +\n
static char code[LET];  // letters collected along the way
static int codelen;
static int steps;

static char probe(const Vec2 pos, const int dir)
{
    return grid[pos.y + step[dir].y][pos.x + step[dir].x];
}

static char go(Vec2 *const pos, const int dir)
{
    steps++;
    return grid[(pos->y += step[dir].y)][(pos->x += step[dir].x)];
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    fread(grid, sizeof grid, 1, f);
    fclose(f);

    starttimer();

    Vec2 pos = {0};
    Dir dir = D;
    for (; grid[0][pos.x] == ' '; pos.x++);

    for (;;)
        switch (go(&pos, dir)) {
            case ' ': goto done;
            case '+': dir = dir & 2 ?            // currently U/D or L/R?
                (probe(pos, U) == ' ' ? D : U):  // was L/R, now U/D
                (probe(pos, L) == ' ' ? R : L);  // was U/D, now L/R
            case '-':
            case '|': break;
            default: code[codelen++] = grid[pos.y][pos.x];
        }
done:
    printf("%s %d\n", code, steps);  // DWNBGECOMY 17228
    printf("Time: %.0f us\n", stoptimer_us());
}
