/**
 * Advent of Code 2024
 * Day 14: Restroom Redoubt
 * https://adventofcode.com/2024/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra -O3 -march=native 14.c
 *    gcc   -std=gnu17 -Wall -Wextra -O3 -march=native 14.c
 */

#include <stdio.h>
#include <string.h>  // memset, memcpy
#include <stdint.h>
#include <stdbool.h>
#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-14-example.txt"
    #define N 12  // lines in example file
    #define X 11  // width
    #define Y  7  // height
#else
    #define FNAME "../aocinput/2024-14-input.txt"
    #define N 500  // lines in input file
    #define X 101  // width
    #define Y 103  // height
#endif
#define T 100  // part 1: 100 seconds
#define XQ (X >> 2)
#define YQ (Y >> 2)
#define XR (X & 3)
#define YR (Y & 3)

typedef struct vec {
    int x, y;
} Vec;

typedef union quad {
    int mat[2][2];
    int arr[4];
} Quad;

// quad-quad
typedef union qq {
    int mat[4][4];
    int arr[16];
} QQ;

static Vec p[N];  // position
static Vec v[N];  // velocity

// Non-negative remainder for positive m
static int mod(const int a, const int m)
{
    const int b = a % m;
    return b >= 0 ? b : b + m;
}

// Return variance of distribution over 4x4 blocks
static int step(void)
{
    QQ qq = {0};
    for (int i = 0; i < N; ++i) {
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        if (p[i].x < 0) p[i].x += X; else if (p[i].x >= X) p[i].x -= X;
        if (p[i].y < 0) p[i].y += Y; else if (p[i].y >= Y) p[i].y -= Y;
        if (p[i].x < X - XR && p[i].y < Y - YR)
            qq.mat[p[i].y / 25][p[i].x / 25]++;
    }
    int var = 8;
    for (int i = 0; i < 16; ++i) {
        const int d = qq.b[i] - 32;
        var += d * d;
    }
    return var >> 4;
}

static void show(void)
{
    char map[Y][X];
    memset(map, '.', sizeof map);
    for (int i = 0; i < N; ++i)
        map[p[i].y][p[i].x] = '#';
    for (int i = 0; i < Y; ++i) {
        for (int j = 0; j < X; ++j)
            putchar(map[i][j]);
        putchar('\n');
    }
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 0; i < N; ++i)
        fscanf(f, "p=%d,%d v=%d,%d ",
            &p0[i].x, &p0[i].y, &v[i].x, &v[i].y);
    fclose(f);

    int quad[2][2] = {0};
    for (int i = 0; i < N; ++i) {
        p[i].x = mod(p0[i].x + v[i].x * T, X);
        p[i].y = mod(p0[i].y + v[i].y * T, Y);
        if (p[i].x != (X >> 1) && p[i].y != (Y >> 1))
            ++quad[p[i].y / ((Y >> 1) + 1)][p[i].x / ((X >> 1) + 1)];
    }
    const int prod = quad[0][0] * quad[0][1] * quad[1][0] * quad[1][1];
    printf("%d\n", prod);  // example: 12, input: 221655456

    memcpy(p, p0, sizeof p);
    for (int i = 0; i < N; ++i)
        ++map[p[i].y][p[i].x];
    int sec = 0, max = 1000, var;
    while (1) {
        do {
            ++sec;
            var = step();
        } while (var <= max);
        printf("\nAter %d seconds: var=%d\n", sec, var);
        show();
        max = var;
    }

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    return 0;
}
