/**
 * Advent of Code 2024
 * Day 18: RAM Run
 * https://adventofcode.com/2024/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : ? µs
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define EXAMPLE 1
#if EXAMPLE
    #define FNAME "../aocinput/2024-18-example.txt"
    #define FSIZE 100
    #define N 25
    #define M 12
    #define DIM 7
    #define QSIZE 1024
#else
    #define FNAME "../aocinput/2024-18-input.txt"
    #define FSIZE 20000
    #define N 3450
    #define M 1024
    #define DIM 71
    #define QSIZE 1024
#endif

typedef struct vec {
    int x, y;
} Vec;

static const Vec dir[4] = {{1,0},{0,1},{-1,0},{0,-1}};
static const Vec error = {-1,-1};

static Vec queue[QSIZE];
static int qhead, qtail, qlen;

static char input[FSIZE];
static Vec bytes[N];
static char grid[DIM][DIM];
static bool seen[DIM][DIM];

static void enqueue(const Vec pos)
{
    if (qlen == QSIZE)
        return;
    queue[qhead++] = pos;
    if (qhead == QSIZE)
        qhead = 0;
    qlen++;
}

static Vec dequeue(void)
{
    if (qlen == 0)
        return error;
    qlen--;
    const int i = qtail;
    if (++qtail == QSIZE)
        qtail = 0;
    return queue[i];
}

static bool isequal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

int main(void)
{
    // Read input file
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    const char *c = input;
    for (int i = 0; i < N; ++i) {
        int x = *c++ & 15;
        if (*c != ',')
            x = x * 10 + (*c++ & 15);
        ++c;
        int y = *c++ & 15;
        if (*c != '\n')
            y = y * 10 + (*c++ & 15);
        ++c;
        bytes[i] = (Vec){x, y};
    }
    memset(grid, '.', sizeof grid);
    for (int i = 0; i < M; ++i)
        grid[bytes[i].y][bytes[i].x] = '#';
    for (int i = 0; i < DIM; ++i)
        printf("%."STR(DIM)"s\n", grid[i]);
    Vec pos = {0};
    const Vec goal = {DIM - 1, DIM - 1};

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
