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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 32 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? ms
 *     Raspberry Pi 5 (2.4 GHz)      : 92 ms
 */

#include <stdio.h>
#include <stdlib.h>    // qsort, malloc, free
#include <string.h>    // memcpy
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
    #define M 10  // closest pairs to connect for part 1
#else
    #define FNAME "../aocinput/2025-08-input.txt"
    #define N 1000  // junction boxes
    #define M 1000  // closest pairs to connect for part 1
#endif
#define PAIRS ((N - 1) * N >> 1)  // lower/upper triangle count of N by N matrix
#define CIRCUITS (N >> 1)  // max needed for my input: 298
#define INITCOUNT 8

typedef struct vec {
    int x, y, z;
} Vec;

typedef struct pair {
    int64_t dist;  // faster to sort with key in front?
    int index[2];
} Pair;

typedef struct circuit {
    int *box;  // index
    int len, size;
} Circuit;

static Vec junctionbox[N];
static Pair pair[PAIRS];
static Circuit circuit[CIRCUITS];
static int circuitsize[CIRCUITS];  // separate array to sort for part 1
static int circuitcount;  // max=CIRCUITS
static int circuitid[N];  // circuit ID per junction box index

static Vec sub(const Vec a, const Vec b)
{
    return (Vec){a.x - b.x, a.y - b.y, a.z - b.z};
}

static int64_t sqrsum(const Vec a)
{
    return (int64_t)a.x * a.x + (int64_t)a.y * a.y + (int64_t)a.z * a.z;
}

static int64_t sqrdist(const int i, const int j)
{
    return sqrsum(sub(junctionbox[i], junctionbox[j]));
}

static int desc(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return  1;
    if (a > b) return -1;
    return 0;
}

static int cmpdist(const void *p, const void *q)
{
    const Pair *a = p;
    const Pair *b = q;
    if (a->dist < b->dist) return -1;
    if (a->dist > b->dist) return  1;
    return 0;
}

static bool grow(void **curptr, const size_t ptrsize, int *curcount)
{
#if DEBUG
    if (!curptr || !*curptr || !ptrsize || !curcount) {
        fprintf(stderr, "Error: null pointer in grow()\n");
        return false;
    }
#endif
    const int newcount = *curcount > 0 ? *curcount << 1 : INITCOUNT;
#if DEBUG
    if (newcount <= *curcount) {
        fprintf(stderr, "Error: overflow in grow()\n");
        return false;
    }
#endif
    void *newptr = realloc(*curptr, ptrsize * newcount);
#if DEBUG
    if (!newptr) {
        fprintf(stderr, "Error: out of memory in grow()\n");
        return false;
    }
#endif
    *curptr = newptr;
    *curcount = newcount;
    return true;
}

static int createcircuit(const int box0, const int box1)
{
    if (circuitcount == CIRCUITS) {
        fprintf(stderr, "Error: too many circuits in createcircuit(): %d\n", circuitcount);
        return 0;
    }
    const int size = INITCOUNT;
    void *p = malloc(size * sizeof circuit->box[0]);
#if DEBUG
    if (!p) {
        fprintf(stderr, "Error: out of memory in createcircuit()\n");
        return 0;
    }
#endif
    const int len = 2;
    circuit[circuitcount] = (Circuit){p, len, size};
    circuit[circuitcount].box[0] = box0;
    circuit[circuitcount].box[1] = box1;
    circuitcount++;  // circuit[x] has ID x+1
    circuitid[box0] = circuitcount;
    circuitid[box1] = circuitcount;
    return len;
}

static int addtocircuit(const int id, const int box)
{
#if DEBUG
    if (id < 1 || id > CIRCUITS)
        return 0;
#endif
    Circuit *const c = &circuit[id - 1];  // circuit[x] has ID x+1
    if (c->len == c->size && !grow((void **)&c->box, sizeof *(c->box), &c->size)) {
        fprintf(stderr, "Error: buffer full in addtocircuit(): %d\n", c->size);
        return c->len;
    }
    c->box[c->len++] = box;
    circuitid[box] = id;
    return c->len;
}

static int mergecircuit(const int id0, const int id1)
{
#if DEBUG
    if (id0 < 1 || id0 > CIRCUITS || id1 < 1 || id1 > CIRCUITS || id0 == id1) {
        fprintf(stderr, "Error: ID(s) out of range in mergecircuit(): %d,%d\n", id0, id1);
        return 0;
    }
#endif
    const int dstid = id0 < id1 ? id0 : id1;
    const int srcid = id0 < id1 ? id1 : id0;
    Circuit *const dst = &circuit[dstid - 1];  // circuit[x] has ID x+1
#if DEBUG
    if (!dst->box || dst->len <= 0 || dst->len > dst->size) {
        fprintf(stderr, "Error: wrong destination ID in mergecircuit(): %d\n", dstid);
        return 0;
    }
#endif
    Circuit *const src = &circuit[srcid - 1];  // circuit[x] has ID x+1
#if DEBUG
    if (!src->box || src->len <= 0 || src->len > src->size) {
        fprintf(stderr, "Error: wrong source ID in mergecircuit(): %d\n", srcid);
        return 0;
    }
#endif
    const int newlen = dst->len + src->len;
    while (newlen > dst->size && grow((void **)&dst->box, sizeof *(dst->box), &dst->size));
#if DEBUG
    if (newlen > dst->size) {
        fprintf(stderr, "Error: out of memory in mergecircuit(): %d\n", newlen);
        return 0;
    }
#endif
    memcpy(dst->box + dst->len, src->box, src->len * sizeof *(src->box));
    dst->len = newlen;
    for (int i = 0; i < src->len; ++i)
        circuitid[src->box[i]] = dstid;
    free(src->box);
    *src = (Circuit){0};
    return newlen;
}

// Return part 2 answer if size=N
static int64_t addpairs(int i, const int end)
{
    int size = 0;
    for (; i != end && size != N; ++i) {
        const int *const x = &pair[i].index[0];
        const int id0 = circuitid[x[0]];
        const int id1 = circuitid[x[1]];
        if (!id0 && !id1)
            size = createcircuit(x[0], x[1]);
        else if (!id0)
            size = addtocircuit(id1, x[0]);
        else if (!id1)
            size = addtocircuit(id0, x[1]);
        else if (id0 != id1)
            size = mergecircuit(id0, id1);
    }
    return size == N ? (int64_t)junctionbox[pair[i - 1].index[0]].x * junctionbox[pair[i - 1].index[1]].x : 0;
}

int main(void)
{
    // Read input file from disk
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 0, x, y, z; i < N && fscanf(f, "%d,%d,%d", &x, &y, &z) == 3; ++i)
        junctionbox[i] = (Vec){x, y, z};
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Distance between unique index pairs
    for (int i = 0, m = 0; i < N - 1; ++i)
        for (int j = i + 1; j < N; ++j)
            pair[m++] = (Pair){sqrdist(i, j), {i, j}};
    // Sort connected pairs by distance ascending
    qsort(pair, PAIRS, sizeof *pair, cmpdist);
    // Add first M connected pairs with shortest distance
    addpairs(0, M);
    {  // Find 3 largest circuit sizes
        int k = 0;
        for (int i = 0; i < circuitcount; ++i)
            if (circuit[i].len > 1)
                circuitsize[k++] = circuit[i].len;
        qsort(circuitsize, k, sizeof *circuitsize, desc);
    }
    printf("Part 1: %d\n", circuitsize[0] * circuitsize[1] * circuitsize[2]);  // example: 40, input: 163548
    printf("Part 2: %"PRId64"\n", addpairs(M, PAIRS));  // example: 25272, input: 772452514

    // Cleanup
    for (int i = 0; i < circuitcount; ++i)
        free(circuit[i].box);

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
