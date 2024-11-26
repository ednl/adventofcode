/**
 * Advent of Code 2022
 * Day 22: Monkey Map
 * https://adventofcode.com/2022/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 22.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 22.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<1000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :   ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)            :   ? µs
 */

#include <stdio.h>
#include <string.h>  // memset

#define EXAMPLE 1
#if EXAMPLE == 1
    #define NAME "../aocinput/2022-22-example.txt"
    #define X 16
    #define Y 12
#else
    #define NAME "../aocinput/2022-22-input.txt"
    #define X 150
    #define Y 200
#endif

#define DIRSIZE 4
typedef enum dir {
    EAST = 0, SOUTH, WEST, NORTH
} Dir;

typedef struct pos {
    int x, y;
} Pos;

// East and south are positive, west and north negative.
// Order is like 'enum dir': east, south, west, north.
static const Pos step[DIRSIZE] = {{1,0},{0,1},{-1,0},{-1,0}};

static char map[Y + 2][X + 3];  // +spaces +\0

static char peek(const Pos pos)
{
    return map[pos.y][pos.x];
}

static void poke(const Pos pos, const char tile)
{
    map[pos.y][pos.x] = tile;
}

static Dir turn(const Dir facing, const char letter)
{
    switch (letter) {
        case 'R': return (facing + 1) % DIRSIZE;
        case 'L': return (facing - 1 + DIRSIZE) % DIRSIZE;
        default : return facing;
    }
}

// Add positions
static Pos add(const Pos p, const Pos q)
{
    return (Pos){p.x + q.x, p.y + q.y};
}

// Add positions by reference
static void add_r(Pos *const p, const Pos q)
{
    p->x += q.x;
    p->y += q.y;
}

static int move(Pos *const start, const Dir facing, const int amount)
{
    return amount;
}

int main(void)
{
    memset(map, ' ', sizeof map);
    for (int y = 0; y < Y + 2; ++y)
        poke((Pos){0, y}, '\0');

    FILE *f = fopen(NAME, "r");
    if (!f) return 1;
    for (int y = 1; y <= Y; ++y)
        fgets(&map[y][1], X + 2, f);
    fclose(f);

    for (int y = 1; y <= Y; ++y) {
        //
    }

    for (int y = 0; y < Y + 2; ++y)
        printf("%s", map[y]);

    Pos pos = {1, 1};
    while (map[pos.y][pos.x] != '.')
        pos.x++;

    int part1 = 0;
    printf("Part 1: %d\n", part1);  // example=1000*6+4*8+0=6032, input=?

    return 0;
}
