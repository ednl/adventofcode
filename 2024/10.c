/**
 * Advent of Code 2024
 * Day 10: Hoof It
 * https://adventofcode.com/2024/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 10.c
 *    gcc   -std=gnu17 -Wall -Wextra 10.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 10.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 10.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  59 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 123 µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-10-example.txt"
    #define N 8
#else
    #define FNAME "../aocinput/2024-10-input.txt"
    #define N 45
#endif
#define FSIZE (N * (N + 1))
#define SSIZE 32
#define START '0'
#define GOAL  '9'

typedef struct vec {
    int x, y;
} Vec;

typedef struct stack {
    size_t len;
    Vec mem[SSIZE];
} Stack;

static char map[N][N + 1];  // input file
static bool seen[N][N];  // which destinations already visited? (part 1)
static Stack stack;

static bool push(const int x, const int y)
{
    if (stack.len == SSIZE)
        return false;
    stack.mem[stack.len++] = (Vec){x, y};
    return true;
}

static bool pop(int *const restrict x, int *const restrict y)
{
    if (!stack.len)
        return false;
    const Vec val = stack.mem[--stack.len];
    *x = val.x;
    *y = val.y;
    return true;
}

// Depth-first search (DFS)
static Vec findtrails(const int row, const int col)
{
    Vec count = {0};
    int i = row, j = col;
    memset(seen, 0, sizeof seen);  // part 1
    do {
        const char alt = map[i][j];
        if (alt == GOAL) {
            if (!seen[i][j]) {  // part 1
                seen[i][j] = true;
                ++count.x;
            }
            ++count.y;  // part 2
        } else {
            const char up = alt + 1;
            if (i > 0     && map[i - 1][j] == up) push(i - 1, j);
            if (i < N - 1 && map[i + 1][j] == up) push(i + 1, j);
            if (j > 0     && map[i][j - 1] == up) push(i, j - 1);
            if (j < N - 1 && map[i][j + 1] == up) push(i, j + 1);
        }
    } while (pop(&i, &j));
    return count;
}

static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(map, sizeof map, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    Vec sum = {0};
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            if (map[i][j] == START)
                add_r(&sum, findtrails(i, j));
    printf("%d %d\n", sum.x, sum.y);  // 552 1225

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
