/**
 * Advent of Code 2024
 * Day 12: Garden Groups
 * https://adventofcode.com/2024/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c99 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  154 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  261 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  422 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  521 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 1026 µs
 */

#include <stdio.h>
#include <stdint.h>   // uint8_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-12-example.txt"
    #define N 10
#else
    #define FNAME "../aocinput/2024-12-input.txt"
    #define N 140
#endif
#define SSIZE 128  // max needed for my input: 116

// Orthogonal directions
#define TOP (1u << 0)  // bit 0
#define BTM (1u << 1)  // bit 1
#define LFT (1u << 2)  // bit 2
#define RGT (1u << 3)  // bit 3

// Diagonal directions
#define TL (TOP | LFT)
#define BL (BTM | LFT)
#define TR (TOP | RGT)
#define BR (BTM | RGT)

typedef struct vec {
    int x, y;
} Vec;

typedef struct stack {
    size_t len;
    Vec mem[SSIZE];
} Stack;

static char map[N + 2][N + 3];    // input file +border
static bool seen[N + 1][N + 1];   // explored nodes
static uint8_t nb[N + 1][N + 1];  // neighbours (bits 0-3 set/not set)
static Stack stack;

// Add vectors in place ("by reference")
static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

// Save onto stack, return false if already seen or stack full
static bool push(const int x, const int y)
{
    if (seen[x][y] || stack.len == SSIZE)
        return false;
    stack.mem[stack.len++] = (Vec){x, y};
    return (seen[x][y] = true);
}

// Remove from stack, set row/col, return false if stack was empty
static bool pop(int *const restrict x, int *const restrict y)
{
    if (!stack.len)
        return false;
    const Vec val = stack.mem[--stack.len];
    *x = val.x;
    *y = val.y;
    return true;
}

// Explore plot with same plants starting from i,j
// Return plot price for parts 1 and 2 together in one Vec
static Vec price(int i, int j)
{
    int area = 0, perim = 0, corners = 0;  // number of corners is same as sides
    seen[i][j] = true;
    const char plant = map[i][j];
    do {
        ++area;  // every plant only counted once
        // Part 1, and neighbours inventory for part 2
        if (map[i - 1][j] == plant) { nb[i][j] |= TOP; push(i - 1, j); } else ++perim;
        if (map[i + 1][j] == plant) { nb[i][j] |= BTM; push(i + 1, j); } else ++perim;
        if (map[i][j - 1] == plant) { nb[i][j] |= LFT; push(i, j - 1); } else ++perim;
        if (map[i][j + 1] == plant) { nb[i][j] |= RGT; push(i, j + 1); } else ++perim;
        // Part 2
        switch (nb[i][j] & TL) {
            case 0: ++corners; break;
            case TL: corners += map[i - 1][j - 1] != plant; break;
        }
        switch (nb[i][j] & BL) {
            case 0: ++corners; break;
            case BL: corners += map[i + 1][j - 1] != plant; break;
        }
        switch (nb[i][j] & TR) {
            case 0: ++corners; break;
            case TR: corners += map[i - 1][j + 1] != plant; break;
        }
        switch (nb[i][j] & BR) {
            case 0: ++corners; break;
            case BR: corners += map[i + 1][j + 1] != plant; break;
        }
    } while (pop(&i, &j));
#if EXAMPLE
    printf("%c: %2d x [%2d,%2d] = [%3d,%3d]\n", plant, area, perim, corners, area * perim, area * corners);
#endif
    return (Vec){area * perim, area * corners};
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");  // binary mode needed for fread()
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 1; i <= N; ++i)
        fgets(&map[i][1], N + 2, f);  // +\n\0
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    Vec sum = {0};
    for (int i = 1; i <= N; ++i)      // row value with borders
        for (int j = 1; j <= N; ++j)  // col value with borders
            if (!seen[i][j])          // new plot?
                add_r(&sum, price(i, j));
    printf("%d %d\n", sum.x, sum.y);  // example: 1930 1206, input: 1361494 830516

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
