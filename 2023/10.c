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
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-multichar 10.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c -Wno-multichar 10.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 18.3 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 99.0 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-10-input.txt"
#define W 140
#define H 140

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
static const int pdiff[] = {0, -(W + 1), W + 1, -1, 1};
static char pipe[H][W + 1];  // +newline

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
    for (int y = H - 1; y >= 0; --y)
        for (int x = W - 1; x >= 0; --x)
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
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(pipe, sizeof pipe, 1, f);  // read as one block
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

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
    printf("%d %d\n", border, inside);  // 7005 417

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
