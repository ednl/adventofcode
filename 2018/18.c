/**
 * Advent of Code 2018
 * Day 18: Settlers of The North Pole
 * https://adventofcode.com/2018/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  9.05 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    : 13.7  ms
 *     Raspberry Pi 5 (2.4 GHz)      :  ?    ms
*/

#include <stdio.h>
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
#define INPUT "../aocinput/2018-18-example.txt"
#define N 10
#else
#define INPUT "../aocinput/2018-18-input.txt"
#define N 50
#endif
#define M (N + 2)
#define PART1 10
#define PART2 1000000000
#define OPEN 0
#define TREE 1
#define YARD 2

static uint8_t area[M][M], next[M][M];
typedef uint8_t Row[M];

static void parse(void)
{
    FILE *f = fopen(INPUT, "r");
    if (!f)
        return;
    int c, i = 1, j = 1;
    while ((c = fgetc(f)) != EOF)
        switch (c) {
            case '\n': i++; j = 1;          break;
            case '#' : area[i][j++] = YARD; break;
            case '.' : area[i][j++] = OPEN; break;
            case '|' : area[i][j++] = TREE; break;
        }
    fclose(f);
}

#if EXAMPLE
static void show(void)
{
    for (int i = 1; i <= N; ++i) {
        for (int j = 1; j <= N; ++j)
            putchar(".|#"[area[i][j]]);
        putchar('\n');
    }
    putchar('\n');
}
#endif

// Time steps
static int evolve(const int minutes)
{
    static Row *a = area, *b = next;
    for (int m = 0; m < minutes; ++m) {
        for (int i = 1; i <= N; ++i)
            for (int j = 1; j <= N; ++j) {
                int sum[3] = {0};
                for (int y = i - 1; y <= i + 1; ++y)
                    for (int x = j - 1; x <= j + 1; ++x)
                        sum[a[y][x]]++;  // also count centre cell, compensate in conditions below
                switch (a[i][j]) {
                    case OPEN: b[i][j] = sum[TREE] >= 3 ? TREE : OPEN; break;
                    case TREE: b[i][j] = sum[YARD] >= 3 ? YARD : TREE; break;
                    case YARD: b[i][j] = sum[YARD] >= 2 && sum[TREE] ? YARD : OPEN; break;
                }
            }
        Row *c = a; a = b; b = c;  // a,b = b,a
    }
    int sum[3] = {0};
    for (int i = 1; i <= N; ++i)
        for (int j = 1; j <= N; ++j)
            sum[a[i][j]]++;
    return sum[TREE] * sum[YARD];
}

int main(void)
{
    parse();
#if EXAMPLE
    show();
#endif
#ifdef TIMER
    starttimer();
#endif
    printf("Part 1: %d\n", evolve(PART1));  // 663502
#if EXAMPLE
    show();
#endif
    int stable = 1000;  // my input: 536 is start of repeating pattern, 1000 was a reasonable guess
    int period = 1, base = evolve(stable - PART1);
    while (evolve(1) != base)
        period++;  // my input: period = 28 (and incidentally, (1000000000 - 1000) % 28 = 0)
    printf("Part 2: %d\n", evolve((PART2 - stable) % period));  // 201341 (same as step 1000)
#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
