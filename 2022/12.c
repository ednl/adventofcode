/**
 * Advent of Code 2022
 * Day 12: Hill Climbing Algorithm
 * https://adventofcode.com/2022/day/12
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 12.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 12.c
 * Test output with timer enabled:
 *     ./a.out | tail -n7
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 15.3 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>  // int16_t
#include <stdbool.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2022-12-input.txt"
#define ROWS  41   // lines in input file
#define COLS  159  // line width in input file (without newline)
#define QSIZE 64   // must be power of 2 (needed for my input: 42)

typedef struct {
    int16_t x, y;
} Vec;

static const Vec dir[4] = {{-1,0},{0,1},{0,-1},{1,0}};

static Vec queue[QSIZE];
static int qhead, qtail, qlen;

static char alt[ROWS][COLS + 1];  // altitude map +newline
static int16_t dist[ROWS][COLS];  // distance travelled

static void enqueue(const Vec pos)
{
    queue[qhead++] = pos;
    qhead &= (QSIZE - 1);
    qlen++;
}

static Vec dequeue(void)
{
    qlen--;
    const int i = qtail++;
    qtail &= (QSIZE - 1);
    return queue[i];
}

static bool isequal(const Vec a, const Vec b)
{
    return a.x == b.x && a.y == b.y;
}

static Vec add(const Vec a, const Vec b)
{
    return (Vec){a.x + b.x, a.y + b.y};
}

static void findSE(Vec *const restrict start, Vec *const restrict end)
{
    for (int i = 0, found = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j) {
            if (alt[i][j] & 32)  // lowercase?
                continue;
            switch (alt[i][j]) {
                case 'S': *start = (Vec){j, i}; alt[i][j] = 'a'; found++; break;
                case 'E':   *end = (Vec){j, i}; alt[i][j] = 'z'; found++; break;
            }
            if (found == 2)
                return;
        }
}

// Going down by 1 step max (level or up also allowed)
static bool reachable(const Vec v, const char base)
{
    if (v.x < 0 || v.y < 0 || v.x >= COLS || v.y >= ROWS || dist[v.y][v.x])
        return false;  // off-grid or visited
    return alt[v.y][v.x] > base - 2;  // descent <= 1
}

static int flood(const Vec start, const Vec goal, int *const dist2a)
{
    dist[start.y][start.x] = 1;
    enqueue(start);
    while (qlen) {
        Vec v = dequeue();
        const char a = alt[v.y][v.x];
        const int d = dist[v.y][v.x];
        if (a == 'a' && !*dist2a)
            *dist2a = d - 1;
        if (isequal(v, goal))  // found shortest path
            return d - 1;
        for (size_t i = 0; i < sizeof dir / sizeof *dir; ++i) {
            const Vec w = add(v, dir[i]);
            if (reachable(w, a)) {
                dist[w.y][w.x] = d + 1;
                enqueue(w);
            }
        }
    }
    return 0;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(alt, sizeof alt, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    // Reset tracking data at start of timing loop
    qhead = qtail = qlen = 0;
    memset(dist, 0, sizeof dist);
#endif

    Vec start = {0}, end = {0};
    findSE(&start, &end);
    int part2 = 0;
    const int part1 = flood(end, start, &part2);
    printf("%d %d\n", part1, part2);  // 504 500

#ifdef TIMER
    // Reset input file for next timing loop
    alt[start.y][start.x] = 'S';
    alt[end.y][end.x] = 'E';
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
