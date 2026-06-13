/**
 * Advent of Code 2020
 * Day 12: Rain Risk
 * https://adventofcode.com/2020/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.35 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#include <stdint.h>  // uint8_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-12-input.txt"
#define FSIZE 3200  // needed for my input: 2833
#define LEN   800   // needed for my input: 796

typedef struct nav {
    int cmd;
    int val;
} Nav;

typedef struct vec {
    int x, y;
} Vec;

typedef enum compass {
    N, S, E, W
} Comnpass;

static const Vec dir[] = {
    [N] = { 0,  1},
    [S] = { 0, -1},
    [E] = { 1,  0},
    [W] = {-1,  0},
};

static char input[FSIZE];
static Nav nav[LEN];

static int manh(const Vec v)
{
    return abs(v.x) + abs(v.y);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int n = 0;
    for (const char *c = input; c != end; ++n, ++c) {
        nav[n].cmd = *c++;
        int x = *c++ & 15;
        while (*c != '\n')
            x = x * 10 + (*c++ & 15);
        nav[n].val = x;
        // switch (x) {
        //     case 180: nav[n].cmd = 'T'; break;  // turn around
        //     case 270: nav[n].cmd ^= 30; break;  // L<->R
        // }
    }

    Vec ship1 = {0}, ship2 = {0};
    Vec head = dir[E];
    Vec wayp = {10, 1};
    for (int i = 0; i < n; ++i)
        switch (nav[i].cmd) {
            case 'F':
                ship1.x += head.x * nav[i].val; ship1.y += head.y * nav[i].val;
                ship2.x += wayp.x * nav[i].val; ship2.y += wayp.y * nav[i].val;
                break;
            case 'N': ship1.y += nav[i].val; wayp.y += nav[i].val; break;
            case 'S': ship1.y -= nav[i].val; wayp.y -= nav[i].val; break;
            case 'E': ship1.x += nav[i].val; wayp.x += nav[i].val; break;
            case 'W': ship1.x -= nav[i].val; wayp.x -= nav[i].val; break;
            case 'L':
                switch (nav[i].val) {
                    case  90: head = (Vec){-head.y,  head.x}; wayp = (Vec){-wayp.y,  wayp.x}; break;
                    case 180: head = (Vec){-head.x, -head.y}; wayp = (Vec){-wayp.x, -wayp.y}; break;
                    case 270: head = (Vec){ head.y, -head.x}; wayp = (Vec){ wayp.y, -wayp.x}; break;
                }
                // head = (Vec){-head.y, head.x}; wayp = (Vec){-wayp.y, wayp.x};
                break;
            case 'R':
                switch (nav[i].val) {
                    case  90: head = (Vec){ head.y, -head.x}; wayp = (Vec){ wayp.y, -wayp.x}; break;
                    case 180: head = (Vec){-head.x, -head.y}; wayp = (Vec){-wayp.x, -wayp.y}; break;
                    case 270: head = (Vec){-head.y,  head.x}; wayp = (Vec){-wayp.y,  wayp.x}; break;
                }
                // head = (Vec){head.y, -head.x}; wayp = (Vec){wayp.y, -wayp.x};
                break;
            // case 'T': head = (Vec){-head.x, -head.y}; wayp = (Vec){-wayp.x, -wayp.y}; break;
        }
    printf("%d %d\n", manh(ship1), manh(ship2));  // 1032 156735

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
