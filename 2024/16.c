/**
 * Advent of Code 2024
 * Day 16: Reindeer Maze
 * https://adventofcode.com/2024/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 16.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <stdbool.h>
#include <limits.h>  // INT_MAX

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

// Cost
#define STEP 1
#define TURN 1000

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
    int score;
} State;

// Same order as 'enum dir'
static const Pos step[] = {{1,0},{0,1},{-1,0},{0,-1}};
static char map[N][N + 1];
static int dist[N][N + 1];
static bool seen[N][N + 1];
static Pos end;

static int cmp(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static Dir turnleft(const Dir dir)
{
    return (dir + 3) & 3;
}

static Dir turnright(const Dir dir)
{
    return (dir + 1) & 3;
}

static bool eq(const Pos a, const Pos b)
{
    return a.x == b.x && a.y == b.y;
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

static int update(const Pos p, const int tentative)
{
    if (map[p.y][p.x] != '#' && !seen[p.y][p.x] && tentative < dist[p.y][p.x]) {
        dist[p.y][p.x] = tentative;
        return tentative;
    }
    return dist[p.y][p.x];
}

static int walk(const State s)
{
    if (eq(s.pos, end))
        return s.score;
    // Update unvisited neighbours if necessary
    int cost[3];
    int cost[0] = update(add(s.pos, step[s.dir]), s.score + STEP);
    int cost[1] = update(add(s.pos, step[turnleft(s.dir)]), s.score + TURN + STEP);
    int cost[2] = update(add(s.pos, step[turnright(s.dir)]), s.score + TURN + STEP);
    seen[s.pos.y][s.pos.x] = true;
    qsort(cost, 3, sizeof *cost, cmp);
    if (cost[0] == INT_MAX)
        return INT_MAX;
    // TODO
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(map, sizeof map, 1, f);
    fclose(f);

    Pos start = {0};
    for (int i = 1; i < N - 1; ++i)
        for (int j = 1; j < N - 1; ++j) {
            dist[i][j] = INT_MAX;
            switch (map[i][j]) {
                case 'E': end   = (Pos){j, i}; break;
                case 'S': start = (Pos){j, i}; dist[i][j] = 0; break;
            }
        }
    State cur = {start, RIGHT, 0};

    int score = 0;
    // TODO
    printf("Part1: %d\n", score);  // example 1: 7036, example 2: 11048, input: ?

    return 0;
}
