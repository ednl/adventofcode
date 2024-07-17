/**
 * Advent of Code 2023
 * Day 10: Pipe Maze
 * https://adventofcode.com/2023/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Theory:
 *     https://en.wikipedia.org/wiki/Shoelace_formula
 *     https://en.wikipedia.org/wiki/Pick%27s_theorem
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra -Wno-multichar 10.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra -Wno-multichar 10.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  90 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 148 µs
 *     Raspberry Pi 5 (2.4 GHz)            : 150 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 361 µs
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include "../startstoptimer.h"

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
#define W2 (W + 2)  // +2 for '\n\0'

typedef struct vec {
    int x, y;
} Vec;
typedef enum dir {
    NONE, UP, DOWN, LEFT, RIGHT
} Dir;
typedef struct state {
    Vec pos;
    Dir dir;
} State;

static const Vec delta[] = {{0,0},{0,-1},{0,1},{-1,0},{1,0}};
static const int pdiff[] = {0, -W2, W2, -1, 1};
static char pipe[H][W2];

static void add_r(Vec *a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static int absi(const int a)
{
    return a < 0 ? -a : a;
}

static int manh(const Vec a, const Vec b)
{
    return absi(a.x - b.x) + absi(a.y - b.y);
}

// https://en.wikipedia.org/wiki/Shoelace_formula
static int shoelace(const Vec a, const Vec b)
{
    return (a.y + b.y) * (a.x - b.x);
}

// Find start point 'S'
static State start(void)
{
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            if (pipe[y][x] == 'S') {
                if (y > 0) {
                    const char p = pipe[y - 1][x];
                    if (p == '|' || p == '7' || p == 'F') return (State){{x, y}, UP};
                }
                if (y < H - 1) {
                    const char p = pipe[y + 1][x];
                    if (p == '|' || p == 'J' || p == 'L') return (State){{x, y}, DOWN};
                }
                if (x > 0) {
                    const char p = pipe[y][x - 1];
                    if (p == '-' || p == 'L' || p == 'F') return (State){{x, y}, LEFT};
                }
                if (x < W - 1) {
                    const char p = pipe[y][x + 1];
                    if (p == '-' || p == 'J' || p == '7') return (State){{x, y}, RIGHT};
                }
                return (State){{x, y}, NONE};
            }
    return (State){{0, 0}, NONE};
}

int main(void)
{
    starttimer();
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    for (int i = 0; i < H; ++i)
        fgets(pipe[i], sizeof *pipe, f);
    fclose(f);

    State s = start();
    const char *p = &pipe[s.pos.y][s.pos.x];
    int area = 0, border = 0;
    do {
        const Vec prev = s.pos;
        do {
            add_r(&s.pos, delta[s.dir]);
            p += pdiff[s.dir];
        } while (*p == '-' || *p == '|');
        switch ("_UDLR"[s.dir] << 8 | *p) {
            case 'LL':
            case 'RJ': s.dir = UP; break;
            case 'LF':
            case 'R7': s.dir = DOWN; break;
            case 'U7':
            case 'DJ': s.dir = LEFT; break;
            case 'UF':
            case 'DL': s.dir = RIGHT; break;
        }
        area += shoelace(prev, s.pos);
        border += manh(prev, s.pos);
    } while (*p != 'S');

    border /= 2;  // half border = from 'S' to farthest point
    // Shoelace formula: A = 1/2 . sum((y_i + y_i+1) . (x_i - x_i+1))
    // Pick's theorem: i = A - b/2 + 1
    const int inside = absi(area) / 2 - border + 1;

    printf("Part 1: %d\n", border);  // 7005
    printf("Part 2: %d\n", inside);  // 417
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
