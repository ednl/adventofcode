/**
 * Advent of Code 2016
 * Day 24: Air Duct Spelunking
 * https://adventofcode.com/2016/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 24.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 24.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 271 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 552 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 999 µs
 */

#include <stdio.h>    // fopen, fclose, printf, putchar, puts
#include <string.h>   // memset, strcpy
#include <stdbool.h>  // bool
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-24-input.txt"
#define W 179      // width
#define H 43       // height
#define N 8        // points of interest 0..7
#define FAC7 5040  // 7! = 5040
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
static Vec2 poi[N];         // position of POI 0..7
static int dist[N][N];      // distance between POI i and j
static char perm[FAC7][N];  // all permutations of "1234567" (always start at 0)

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

static void swap(char *const a, char *const b)
{
    const char tmp = *a;
    *a = *b;
    *b = tmp;
}

static void permute(char *s, int l, int r)
{
    static int count = 0;

    if (l == r)
        strcpy(perm[count++], s);
    else
        for (int i = l; i <= r; ++i) {
            swap(s + l, s + i);
            permute(s, l + 1, r);
            swap(s + l, s + i);  // backtrack
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

static void mindist(const int part)
{
    int min = W * H;
#ifdef DEBUG
    int ix = 0;
#endif
    for (int i = 0; i < FAC7; ++i) {
        int sum = part == 2 ? dist[0][(int)perm[i][0]] : 0;  // part 2: return to '0'
        for (int j = 0; j < N - 1; ++j)
            sum += dist[(int)perm[i][j]][(int)perm[i][j + 1]];  // perm[i][7] = '\0'
        if (sum < min) {
            min = sum;
        #ifdef DEBUG
            ix = i;
        #endif
        }
    }
#ifdef DEBUG
    char s[8] = {0};
    strcpy(s, perm[ix]);
    for (int i = 0; i < 7; ++i)
        s[i] += '0';
    int i = 0, j = N - 2;
    while (i < j)
        swap(s + i++, s + j--);
    printf("Part %d: \"0%s%s\" = %d\n", part, s, part == 2 ? "0" : "", min);  // 490, 744
#else
    printf("%d\n", min);
#endif
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
    char s[] = {1,2,3,4,5,6,7,0};
    permute(s, 0, N - 2);

    mindist(1);  // 490
    mindist(2);  // 744

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
