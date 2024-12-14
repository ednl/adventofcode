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
#include <string.h>  // memset
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

typedef struct vec {
    int x, y;
} Vec;

// Split map into 2x2=4 regions
typedef union quad {
    int a[4];     // access as array
    int m[2][2];  // access as matrix
} Quad;

// Split map into 3x3=9 regions
typedef union nine {
    int a[9];     // access as array
    int m[3][3];  // access as matrix
} Nine;

// Split map into 4x4=16 regions ("quad-quad")
// typedef union qq {
//     int a[16];    // access as array
//     int m[4][4];  // access as matrix
// } QQ;

static Vec p[N];  // position
static Vec v[N];  // velocity

static void show(void)
{
    char map[Y][X];
    memset(map, '.', sizeof map);  // background
    for (int i = 0; i < N; ++i)
        if (map[p[i].y][p[i].x] == '.')  // no robots?
            map[p[i].y][p[i].x] = '1';  // first robot
        else
            map[p[i].y][p[i].x]++;  // more robots
#if EXAMPLE
    for (int i = 0; i < Y; ++i) {
        for (int j = 0; j < X; ++j)
#else
    // Region of interest for my input
    for (int i = 44; i < 77; ++i) {
        for (int j = 35; j < 66; ++j)
#endif
            putchar(map[i][j]);
        putchar('\n');
    }
}

// Non-negative remainder (for positive m)
static int mod(const int a, const int m)
{
    const int b = a % m;
    return b >= 0 ? b : b + m;
}

// Integer variance of distribution over 3x3=9 regions.
// Experimental parameters: prevmax=2860, prv=37, fin=8048, fin/prv=217
// so the target configuration is the first one with var3 above 3000.
static int var3(void)
{
    Nine nine = {0};  // 9 regions
    for (int i = 0; i < N; ++i)
        nine.m[p[i].y / ((Y / 3) + 1)][p[i].x / ((X / 3) + 1)]++;  // divide by 35 (y) or 34 (x) to get region index
    int var = 0;
    for (int i = 0; i < 9; ++i) {
        const int d = nine.a[i] - (N / 9);  // difference from mean 55
        var += d * d;
    }
    return (var / 9);  // normalise for 9 regions
}

// Integer variance of distribution over 4x4=16 regions
// Experimental parameters: prevmax=969, prv=29, fin=2318, fin/prv=79
// so the target configuration is the first one with var4 above 1000.
// static int var4(void)
// {
//     QQ qq = {0};  // 16 regions
//     for (int i = 0; i < N; ++i)
//         qq.m[p[i].y / ((Y >> 2) + 1)][p[i].x / ((X >> 2) + 1)]++;  // divide by 26 to get region index
//     int var = 0;
//     for (int i = 0; i < 16; ++i) {
//         const int d = qq.a[i] - (N >> 4);  // difference from mean 31
//         var += d * d;
//     }
//     return (var >> 4);  // normalise for 16 regions
// }

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 0; i < N; ++i)
        fscanf(f, "p=%d,%d v=%d,%d ",
            &p[i].x, &p[i].y, &v[i].x, &v[i].y);
    fclose(f);

    // Part 1
    Quad quad = {0};
    for (int i = 0; i < N; ++i) {
        p[i].x = mod(p[i].x + v[i].x * T, X);  // 100 time steps in one
        p[i].y = mod(p[i].y + v[i].y * T, Y);
        if (p[i].x != (X >> 1) && p[i].y != (Y >> 1))
            quad.m[p[i].y / ((Y >> 1) + 1)][p[i].x / ((X >> 1) + 1)]++;
    }
    int prod = 1;
    for (int i = 0; i < 4; ++i)
        prod *= quad.a[i];
    printf("Part 1: %d\n", prod);  // example: 12, input: 221655456

#if !EXAMPLE
    // Part 2
    // Experimentally found variance threshold; for my input the
    // tree configuration has: var3=8048, the first one above 3000,
    // or: var4=2318, the first one above 1000.
    int sec = T;
    for (; var3() < 3000; ++sec)
        for (int i = 0; i < N; ++i) {
            p[i].x = mod(p[i].x + v[i].x, X);  // single time step
            p[i].y = mod(p[i].y + v[i].y, Y);
        }
    printf("Part 2: %d\n", sec);  // example: -, input: 7858
    show();
#endif
    return 0;
}
