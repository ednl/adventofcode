/**
 * Advent of Code 2018
 * Day 23: Experimental Emergency Teleportation
 * https://adventofcode.com/2018/day/23
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 23.c
 *    gcc   -std=gnu17 -Wall -Wextra 23.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 23.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 23.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 139 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 276 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 301 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 675 µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, abs
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define N 1000  // lines in input file

typedef struct {
    int x, y, z;
} Pos;

typedef struct {
    Pos pos;
    int range;
} Nanobot;

typedef struct {
    int dist;
    int delta;
} Tuple;

static Nanobot nanobot[N];
static Tuple counter[N * 2];  // two for each nanobot

// Helper function for qsort: sort nanobots by range descending
static int range_desc(const void *p, const void *q)
{
    const Nanobot *a = p, *b = q;
    if (a->range < b->range) return  1;
    if (a->range > b->range) return -1;
    return 0;
}

// Helper function for qsort: sort counter tuples by distance ascending
static int dist_asc(const void *p, const void *q)
{
    const Tuple *a = p, *b = q;
    if (a->dist < b->dist) return -1;
    if (a->dist > b->dist) return  1;
    return 0;
}

// Manhattan distance
static int manh(const Pos *const a, const Pos *const b)
{
    return abs(a->x - b->x) + abs(a->y - b->y) + abs(a->z - b->z);
}

// Is nanobot test in range of nanobot ref? yes=1, no=0
static int inrange(const Nanobot *const ref, const Nanobot *const test)
{
    return manh(&ref->pos, &test->pos) <= ref->range;
}

int main(void)
{
    // Read input file
    FILE *f = fopen("../aocinput/2018-23-input.txt", "r");
    if (!f)
        return 1;
    for (int i = 0, x, y, z, r; fscanf(f, "pos=<%d,%d,%d>, r=%d ", &x, &y, &z, &r) == 4; ++i)
        nanobot[i] = (Nanobot){(Pos){x, y, z}, r};
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    qsort(nanobot, N, sizeof *nanobot, range_desc);
    int count = 1;  // "including itself"
    for (size_t i = 1; i < N; ++i)
        count += inrange(&nanobot[0], &nanobot[i]);
    printf("Part 1: %d\n", count);  // 933

    // Part 2
    const Pos origin = (Pos){0, 0, 0};
    for (size_t i = 0; i < N; ++i) {
        const int dist = manh(&nanobot[i].pos, &origin);
        const int add = dist > nanobot[i].range ? dist - nanobot[i].range : 0;
        const int sub = dist + nanobot[i].range;
        counter[i * 2]     = (Tuple){add, +1};  // add this bot at the near end of its range
        counter[i * 2 + 1] = (Tuple){sub, -1};  // subtract this bot at the far end of its range
    }
    qsort(counter, N * 2, sizeof *counter, dist_asc);
    size_t i = 0;
    while (counter[i].delta == 1)  // this only works because no delta is -1 until after maximum
        ++i;
    printf("Part 2: %d\n", counter[i - 1].dist);  // 70887840

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
