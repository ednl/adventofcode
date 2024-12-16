/**
 * Advent of Code 2024
 * Day 16: Reindeer Maze
 * https://adventofcode.com/2024/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 16.c
 *    gcc   -std=gnu17 -Wall -Wextra 16.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 16.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 16.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : ? µs
 */

#include <stdio.h>

#define EXAMPLE 1
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-16-example1.txt"
    #define N 15
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2024-16-example2.txt"
    #define N 17
#else
    #define FNAME "../aocinput/2024-16-input.txt"
    #define N 141
#endif

typedef struct pos {
    int x, y;
} Pos;

// Positive direction for y going down: right->down->left->up->right
typedef enum dir {
    RIGHT, DOWN, LEFT, UP
} Dir;

typedef struct state {
    Pos pos;
    Dir dir;
} State;

// Same order as 'enum dir'
static const Pos step[] = {{1,0},{0,1},{-1,0},{0,-1}};
static char map[N][N + 1];

static inline Dir turnleft(const Dir dir)
{
    return (dir + 3) & 3;
}

static inline Dir turnright(const Dir dir)
{
    return (dir + 1) & 3;
}

static inline Pos add(const Pos a, const Pos b)
{
    return (Pos){a.x + b.x, a.y + b.y};
}

static inline void add_r(Pos *const a, const Pos b)
{
    a->x += b.x;
    a->y += b.y;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(map, sizeof map, 1, f);
    fclose(f);

    return 0;
}
