/**
 * Advent of Code 2024
 * Day 12: Garden Groups
 * https://adventofcode.com/2024/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 12.c
 *    gcc   -std=gnu17 -Wall -Wextra 12.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 12.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 12.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  309 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  576 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :    ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 1392 µs
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
#define TOP (1 << 0)  // bit 0
#define BTM (1 << 1)  // bit 1
#define LFT (1 << 2)  // bit 2
#define RGT (1 << 3)  // bit 3

// Diagonal directions
#define T_L (TOP | LFT)
#define B_L (BTM | LFT)
#define T_R (TOP | RGT)
#define B_R (BTM | RGT)

typedef struct vec {
    int x, y;
} Vec;

typedef struct stack {
    size_t len;
    Vec mem[SSIZE];
} Stack;

static char map[N][N + 1];  // input file +newlines
static bool seen[N][N];     // explored nodes
static uint8_t nb[N][N];    // neighbours (bits 0-3 set/not set)
static Stack stack;

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

// Is this position part of the same plot? Also check out of bounds
// Possibly faster with added border, but that complicates reading input file
static inline bool same(const int i, const int j, const char plant)
{
    return i >= 0 && i < N && j >= 0 && j < N && map[i][j] == plant;
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
        if (same(i - 1, j, plant)) { nb[i][j] |= TOP; push(i - 1, j); } else ++perim;
        if (same(i + 1, j, plant)) { nb[i][j] |= BTM; push(i + 1, j); } else ++perim;
        if (same(i, j - 1, plant)) { nb[i][j] |= LFT; push(i, j - 1); } else ++perim;
        if (same(i, j + 1, plant)) { nb[i][j] |= RGT; push(i, j + 1); } else ++perim;
        // Part 2
        // Concave ("inside") corners are always formed by 3 plants, so they are counted 3x
        // => also count convex ("outside") corners 3x, and divide by 3 afterwards
        // "7JLF" are angle shapes of three plants with a concave corner
        switch (nb[i][j] & T_L) {                                    // top & left
            case   0: corners += 3; break;                           // convex (x3)
            case TOP:                                                // concave 7
            case LFT: corners +=  same(i - 1, j - 1, plant); break;  // concave L
            case T_L: corners += !same(i - 1, j - 1, plant); break;  // concave J
        }
        switch (nb[i][j] & B_L) {                                    // bottom & left
            case   0: corners += 3; break;                           // convex (x3)
            case BTM:                                                // concave J
            case LFT: corners +=  same(i + 1, j - 1, plant); break;  // concave F
            case B_L: corners += !same(i + 1, j - 1, plant); break;  // concave 7
        }
        switch (nb[i][j] & T_R) {                                    // top & right
            case   0: corners += 3; break;                           // convex (x3)
            case TOP:                                                // concave F
            case RGT: corners +=  same(i - 1, j + 1, plant); break;  // concave J
            case T_R: corners += !same(i - 1, j + 1, plant); break;  // concave L
        }
        switch (nb[i][j] & B_R) {                                    // bottom & right
            case   0: corners += 3; break;                           // convex (x3)
            case BTM:                                                // concave L
            case RGT: corners +=  same(i + 1, j + 1, plant); break;  // concave 7
            case B_R: corners += !same(i + 1, j + 1, plant); break;  // concave F
        }
    } while (pop(&i, &j));
    corners /= 3;  // correction for counting corners 3x
#if EXAMPLE
    printf("%c: %2d x [%2d,%2d] = [%3d,%3d]\n", plant, area, perim, corners, area * perim, area * corners);
#endif
    return (Vec){area * perim, area * corners};
}

static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread()
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(map, sizeof map, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    Vec sum = {0};
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (!seen[i][j])  // new plot?
                add_r(&sum, price(i, j));
    printf("%d %d\n", sum.x, sum.y);  // example: 1930 1206, input: 1361494 830516

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
