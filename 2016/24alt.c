/**
 * Advent of Code 2016
 * Day 24: Air Duct Spelunking
 * https://adventofcode.com/2016/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic ../combperm.c 24alt.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c ../combperm.c 24alt.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 279 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>    // fopen, fclose, printf, putchar, puts
#include <string.h>   // memset
#include <stdbool.h>  // bool
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-24-input.txt"
#define W 179      // grid width
#define H 43       // grid height
#define N 8        // points of interest 0..7
#define FAC7 2520  // 7!/2 = 5040/2 = 2520
#define QSIZE 100  // needed for my input: max=69 (nice)

typedef struct vec2 {
    int x, y;
} Vec2;

typedef struct state {
    Vec2 pos;
    int dist;
} State;

typedef struct queue {
    size_t len, pop, ins;
    State q[QSIZE];
} Queue;

static const Vec2 dir[4] = {{-1,0},{1,0},{0,-1},{0,1}};
static Queue queue;
static char grid[H][W];
static bool seen[H][W];
static Vec2 poi[N];     // position of POI 0..7
static int dist[N][N];  // distance between POI i and j
static int mindist1 = W * H;
static int mindist2 = W * H;

// Dequeue = pop off the tail of the queue
static bool deq(Queue *const q, State *const val)
{
    if (!q || !q->len)
        return false;
    q->len--;
    *val = q->q[q->pop++];
    if (q->pop == QSIZE)
        q->pop = 0;
    return true;
}

// Enqueue = push onto the head of the queue
static bool enq(Queue *const q, const State val)
{
    if (!q || q->len == QSIZE)
        return false;
    q->len++;
    q->q[q->ins++] = val;
    if (q->ins == QSIZE)
        q->ins = 0;
    return true;
}

static Vec2 add(const Vec2 a, const Vec2 b)
{
    return (Vec2){a.x + b.x, a.y + b.y};
}

static void flood(const int start)
{
    memset(seen, 0, sizeof seen);
    seen[poi[start].y][poi[start].x] = true;
    State s = {poi[start], 0};
    enq(&queue, s);
    while (deq(&queue, &s)) {
        const char c = grid[s.pos.y][s.pos.x];
        if (c >= '0' && c < '0' + N)
            dist[start][c - '0'] = dist[c - '0'][start] = s.dist;
        for (int i = 0; i < (int)(sizeof dir / sizeof *dir); ++i) {
            Vec2 next = add(s.pos, dir[i]);
            if (grid[next.y][next.x] != '#' && !seen[next.y][next.x]) {
                seen[next.y][next.x] = true;
                if (!enq(&queue, (State){next, s.dist + 1}))
                    puts("***** QUEUE OVERFLOW *****");
            }
        }
    }
}

#ifdef DEBUG
static void showdist(void)
{
    printf("\n           |");
    for (int i = 0; i < N; ++i)
        printf("%4d", i);
    putchar('\n');
    puts("-----------+--------------------------------");
    for (int i = 0; i < N; ++i) {
        printf("%d (%3d,%2d) |", i, poi[i].x, poi[i].y);
        for (int j = 0; j < N; ++j)
            printf("%4d", dist[i][j]);
        putchar('\n');
    }
    putchar('\n');
}
#endif

static void heapperm(char *a, int k)
{
    if (k == 1) {
        int d = dist[0][a[0]];
        for (int n = 0; n < N - 2; ++n)
            d += dist[a[n]][a[n + 1]];
        if (d < mindist1) mindist1 = d;
        d += dist[a[N - 2]][0];
        if (d < mindist2) mindist2 = d;
        return;
    }
    heapperm(a, --k);
    for (int i = 0; i < k; ++i) {
        const int j = (k & 1) * i;
        const char tmp = a[j];
        a[j] = a[k];
        a[k] = tmp;
        heapperm(a, k);
    }
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    int c, x = 0, y = 0;
    while (y < H && (c = fgetc(f)) != EOF) {
        if (c == '\n') {
            x = 0;
            ++y;
        } else {
            if (c >= '0' && c < '0' + N)
                poi[c - '0'] = (Vec2){x, y};
            if (x < W)
                grid[y][x++] = c;
        }
    }
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    for (int i = 0; i < N; ++i)
        flood(i);
#ifdef DEBUG
    showdist();
#endif

    char a[] = {1,2,3,4,5,6,7};
    heapperm(a, 7);
    printf("%d %d\n", mindist1, mindist2);  // 490 744

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
