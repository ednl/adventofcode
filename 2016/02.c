/**
 * Advent of Code 2016
 * Day 2: Bathroom Security
 * https://adventofcode.com/2016/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output which is redirected to /dev/null in shell:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  3.61 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 11.9  µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-02-input.txt"
#define FSIZE 2481

static char input[FSIZE];
static const char *const end = input + FSIZE;

typedef struct vec2 {
    int x, y;
} Vec2;

// Must be same order as Vec2 step[]
typedef enum dir {
    UP = 0, DOWN, LEFT, RIGHT
} Dir;

// Must be same order as enum dir
static const Vec2 step[] = {{0,-1},{0,1},{-1,0},{1,0}};  // UDLR (y=-y)

// Keypads for part 1 and 2 from puzzle description
// with zero-padding on all sides
static const int pad[2][7][7] = {{
    {0},
    {0},
    {0, 0, 1, 2, 3},
    {0, 0, 4, 5, 6},
    {0, 0, 7, 8, 9},
    {0},
    {0}},{
    {0},
    {0, 0, 0, 1},
    {0, 0, 2, 3, 4},
    {0, 5, 6, 7, 8, 9},
    {0, 0,10,11,12},
    {0, 0, 0,13},
    {0}}};

static Vec2 pos[2];

static Vec2 add(const Vec2 a, const Vec2 b)
{
    return (Vec2){a.x + b.x, a.y + b.y};
}

static void move(const Dir dir)
{
    for (int i = 0; i < 2; ++i) {
        const Vec2 tmp = add(pos[i], step[dir]);
        if (pad[i][tmp.y][tmp.x])
            pos[i] = tmp;
    }
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, sizeof input, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
    for (int timingloop = 0; timingloop < 1000; ++timingloop) {
#endif

    Vec2 code = {0};
    // Initial positions at key "5"
    pos[0] = (Vec2){3, 3};
    pos[1] = (Vec2){1, 3};
    for (const char *c = input; c != end; ++c) {
        switch (*c) {
            case 'U': move(UP   ); break;
            case 'D': move(DOWN ); break;
            case 'L': move(LEFT ); break;
            case 'R': move(RIGHT); break;
            case '\n':  // newline: record current keypad position
                code.x = code.x * 10 + pad[0][pos[0].y][pos[0].x];
                code.y = code.y << 4 | pad[1][pos[1].y][pos[1].x];
                break;
        }
    }
    printf("%d %X\n", code.x, code.y);  // 95549 D87AD

#ifdef TIMER
    }
    fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
