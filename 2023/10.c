/**
 * Advent of Code 2023
 * Day 10: Pipe Maze
 * https://adventofcode.com/2023/day/10
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <stdbool.h>  // bool
#include "../startstoptimer.h"  // compile: "clang -std=gnu17 -Ofast -march-native 10.c ../startstoptimer.c"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-10-example.txt"
#define W 20
#define H 10
#else
#define NAME "../aocinput/2023-10-input.txt"
#define W 140
#define H 140
#endif

typedef struct vec {
    int x, y;
} Vec;

static char field[H][W + 2];  // room for '\n\0'
static bool visited[H][W];

static bool equal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

// Get pipe at position pos
static char getpipe(const Vec pos)
{
    return field[pos.y][pos.x];
}

// Set pipe at position pos
static void setpipe(const Vec pos, const char pipe)
{
    field[pos.y][pos.x] = pipe;
}

static bool isloop(const Vec pos)
{
    return visited[pos.y][pos.x];
}

static void beenthere(const Vec pos)
{
    visited[pos.y][pos.x] = true;
}

// Find start ('S')
static Vec startpos(void)
{
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            if (getpipe((Vec){x, y}) == 'S')
                return (Vec){x, y};
    return (Vec){-1, -1};
}

// Return next position from current position and direction
static Vec nextpos(const Vec pos, const char dir)
{
    switch (dir) {
        case 'N': return (Vec){pos.x    , pos.y - 1};
        case 'E': return (Vec){pos.x + 1, pos.y    };
        case 'S': return (Vec){pos.x    , pos.y + 1};
        case 'W': return (Vec){pos.x - 1, pos.y    };
    }
    return pos;
}

// Set S to appropriate pipe bend
// Return: direction to start (NESW)
static char startdir(const Vec pos)
{
    if (pos.x < 0 || pos.y < 0 || pos.x >= W || pos.y >= H || getpipe(pos) != 'S')
        return '\0';
    int connections = 0;  // set bits for "can go from here to N/E/S/W" (N = LSB, W = MSB)
    if (pos.y > 0) {
        const char p = getpipe(nextpos(pos, 'N'));  // North
        if (p == '|' || p == '7' || p == 'F') connections |= (1 << 0);
    }
    if (pos.x < W - 1) {
        const char p = getpipe(nextpos(pos, 'E'));  // East
        if (p == '-' || p == 'J' || p == '7') connections |= (1 << 1);
    }
    if (pos.y < H - 1) {
        const char p = getpipe(nextpos(pos, 'S'));  // South
        if (p == '|' || p == 'J' || p == 'L') connections |= (1 << 2);
    }
    if (pos.x > 0) {
        const char p = getpipe(nextpos(pos, 'W'));  // West
        if (p == '-' || p == 'L' || p == 'F') connections |= (1 << 3);
    }
    switch (connections) {
        case  3: setpipe(pos, 'L'); return 'N';  // 0b0011 = NE -> L
        case  5: setpipe(pos, '|'); return 'N';  // 0b0101 = NS -> |
        case  9: setpipe(pos, 'J'); return 'N';  // 0b1001 = NW -> J
        case  6: setpipe(pos, 'F'); return 'E';  // 0b0110 = ES -> F
        case 10: setpipe(pos, '-'); return 'E';  // 0b1010 = EW -> -
        case 12: setpipe(pos, '7'); return 'S';  // 0b1100 = SW -> 7
    }
    return '\0';
}

static bool move(Vec* pos, char* dir)
{
    const Vec next = nextpos(*pos, *dir);
    switch (((*dir) << 8) | getpipe(next)) {
        case 'N|':
        case 'EJ':
        case 'WL': *pos = next; *dir = 'N'; return true;
        case 'E-':
        case 'SL':
        case 'NF': *pos = next; *dir = 'E'; return true;
        case 'E7':
        case 'S|':
        case 'WF': *pos = next; *dir = 'S'; return true;
        case 'N7':
        case 'SJ':
        case 'W-': *pos = next; *dir = 'W'; return true;
    }
    return false;
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < H; ++i)
        fgets(field[i], sizeof *field, f);
    fclose(f);

    const Vec start = startpos();  // [118,102]
    char dir = startdir(start);  // can go W,S -> pipe='7' -> dir=W or S

    Vec pos = start;
    int len = move(&pos, &dir);
    beenthere(pos);
    while (!equal(pos, start) && move(&pos, &dir)) {
        beenthere(pos);
        ++len;
    }
    printf("Part 1: %d\n", len / 2);  // 7005

    int count = 0;
    for (int y = 0; y < H; ++y) {
        bool inside = false;
        for (int x = 0; x < W; ++x) {
            const Vec p = (Vec){x, y};
            const bool loop = isloop(p);
            const char pipe = getpipe(p);
            if (loop && (pipe == '|' || pipe == 'F' || pipe == '7'))
                inside = !inside;
            if (inside && !loop)
                ++count;
        }
    }
    printf("Part 2: %d\n", count);  // 417
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
