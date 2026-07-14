/**
 * Advent of Code 2021
 * Day 13: Transparent Origami
 * https://adventofcode.com/2021/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 13.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 13.c
 * Test output with timer enabled:
 *     ./a.out | tail -n7
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :    ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 69.5 µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-13-input.txt"
#define FSIZE 8000  // needed for my input: 7125
#define ND 866
#define NF  12

static char input[FSIZE];
static int16_t dot[ND][2] = {0};
static int16_t fold[NF][2] = {0};

static int16_t parseint(const char **s)
{
    int16_t x = *(*s)++ & 15;
    while (**s >= '0')
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;
    return x;
}

static void parse(void)
{
    const char *c = input;
    for (int i = 0; i < ND; ++i) {
        dot[i][0] = parseint(&c);
        dot[i][1] = parseint(&c);
    }
    c++;  // skip blank line
    for (int i = 0; i < NF; ++i) {
        c += 11;  // skip "fold along "
        fold[i][0] = *c - 'x';  // 'x'=0, 'y'=1
        c += 2;
        fold[i][1] = parseint(&c);
    }
}

// Sort by y,x
static int yx_asc(const void *p, const void *q)
{
    const int16_t *a = p;
    const int16_t *b = q;
    if (a[1] < b[1]) return -1;
    if (a[1] > b[1]) return  1;
    if (a[0] < b[0]) return -1;
    if (a[0] > b[0]) return  1;
    return 0;
}

static void dofold(int i)
{
    const int axis = fold[i][0];
    const int val  = fold[i][1];
    for (int j = 0, dist; j < ND; ++j)
        if ((dist = dot[j][axis] - val) > 0)
            dot[j][axis] -= dist * 2;
}

static int visible(void)
{
    qsort(dot, ND, sizeof(int16_t[2]), yx_asc);
    int n = 1;
    for (int i = 1; i < ND; ++i)
         n += dot[i][0] != dot[i - 1][0] || dot[i][1] != dot[i - 1][1];
    return n;
}

static void show(void)
{
    qsort(dot, ND, sizeof(int16_t[2]), yx_asc);
    for (int i = 0, x = 0, y = dot[0][1]; i < ND; ) {
        for (; x < dot[i][0]; ++x)
            printf(" ");
        while (i < ND && x == dot[i][0] && y == dot[i][1])
            ++i;
        printf("#");
        ++x;
        if (i < ND && y != dot[i][1]) {
            printf("\n");
            x = 0;
            y = dot[i][1];
        }
    }
    printf("\n");
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    parse();
    dofold(0);
    printf("%d\n", visible());  // 708
    for (int i = 1; i < NF; ++i)
        dofold(i);
    show();  // EBLUBRFH

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
