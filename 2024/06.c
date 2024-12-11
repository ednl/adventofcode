/**
 * Advent of Code 2024
 * Day 6: Guard Gallivant
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
#include <string.h>  // memset
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-06-example.txt"
    #define N 10  // rows and cols of example grid
#else
    #define FNAME "../aocinput/2024-06-input.txt"
    #define N 130  // rows and cols of input grid
#endif
#define FSIZE N * (N + 1)  // square grid +newline
#define WALL '#'
#define FREE '.'

typedef enum dir {
    UP, RIGHT, DOWN, LEFT
} Dir;

typedef struct pos {
    int x, y;
} Pos;

typedef struct state {
    Pos pos;
    Dir dir;
} State;

// Same order as enum dir
// static const char *head = "^>v<";
static const Pos step[] = {{0,-1}, {1,0}, {0,1}, {-1,0}};

// Grid, extra newline per row, size in bytes = FSIZE
static char map[N][N + 1];

// bit 0-3 set = been here going u/d/l/r
static char hist[N][N];

static State path[N * N];
static int pathlen;

// Look for '^', set pos, return false if not found
static bool findstart(Pos *const pos)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (map[i][j] == '^') {
                *pos = (Pos){j, i};
                return true;
            }
    return false;
}

// Still on the map?
static bool ismap(const Pos pos)
{
    return pos.x >= 0 && pos.x < N && pos.y >= 0 && pos.y < N;
}

// Look at the map
static char look(const Pos pos)
{
    return map[pos.y][pos.x];
}

// Mark the map
static void mark(const Pos pos, const char c)
{
    map[pos.y][pos.x] = c;
}

// Take one step in direction, return new position
static Pos go(const Pos pos, const Dir dir)
{
    return (Pos){pos.x + step[dir].x, pos.y + step[dir].y};
}

// Turn right, return new direction
static Dir turn(const Dir dir)
{
    return (dir + 1) & 3;
}

static void sethist(const Pos pos, const Dir dir)
{
    hist[pos.y][pos.x] |= (1 << dir);
}

static bool isdupe(const Pos pos, const Dir dir)
{
     return hist[pos.y][pos.x] & (1 << dir);
}

// Part 1: add position to path, save direction on grid
// Return true (+1) if never visited before
static bool save(const Pos pos, const Dir dir)
{
    const bool isnew = !hist[pos.y][pos.x];
    path[pathlen++] = (State){pos, dir};
    sethist(pos, dir);
    return isnew;
}

static int walk(Pos pos, Dir dir)
{
    int visited = 0;
    Pos next;
    while (ismap((next = go(pos, dir))))
        if (look(next) != WALL) {  // can we go ahead?
            visited += save(pos, dir);
            pos = next;  // one step ahead
        } else
            dir = turn(dir);  // wall in front: turn right
    visited += save(pos, dir);  // last tile before going off the map
    return visited;
}

// Try obstruction at path index [1..pathlen)
static bool hasloop(const int index)
{
    // Reset all history
    memset(hist, 0, sizeof hist);
    // Starting position and direction
    Pos pos = path[index - 1].pos, next;
    Dir dir = path[index - 1].dir;
    // Walk until loop found or fallen off map
    while (ismap((next = go(pos, dir))))
        if (look(next) != WALL) {
            if (isdupe(pos, dir))
                return true;
            sethist(pos, dir);
            pos = next;
        } else
            dir = turn(dir);  // wall in front: turn right
    return false;
}

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
    Pos pos = {0};
    Dir dir = UP;  // start in up direction, next is right
    if (!findstart(&pos)) { fputs("Starting position not found.\n", stderr); return 2; }

    // Walk in circles until we fall off the map
    printf("Part 1: %d\n", walk(pos, dir));  // example: 41, input: 5331

    int loops = 0;
    for (int i = 1; i < pathlen; ++i) {
        mark(path[i].pos, WALL);  // try obstruction in the path here
        loops += hasloop(i);
        mark(path[i].pos, FREE);  // reset obstruction
    }
    printf("Part 2: %d\n", loops);  // example: 6, input: (520 too low, 2094 too high)

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
