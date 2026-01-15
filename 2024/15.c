/**
 * Advent of Code 2024
 * Day 15: Warehouse Woes
 * https://adventofcode.com/2024/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 15.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 15.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : ? µs
 */

#include <stdio.h>
#include <stdbool.h>

#define EXAMPLE 2
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-15-example1.txt"
    #define N 8
    #define L 1
    #define M 15
#elif EXAMPLE == 2
    #define FNAME "../aocinput/2024-15-example2.txt"
    #define N 10
    #define L 10
    #define M 70
#else
    #define FNAME "../aocinput/2024-15-input.txt"
    #define N 50
    #define L 20
    #define M 1000
#endif
#define MOVES (L * (M + 1))
#define FLOOR '.'
#define WALL  '#'
#define BOX   'O'
#define ROBOT '@'

typedef struct pos {
    int x, y;
} Pos;

typedef enum dir {
    RIGHT, DOWN, LEFT, UP
} Dir;

// Same order as 'enum dir'
static const Pos step[] = {{1,0},{0,1},{-1,0},{0,-1}};

static char map[N][N + 1];
static char map2[N][N * 2 + 1];
static char move[MOVES];
static Pos robot;

static void show(const char *a, const size_t size)
{
    for (size_t i = 0; i < size; ++i)
        putchar(*a++);
}

static Pos add(const Pos a, const Pos b)
{
    return (Pos){a.x + b.x, a.y + b.y};
}

static void add_r(Pos *const a, const Pos b)
{
    a->x += b.x;
    a->y += b.y;
}

static char peek(const Pos pos)
{
    return map[pos.y][pos.x];
}

static void poke(const Pos pos, const char tile)
{
    map[pos.y][pos.x] = tile;
}

static Pos findrobot(void)
{
    for (int i = 1; i < N - 1; ++i)  // skip border
        for (int j = 1; j < N - 1; ++j)
            if (map[i][j] == ROBOT)
                return (Pos){j, i};
    return (Pos){0};
}

static bool shift(const Pos pos, const Dir dir)
{
    const Pos next = add(pos, step[dir]);
    if (peek(next) == WALL)
        return false;
    if (peek(next) == FLOOR || shift(next, dir)) {
        poke(next, peek(pos));
        poke(pos, FLOOR);
        return true;
    }
    return false;
}

static int gps(void)
{
    int sum = 0;
    for (int i = 1; i < N - 1; ++i)  // skip border
        for (int j = 1; j < N - 1; ++j)
            if (map[i][j] == BOX)
                sum += i * 100 + j;
    return sum;
}

static int gps2(void)
{
    int sum = 0;
    for (int i = 1; i < N - 1; ++i)  // skip border
        for (int j = 2; j < N * 2 - 2; ++j)
            if (map2[i][j] == '[')
                sum += i * 100 + j;
    return sum;
}

int main(void)
{
    // Read file
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(map, sizeof map, 1, f);
    fgetc(f);  // empty line
    fread(move, sizeof move, 1, f);
    fclose(f);

    // Part 1
    robot = findrobot();
    for (int i = 0; i < MOVES; ++i)
        switch (move[i]) {
            case '>': if (shift(robot, RIGHT)) add_r(&robot, step[RIGHT]); break;
            case 'v': if (shift(robot, DOWN )) add_r(&robot, step[DOWN ]); break;
            case '<': if (shift(robot, LEFT )) add_r(&robot, step[LEFT ]); break;
            case '^': if (shift(robot, UP   )) add_r(&robot, step[UP   ]); break;
        }
    show(&map[0][0], sizeof map);
    printf("Part 1: %d\n", gps());  // example 1: 2028, example 2: 10092, input: 1497888

    // Part 2
    // Read original map
    f = fopen(FNAME, "rb");
    fread(map, sizeof map, 1, f);
    fclose(f);
    // Scale up horizontally
    for (int i = 0; i < N; ++i)
        for (int j = 0; j <= N; ++j)  // include newline
            switch (map[i][j]) {
                case '#' : map2[i][j * 2] = '#'; map2[i][j * 2 + 1] = '#'; break;
                case 'O' : map2[i][j * 2] = '['; map2[i][j * 2 + 1] = ']'; break;
                case '.' : map2[i][j * 2] = '.'; map2[i][j * 2 + 1] = '.'; break;
                case '@' : map2[i][j * 2] = '@'; map2[i][j * 2 + 1] = '.'; robot = (Pos){j * 2, i}; break;
                case '\n': map2[i][j * 2] = '\n'; break;
            }
    // TODO
    show(&map2[0][0], sizeof map2);
    printf("Part 2: %d\n", gps2());  // example 1: ?, example 2: 9021, input: ?
    return 0;
}
