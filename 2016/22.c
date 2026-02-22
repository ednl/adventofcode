/**
 * Advent of Code 2016
 * Day 22: Grid Computing
 * https://adventofcode.com/2016/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 22.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 22.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  52 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs, qsort, free
#include <string.h>  // memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-22-input.txt"
#define X 37
#define Y 25
#define N (X * Y)

typedef struct node {
    int id, x, y, used, avail;
} Node;

static Node grid[N], copy1[N], copy2[N];

static int used_asc(const void *p, const void *q)
{
    const Node *const a = p;
    const Node *const b = q;
    if (a->used < b->used) return -1;
    if (a->used > b->used) return  1;
    return 0;
}

static int avail_desc(const void *p, const void *q)
{
    const Node *const a = p;
    const Node *const b = q;
    if (a->avail < b->avail) return  1;
    if (a->avail > b->avail) return -1;
    return 0;
}

static int parse(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) {
        fprintf(stderr, "File not found: "FNAME"\n");
        return 0;  // zero lines read
    }
    char *line = NULL;
    size_t size;
    getline(&line, &size, f);  // "shell command"
    getline(&line, &size, f);  // header
    int n = 0;
    while (n < N && getline(&line, &size, f) > 0) {
        int x, y, s, used, avail;
        if (sscanf(line + 16, "%d-y%d %dT %dT %d", &x, &y, &s, &used, &avail) == 5) {
            grid[n] = (Node){n, x, y, used, avail};
            ++n;
        }
    }
    fclose(f);
    free(line);
    return n;
}

static int manh(const int x0, const int y0, const int x1, const int y1)
{
    return abs(x0 - x1) + abs(y0 - y1);
}

int main(void)
{
    int n = parse();

#ifdef TIMER
    starttimer();
#endif

    memcpy(copy1, grid, sizeof grid);
    memcpy(copy2, grid, sizeof grid);
    qsort(copy1, n, sizeof *copy1, used_asc);
    qsort(copy2, n, sizeof *copy2, avail_desc);

    int viable = 0;
    const Node *a = copy1, *b = NULL, *empty = NULL;
    for (int i = 0; i < n; ++i) {
        b = copy2;
        if (a->used > b->avail)
            break;
        if (a->used)
            for (int j = 0; j < n; ++j) {
                if (a->used > b->avail)
                    break;
                viable += a->id != b->id;
                ++b;
            }
        else
            empty = &grid[a->id];
        ++a;
    }
    printf("Part 1: %d\n\n", viable);  // 892

    // Header
    printf("  ");
    for (int x = 0; x < X; ++x)
        printf("%2d", x % 10);
    putchar('\n');
    // Grid
    for (int y = 0; y < Y; ++y) {
        printf("%2d", y);
        for (int x = 0; x < X; ++x) {
            putchar(' ');
            if (!x && !y) putchar('X');
            else if (x == X - 1 && !y) putchar('G');
            else if (x == empty->x && y == empty->y) putchar('E');
            else if (grid[x * Y + y].used > empty->avail) putchar('#');
            else putchar('.');
        }
        printf(" %d\n", y);
    }
    // Footer
    printf("  ");
    for (int x = 0; x < X; ++x)
        printf("%2d", x % 10);
    printf("\n\n");

    int moves = 0;
    printf("E  : (%2d,%2d)\n", empty->x, empty->y);
    printf("E  : (%2d,%2d) = %3d\n",  4, 7, (moves += manh(empty->x, empty->y, 4, 7)));
    printf("E  : (%2d,%2d) = %3d\n", 35, 0, (moves += manh(4, 7, 35, 0)));
    printf("EG : (%2d,%2d) = %3d\n",  0, 0, (moves += 5 * manh(35, 0, 0, 0)));
    printf("GE : (%2d,%2d) = %3d\n",  0, 0, ++moves);
    printf("\nPart 2: %d\n", moves);  // 227

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
