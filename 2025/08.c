/**
 * Advent of Code 2025
 * Day 8: Playground
 * https://adventofcode.com/2025/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 08.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 08.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2025-08-example.txt"
    #define N 20  // junction boxes
    #define L 10  // closest pairs to connect
    #define QSIZE 8  // max for example: 3
#else
    #define FNAME "../aocinput/2025-08-input.txt"
    #define N 1000  // junction boxes
    #define L 1000  // closest pairs to connect
    #define QSIZE 32  // max for my input: 17
#endif

// Lower/upper triangle count of NxN matrix
#define M ((N - 1) * N >> 1)

typedef struct vec {
    int64_t x, y, z;
} Vec;

typedef struct pair {
    int index[2];
    int64_t dist;
} Pair;

static Vec box[N];
static Pair pair[M];
static bool conn[N][N];
static bool seen[N];

static int circuitsize[N];
static int circuits;

static int queue[QSIZE];
static int qhead, qtail, qlen;
#if DEBUG
    static int qmax;
#endif

// Add to head of queue (FIFO)
// Return false if queue was full
static bool enqueue(const int data)
{
    if (qlen == QSIZE)
        return false;
    queue[qhead++] = data;
    if (qhead == QSIZE)
        qhead = 0;
    qlen++;
#if DEBUG
    if (qlen > qmax)
        qmax = qlen;
#endif
    return true;
}

// Remove from tail of queue (FIFO)
// Return false if queue was empty
static bool dequeue(int *const data)
{
    if (qlen == 0)
        return false;
    *data = queue[qtail++];
    if (qtail == QSIZE)
        qtail = 0;
    qlen--;
    return true;
}

static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y, a.z - b.z};
}

static int64_t sqrsum(const Vec a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

static int64_t sqrdist(const Vec a, const Vec b)
{
    return sqrsum(sub(a, b));
}

static int cmpdist(const void *p, const void *q)
{
    const Pair *a = p;
    const Pair *b = q;
    if (a->dist < b->dist) return -1;
    if (a->dist > b->dist) return  1;
    return 0;
}

static int desc(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return  1;
    if (a > b) return -1;
    return 0;
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 0, x, y, z; i < N && fscanf(f, "%d,%d,%d", &x, &y, &z) == 3; ++i)
        box[i] = (Vec){x, y, z};
    fclose(f);

#if DEBUG
    for (int i = N - 3; i < N; ++i)
        printf("box[%d] = %"PRId64",%"PRId64",%"PRId64"\n", i, box[i].x, box[i].y, box[i].z);
    printf("\n");
#endif

#ifdef TIMER
    starttimer();
#endif

    // Distance between unique index pairs
    for (int i = 0, m = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            pair[m++] = (Pair){{i, j}, sqrdist(box[i], box[j])};

    // Sort connected pairs by distance
    qsort(pair, M, sizeof *pair, cmpdist);

#if EXAMPLE || DEBUG
    for (int i = 0; i < L && i < 15; ++i)
        printf("%2d: %3d %3d %8"PRId64"\n", i, pair[i].index[0], pair[i].index[1], pair[i].dist);
    printf("\n");
#endif

    // Connection matrix of L shortest distance pairs
    for (int k = 0; k < L; ++k) {
        const int i = pair[k].index[0];
        const int j = pair[k].index[1];
        conn[i][j] = conn[j][i] = true;
    }

    for (int k = 0; k < L; ++k) {
        int i = pair[k].index[0];
        if (!seen[i]) {
            seen[i] = enqueue(i);
            while (dequeue(&i)) {
                circuitsize[circuits]++;
                for (int j = 0; j < N; ++j)
                    if (conn[i][j] && !seen[j])
                        seen[j] = enqueue(j);
            }
            circuits++;
        }
    }

#if DEBUG
    printf("qmax=%d\n", qmax);
#endif

    qsort(circuitsize, circuits, sizeof *circuitsize, desc);

#if DEBUG
    printf("circuits=%d\n", circuits);
    for (int i = 0; i < circuits; ++i)
        printf(" %d", circuitsize[i]);
    printf("\n\n");
#endif

    int64_t prod = 1;
    for (int i = 0; i < 3; ++i)
        prod *= circuitsize[i];
    printf("Part 1: %"PRId64"\n", prod);  // example: 40, input: 163548

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
