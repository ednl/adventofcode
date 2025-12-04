/**
 * Advent of Code 2025
 * Day 4: Printing Department
 * https://adventofcode.com/2025/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 04alt.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04alt.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 250 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 603 µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2025-04-example.txt"
    #define N 10
#else
    #define FNAME "../aocinput/2025-04-input.txt"
    #define N 140
#endif

#define ROLL  '@'   // paper roll
#define SPACE '.'   // free space/floor
#define BOXED 4     // 4 or more neighbours = unreachable for the forklift
#define QSIZE 2048  // max needed for my input: 1495 (=first iteration)

typedef struct vec {
    int row, col;
} Vec;

static char grid[N + 2][N + 3];  // +border, +'\0'
static bool seen[N + 2][N + 3];  // marked for removal (=queued)
static Vec queue[QSIZE];
static int qhead, qtail, qlen;

static bool enqueue(const Vec pos)
{
    if (qlen == QSIZE)
        return false;
    queue[qhead++] = pos;
    if (qhead == QSIZE)
        qhead = 0;
    qlen++;
    return true;
}

static bool dequeue(Vec *const v)
{
    if (qlen == 0)
        return false;
    qlen--;
    const int i = qtail;
    if (++qtail == QSIZE)
        qtail = 0;
    *v = queue[i];
    return true;
}

#if EXAMPLE
static void show(void)
{
    for (int i = 1; i <= N; ++i)
        printf("%*s", N + 1, &grid[i][1]);  // includes newline
}
#endif

// Neigbour count low enough?
// Centre cell is always '@', so adds 1
static bool reachable(const int row, const int col)
{
    int nb = 0;
    for (int i = row - 1; i < row + 2; ++i)
        for (int j = col - 1; j < col + 2; ++j)
            nb += grid[i][j] == ROLL;
    return nb < BOXED + 1;
}

// Is it a paper roll, and not in the queue yet, and doesn't have too many neighbours?
static void check(const int row, const int col)
{
    if (grid[row][col] == ROLL && !seen[row][col] && reachable(row, col)) {
        enqueue((Vec){row, col});
        seen[row][col] = true;
    }
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 1; i <= N && fgets(&grid[i][1], N + 2, f); ++i);  // +"\n\0"
    fclose(f);
#if EXAMPLE
    show();
#endif

#ifdef TIMER
    starttimer();
#endif

    // Part 1: fill the queue with all paper rolls to be removed
    for (int i = 1; i < N + 1; ++i)
        for (int j = 1; j < N + 1; ++j)
            check(i, j);
    printf("%d\n", qlen);  // example: 13, input: 1495

    // Part 2: remove, check neighbours, repeat until no more paper rolls removable
    Vec pos;
    int removed = 0;
    while (dequeue(&pos)) {
        grid[pos.row][pos.col] = SPACE;
        ++removed;
        for (int i = -1; i < 2; ++i)
            for (int j = -1; j < 2; ++j)
                check(pos.row + i, pos.col + j);
    }
#if EXAMPLE
    show();
#endif
    printf("%d\n", removed); // example: 43, input: 8768

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
