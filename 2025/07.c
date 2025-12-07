/**
 * Advent of Code 2025
 * Day 7: Laboratories
 * https://adventofcode.com/2025/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-07-example.txt"
    #define N 15
    #define QSIZE 32  // max needed for example part 1: 10, part 2: 22
#else
    #define FNAME "../aocinput/2025-07-input.txt"
    #define N 141
    #define QSIZE 128  // max needed for my input part 1: 92, part 2: ?
#endif
#define M (N + 1)
#define FSIZE (M * M)  // +1 bottom row, +1 '\n'

#define START (N >> 1)  // example: 7, input: 70
#define SPLIT '^'   // splitter
#define SPACE '.'   // free space
#define BEAM  '|'

typedef struct vec {
    int row, col;
} Vec;

static char grid[M][M];
static bool seen[M][M];  // beam was here
static Vec queue[QSIZE];
static int qhead, qtail, qlen;
#if DEBUG
    static int maxq;
#endif

// Add to head of queue (FIFO)
// Return false if queue was full
static bool enqueue(const Vec pos)
{
    if (qlen == QSIZE)
        return false;
    queue[qhead++] = pos;
    if (qhead == QSIZE)
        qhead = 0;
    qlen++;
#if DEBUG
    if (qlen > maxq)
        maxq = qlen;
#endif
    return true;
}

// Remove from tail of queue (FIFO)
// Return false if queue was empty
static bool dequeue(Vec *const v)
{
    if (qlen == 0)
        return false;
    *v = queue[qtail++];
    if (qtail == QSIZE)
        qtail = 0;
    qlen--;
    return true;
}

static bool splitter(const Vec pos)
{
    return grid[pos.row][pos.col] == SPLIT;
}

static bool setbeam(const Vec pos)
{
#if EXAMPLE || DEBUG
    if (grid[pos.row][pos.col] == SPACE)
        grid[pos.row][pos.col] = BEAM;
#endif
    return seen[pos.row][pos.col] = true;
}

static Vec side(const Vec pos, const int dx)
{
    return (Vec){pos.row, pos.col + dx};
}

static bool check(const Vec pos)
{
    return !seen[pos.row][pos.col] && enqueue(pos) && setbeam(pos);
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(grid, sizeof grid, 1, f);  // read whole file at once
    fclose(f);

#if EXAMPLE
    fwrite(grid, sizeof grid, 1, stdout);
    printf("\n");
#endif

#ifdef TIMER
    starttimer();
#endif

    int split = 0;
    enqueue((Vec){0, START});
    for (Vec pos; dequeue(&pos); ) {
        ++pos.row;
        setbeam(pos);
        if (pos.row == N)
            continue;
        ++pos.row;
        if (splitter(pos)) {
            ++split;
            check(side(pos, -1));
            check(side(pos, +1));
        } else
            check(pos);
    }
#if EXAMPLE || DEBUG
    fwrite(grid, sizeof grid, 1, stdout);
    printf("\n");
#endif
#if DEBUG
    printf("maxq=%d\n", maxq);
#endif
    printf("%d\n", split);  // example: 21, input: 1598

    uint64_t paths = 1;
    enqueue((Vec){0, START});
    for (Vec pos; dequeue(&pos); ) {
        for (; pos.row < M && !splitter(pos); pos.row++);
        if (pos.row == M)
            continue;
        ++paths;
        enqueue(side(pos, -1));
        enqueue(side(pos, +1));
    }
#if DEBUG
    printf("maxq=%d\n", maxq);
#endif
    printf("%"PRIu64"\n", paths);  // example: 40, input: ?

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
