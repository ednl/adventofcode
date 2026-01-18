/**
 * Advent of Code 2021
 * Day 15: Chiton
 * https://adventofcode.com/2021/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 15.c
 *    gcc   -std=gnu17 -Wall -Wextra 15.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 15.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 15.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=9999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    : 209 ms
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? ms
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :   ? ms
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? ms
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define INP "../aocinput/2021-15-input.txt"
#define DIM 100
#define BQMOD 9  // different edge weights 1-9
static int risk[DIM][DIM] = {0};

typedef struct {
    int x, y;
} Point;

typedef struct PQ {
    struct PQ *prev, *next;
    int64_t prio;
    Point data;
} PQ;

static bool pq_clean(PQ **pq)
{
    while (*pq) {
        PQ *next = (*pq)->next;
        free(*pq);
        *pq = next;
    }
    return true;
}

static bool pq_push(PQ **pq, const Point data, const int prio)
{
    PQ *node = malloc(sizeof(PQ));
    if (!node)
        return false;
    PQ *p = NULL, *q = *pq;
    while (q && q->prio < prio) {
        p = q;
        q = q->next;
    }
    *node = (PQ){.prev=p, .next=q, .prio=prio, .data=data};
    if (p) p->next = node; else *pq = node;
    if (q) q->prev = node;
    return true;
}

static bool pq_pop(PQ **pq, Point * const data)
{
    if (!*pq)
        return false;
    PQ *p = *pq;
    *pq = p->next;
    *data = p->data;
    free(p);
    return true;
}

typedef struct BQL {
    struct BQL *next;
    Point data;
} BQL;
static struct {
    BQL *head, *tail;
} bucketq[BQMOD + 1] = {0};  // 0, 1-9
static int bqsize = 0, bqmin = 0;

static bool bq_push(const Point data, const int prio)
{
    BQL *node = malloc(sizeof(BQL));
    if (!node)
        return false;
    *node = (BQL){.next=NULL, .data=data};
    const int b = prio ? (prio - 1) % BQMOD + 1 : 0;
    if (bucketq[b].tail)
        bucketq[b].tail = (bucketq[b].tail->next = node);
    else
        bucketq[b].head = (bucketq[b].tail = node);
    if (!bqsize++)
        bqmin = b;
    return true;
}

static bool bq_pop(Point *data)
{
    if (!bqsize)
        return false;
    BQL *p = bucketq[bqmin].head;
    *data = p->data;
    --bqsize;
    if ((bucketq[bqmin].head = p->next) == NULL) {
        bucketq[bqmin].tail = NULL;
        if (bqsize)
            while (bucketq[bqmin].head == NULL)
                bqmin = bqmin % BQMOD + 1;
    }
    free(p);
    return data;
}

static void bq_clean(void)
{
    for (int i = 0; i < 10; ++i) {
        while (bucketq[i].head) {
            BQL *p = bucketq[i].head;
            bucketq[i].head = p->next;
            free(p);
        }
        bucketq[i].tail = NULL;
    }
    bqsize = 0;
}

static int astar(const int mult)
{
    static const Point nb[4] = {{-1,0},{0,-1},{1,0},{0,1}};
    int mdim = DIM * mult;
    int cost[mdim][mdim];
    int f[mdim][mdim];
    int g[mdim][mdim];
    bool inq[mdim][mdim];
    Point cur = {0, 0};
    Point end = {mdim - 1, mdim - 1};
    PQ *pq = NULL;

    for (int i = 0; i < mdim; ++i) {
        const int id = i / DIM;
        const int im = i % DIM;
        for (int j = 0; j < mdim; ++j) {
            cost[i][j] = (risk[im][j % DIM] - 1 + id + j / DIM) % BQMOD + 1;
            f[i][j] = g[i][j] = INT_MAX;
            inq[i][j] = false;
        }
    }
    f[0][0] = end.x + end.y;  // h() = Manhattan distance to goal
    g[0][0] = 0;
    pq_push(&pq, cur, f[0][0]);
    // bq_push(cur, f[0][0]);
    inq[cur.y][cur.x] = true;

    while (pq_pop(&pq, &cur)) {
    // while (bq_pop(&cur)) {
        if (cur.x == end.x && cur.y == end.y) {
            pq_clean(&pq);
            // bq_clean();
            return f[end.y][end.x];
        }
        inq[cur.y][cur.x] = false;
        for (size_t i = 0; i < sizeof nb / sizeof *nb; ++i) {
            Point nxt = {cur.x + nb[i].x, cur.y + nb[i].y};
            if (nxt.x >= 0 && nxt.y >= 0 && nxt.x < mdim && nxt.y < mdim) {
                int nxtg = g[cur.y][cur.x] + cost[nxt.y][nxt.x];
                if (nxtg < g[nxt.y][nxt.x]) {
                    g[nxt.y][nxt.x] = nxtg;
                    f[nxt.y][nxt.x] = nxtg + end.x - nxt.x + end.y - nxt.y;  // h() = Manhattan distance to goal
                    if (!inq[nxt.y][nxt.x])
                        inq[nxt.y][nxt.x] = pq_push(&pq, nxt, f[nxt.y][nxt.x]);
                        // inq[nxt.y][nxt.x] = bq_push(nxt, f[nxt.y][nxt.x]);
                }
            }
        }
    }
    pq_clean(&pq);
    // bq_clean();
    return -1;
}

static int dijkstra(const int mult)
{
    static const Point nb[4] = {{-1,0},{0,-1},{1,0},{0,1}};
    int mdim = DIM * mult;
    int edge[mdim][mdim];
    int dist[mdim][mdim];
    bool inq[mdim][mdim];
    Point cur = {0, 0};
    Point end = {mdim - 1, mdim - 1};

    for (int i = 0; i < mdim; ++i) {
        const int idiv = i / DIM;
        const int imod = i % DIM;
        for (int j = 0; j < mdim; ++j) {
            edge[i][j] = (risk[imod][j % DIM] - 1 + idiv + j / DIM) % BQMOD + 1;
            dist[i][j] = INT_MAX;
            inq[i][j] = false;
        }
    }
    dist[0][0] = 0;
    inq[cur.y][cur.x] = bq_push(cur, dist[0][0]);

    while (bq_pop(&cur)) {
        if (cur.x == end.x && cur.y == end.y) {
            bq_clean();
            return dist[end.y][end.x];
        }
        inq[cur.y][cur.x] = false;
        for (size_t i = 0; i < sizeof nb / sizeof *nb; ++i) {
            Point nxt = {cur.x + nb[i].x, cur.y + nb[i].y};
            if (nxt.x >= 0 && nxt.y >= 0 && nxt.x < mdim && nxt.y < mdim) {
                int nxtdist = dist[cur.y][cur.x] + edge[nxt.y][nxt.x];
                if (nxtdist < dist[nxt.y][nxt.x]) {
                    dist[nxt.y][nxt.x] = nxtdist;
                    if (!inq[nxt.y][nxt.x]) {
                        inq[nxt.y][nxt.x] = bq_push(nxt, dist[nxt.y][nxt.x]);
                    }
                }
            }
        }
    }
    bq_clean();
    return -1;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif
    FILE *f = fopen(INP, "r");
    for (int i = 0; i < DIM; ++i) {
        for (int j = 0; j < DIM; ++j)
            risk[i][j] = fgetc(f) - '0';
        fgetc(f);  // newline
    }
    fclose(f);

    printf("Part 1: %d\n", astar(1));  // 592
    printf("Part 2: %d\n", astar(5));  // 2897
    // printf("Part 1: %d\n", dijkstra(1));  // 592
    // printf("Part 2: %d\n", dijkstra(5));  // 2897
#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
