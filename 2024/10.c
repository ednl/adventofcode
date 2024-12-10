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
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  80 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
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
#define QSIZE 32  // max needed for my input: 27
#define QMASK (QSIZE - 1)  // QSIZE must be power of 2 => mask of all ones
#define START '0'
#define GOAL  '9'

typedef struct vec {
    int i, j;
} Vec;

typedef struct queue {
    int len, pop, ins;
    Vec q[QSIZE];
} Queue;

// Right, left, down, up (order doesn't matter)
static const Vec step[] = {{0,1},{0,-1},{1,0},{-1,0}};

static char map[N][N + 1];  // input file
static bool seen[N][N];  // explored goals for part 1
static Queue queue;

static void qreset(Queue *const q)
{
    q->len = q->pop = q->ins = 0;
    memset(seen, 0, sizeof seen);
}

// Dequeue = pop off the tail of the queue
static bool deq(Queue *const q, Vec *const val)
{
    if (!q || !q->len)
        return false;
    q->len--;
    *val = q->q[q->pop++];
    q->pop &= QMASK;  // QSIZE must be power of 2 so QMASK is all ones
    return true;
}

// Enqueue = push onto the head of the queue
static bool enq(Queue *const q, const Vec val)
{
    if (!q || q->len == QSIZE) {
        putchar('q');  // major error
        return false;
    }
    q->len++;
    q->q[q->ins++] = val;
    q->ins &= QMASK;  // QSIZE must be power of 2 so QMASK is all ones
    return true;
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.i + b.i, a.j + b.j};
}

static bool ismap(const Vec pos)
{
    return pos.i >= 0 && pos.i < N && pos.j >= 0 && pos.j < N;
}

static char peek(const Vec pos)
{
    return map[pos.i][pos.j];
}

static bool haveseen(const Vec pos)
{
    return seen[pos.i][pos.j];
}

static void markseen(const Vec pos)
{
    seen[pos.i][pos.j] = true;
}

// BFS (breadth-first search) with "mark explored" only for part 1.
static Vec findtrails(const int row, const int col)
{
    Vec count = {0};  // results for part 1 and 2
    qreset(&queue);  // also resets 'seen'
    Vec pos = {row, col};
    enq(&queue, pos);
    while (deq(&queue, &pos)) {
        const char height = peek(pos);
        if (height == GOAL) {
            if (!haveseen(pos)) {
                markseen(pos);
                ++count.i;  // part 1
            }
            ++count.j;  // part 2
        } else {
            const char uphill = height + 1;
            for (int i = 0; i < 4; ++i) {
                const Vec next = add(pos, step[i]);
                if (ismap(next) && peek(next) == uphill)
                    enq(&queue, next);
            }
        }
    }
    return count;
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
                sum = add(sum, findtrails(i, j));
    printf("%d %d\n", sum.i, sum.j);  // example: 36 81, input: 552 1225

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
