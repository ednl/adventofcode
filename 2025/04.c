/**
 * Advent of Code 2025
 * Day 4: Printing Department
 * https://adventofcode.com/2025/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 244 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 360 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 601 µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-04-example.txt"
    #define N 10
    #define QSIZE 16
#elif EXAMPLE == 2
    #define FNAME "04_extra.txt"
    #define N 258
    #define QSIZE 10000  // max needed for extra input: 8638
#else
    #define FNAME "../aocinput/2025-04-input.txt"
    #define N 140
    #define QSIZE 2048  // max needed for my input: 1495 (=first iteration)
#endif

#define ROLL  '@'   // paper roll
#define SPACE '.'   // free space/floor
#define BOXED 4     // 4 or more neighbours = unreachable for the forklift

typedef struct vec {
    int row, col;
} Vec;

static char grid[N + 2][N + 3];  // +border, +'\0'
static bool seen[N + 2][N + 3];  // marked for removal (=queued)
static Vec queue[QSIZE];
static int qhead, qtail, qlen;

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

#if EXAMPLE == 1
static void show(void)
{
    for (int i = 1; i <= N; ++i)
        printf("%*s", N + 1, &grid[i][1]);  // includes newline
}
#endif

// Neigbour count low enough?
static bool reachable(const int row, const int col)
{
    int nb = 0;
    for (int i = row - 1; i < row + 2; ++i)
        for (int j = col - 1; j < col + 2; ++j)
            nb += grid[i][j] == ROLL;  // count self, too
    return nb < BOXED + 1;  // +1 because self is always '@'
}

// Is it a paper roll, and not in the queue yet, and doesn't have too many neighbours?
static void check(const int row, const int col)
{
    if (grid[row][col] == ROLL && !seen[row][col] && reachable(row, col)) {
        enqueue((Vec){row, col});
        seen[row][col] = true;  // marked for removal = in the queue
    }
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 1; i <= N && fgets(&grid[i][1], N + 2, f); ++i);  // +"\n\0"
    fclose(f);
#if EXAMPLE == 1
    show();
#endif

#ifdef TIMER
    starttimer();
#endif

    // Part 1: fill queue with all paper rolls to be removed
    for (int i = 1; i < N + 1; ++i)
        for (int j = 1; j < N + 1; ++j)
            check(i, j);
    printf("%d\n", qlen);  // example: 13, input: 1495

    // Part 2: remove, check neighbours, repeat until no more paper rolls removable
    int removed = 0;
    for (Vec pos; dequeue(&pos); ++removed) {
        grid[pos.row][pos.col] = SPACE;  // forklift action
        for (int i = -1; i < 2; ++i)
            for (int j = -1; j < 2; ++j)
                check(pos.row + i, pos.col + j);  // neighbours might be reachable now
    }
#if EXAMPLE == 1
    show();
#endif
    printf("%d\n", removed); // example: 43, input: 8768

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
