/**
 * Advent of Code 2023
 * Day 16: The Floor Will Be Lava
 * https://adventofcode.com/2023/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wno-multichar 16.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wno-multichar 16.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<500;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)                  :  12 ms
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               :  24 ms
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) :  27 ms
 *     Raspberry Pi 5 (2.4 GHz)                       :  33 ms
 *     Raspberry Pi 4 (1.8 GHz)                       :  70 ms
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <string.h>   // memset
#include <stdint.h>   // int32_t
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-16-example.txt"
#define N 10
#define STACKSIZE 8  // max 4 for the example
#else
#define NAME "../aocinput/2023-16-input.txt"
#define N 110
#define STACKSIZE 80  // max 73 for my input
#endif

typedef struct vec {
    int x, y;
} Vec;

typedef struct beam {
    Vec pos;
    char dir;
} Beam;

static char mirror[N][N + 2];
static bool cache[N][N][4];  // last index: NESW=[0..4)
static Beam stack[STACKSIZE];
static int stacklen;

static char getmirror(const Vec pos)
{
    return mirror[pos.y][pos.x];
}

static void show(void)
{
#if EXAMPLE || defined(DEBUG)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j)
            fputc(*(int32_t*)cache[i][j] ? '#' : '.', stdout);  // misuse 4 bools as 1 int
        printf(" %s", mirror[i]);  // includes '\n'
    }
#endif
}

static bool inside(const Vec pos)
{
    return pos.x >= 0 && pos.y >= 0 && pos.x < N && pos.y < N;
}

static int dirindex(const char dir)
{
    switch (dir) {
        case 'N': return 0;
        case 'E': return 1;
        case 'S': return 2;
        case 'W': return 3;
    }
    return -1;
}

static bool isexplored(const Beam beam)
{
    return cache[beam.pos.y][beam.pos.x][dirindex(beam.dir)];
}

static void markexplored(const Beam beam)
{
    cache[beam.pos.y][beam.pos.x][dirindex(beam.dir)] = true;
}

static bool push(const Beam beam)
{
    if (isexplored(beam))
        return false;
    markexplored(beam);
    if (stacklen == STACKSIZE) {
        printf("!E");  // stack too small
        return false;
    }
    stack[stacklen++] = beam;
    return true;
}

static bool pop(Beam *beam)
{
    if (!stacklen)
        return false;
    *beam = stack[--stacklen];
    return true;
}

static Vec nextpos(const Beam beam)
{
    switch (beam.dir) {
        case 'N': return (Vec){beam.pos.x    , beam.pos.y - 1};
        case 'E': return (Vec){beam.pos.x + 1, beam.pos.y    };
        case 'S': return (Vec){beam.pos.x    , beam.pos.y + 1};
        case 'W': return (Vec){beam.pos.x - 1, beam.pos.y    };
    }
    return beam.pos;
}

static int energize(Beam beam)
{
    // Reset
    memset(cache, 0, sizeof cache);
    stacklen = 0;
    // DFS
    stack[stacklen++] = beam;  // root has out of range indices: do not check or markexplored()
    while (pop(&beam)) {
        const Vec next = nextpos(beam);
        if (inside(next)) {
            switch (beam.dir << 8 | getmirror(next)) {
                case 'N.':
                case 'N|':
                case 'E.':
                case 'E-':
                case 'S.':
                case 'S|':
                case 'W.':
                case 'W-':
                    push((Beam){next, beam.dir}); break;  // keep going
                case 'N/':
                case 'S\\':
                    push((Beam){next, 'E'}); break;
                case 'E/':
                case 'W\\':
                    push((Beam){next, 'N'}); break;
                case 'S/':
                case 'N\\':
                    push((Beam){next, 'W'}); break;
                case 'W/':
                case 'E\\':
                    push((Beam){next, 'S'}); break;
                case 'N-':
                case 'S-':
                    push((Beam){next, 'E'});
                    push((Beam){next, 'W'}); break;
                case 'E|':
                case 'W|':
                    push((Beam){next, 'N'});
                    push((Beam){next, 'S'}); break;
            }
        }
    }
    // How many tiles are energized?
    int sum = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            sum += (*(int32_t*)cache[i][j] != 0);
    return sum;
}

int main(void)
{
    starttimer();
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    for (int i = 0; i < N; ++i)
        fgets(mirror[i], sizeof *mirror, f);
    fclose(f);

    const int part1 = energize((Beam){{-1, 0}, 'E'});
    show();
    printf("Part 1: %d\n", part1);  // example: 46, input: 7979

    int part2 = part1;
    for (int i = 1, e; i < N; ++i) {
        e = energize((Beam){{-1,  i}, 'E'});
        if (e > part2) part2 = e;
    }
    for (int i = 0, e; i < N; ++i) {
        e = energize((Beam){{ i,  N}, 'N'});
        if (e > part2) part2 = e;
        e = energize((Beam){{ i, -1}, 'S'});
        if (e > part2) part2 = e;
        e = energize((Beam){{ N,  i}, 'W'});
        if (e > part2) part2 = e;
    }
    printf("Part 2: %d\n", part2);  // example: 51, input: 8437
    printf("Time: %.0f ms\n", stoptimer_ms());
    return 0;
}
