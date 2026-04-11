/**
 * Advent of Code 2018
 * Day 10:
 * https://adventofcode.com/2018/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 10.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.58 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ?    µs
 *     Raspberry Pi 5 (2.4 GHz)      : ?    µs
*/

#include <stdio.h>
#include <string.h>  // memset
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-10-input.txt"
#define N 324  // lines in input file
#define M 3    // use first M points to guess final time

// Matrix display dimensions WITH HINDSIGHT
#define CHARS 8  // number of characters in answer to part 1
#define CHARW 6  // each character is 6 dots wide
#define SPACE 2  // characters are separated by 2 columns
#define W (CHARS * (CHARW + SPACE) - SPACE)  // 62
#define H 10     // each character is 10 dots high

typedef struct vec {
    int x, y;
} Vec;

typedef struct box {
    Vec corner;
    Vec range;
} Box;

// Old and new positions, constant velocity
static Vec pos0[N];
static Vec pos1[N];
static Vec vel[N];

// Matrix display
static char matrix[H][W];

// Read input file, return number of lines read
static int parse(Vec *pos, const int maxlen)
{
    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 0;  // error: zero lines read
    Vec p, v;
    int n = 0;
    for (; n < maxlen && fscanf(f, "position=< %d, %d> velocity=< %d, %d> ", &p.x, &p.y, &v.x, &v.y) == 4; n++) {
        pos[n] = p;
        vel[n] = v;
    }
    fclose(f);
    return n;
}

// Intersection: x0 + v0.t = x1 + v1.t => t = (x0 - x1) / (v1 - v0)
// y should be closer than x because y is spread less
// Pre: 2 <= len <= N
static int guess(const Vec *pos, const int len)
{
    int sum = 0, count = 0;
    for (int i = 0; i < len - 1; ++i)
        for (int j = i + 1; j < len; ++j) {
            const int dy = pos[j].y - pos[i].y;
            const int dv = vel[i].y - vel[j].y;
            if (dv && dy && (dv < 0 == dy < 0)) {
                sum += dy / dv;
                count++;
            }
        }
    return count ? (sum + count / 2) / count : 0;
}

// Bounding box dimensions from array of position vectors
static Box bound(const Vec *pos, const int len)
{
    Vec min = *pos;
    Vec max = *pos;
    for (int i = 1; i < len; ++i) {
        ++pos;
        if (pos->x < min.x) min.x = pos->x;
        if (pos->y < min.y) min.y = pos->y;
        if (pos->x > max.x) max.x = pos->x;
        if (pos->y > max.y) max.y = pos->y;
    }
    return (Box){{min.x, min.y}, {max.x - min.x + 1, max.y - min.y + 1}};
}

// Go to initial position at time t0 in one big step
static Box init(Vec *pos, const int t0, const int len)
{
    for (int i = 0; i < len; ++i) {
        pos[i].x += vel[i].x * t0;
        pos[i].y += vel[i].y * t0;
    }
    return bound(pos, len);
}

static Box step(Vec *nextpos, const Vec *curpos, const int dt, const int len)
{
    for (int i = 0; i < len; ++i) {
        nextpos[i].x = curpos[i].x + vel[i].x * dt;
        nextpos[i].y = curpos[i].y + vel[i].y * dt;
    }
    return bound(nextpos, len);
}

int main(void)
{
    // Read input file into globals pos0[] and vel[]
    int n = 0;
    if ((n = parse(pos0, N)) < N)
        return 1;

#ifdef TIMER
    starttimer();
#endif

    int t = guess(pos0, M);  // for M=3, this is exactly right for my input
    Box bestbox = init(pos0, t, n);

    // Single time steps backward while better, then forward while better
    Vec *bestpos = pos0;
    Vec *nextpos = pos1;
    for (int dt = -1; dt < 2; dt += 2) {
        bool smaller;
        do {
            Box nextbox = step(nextpos, bestpos, dt, n);
            if ((smaller = nextbox.range.x < bestbox.range.x || nextbox.range.y < bestbox.range.y)) {
                t += dt;
                bestbox = nextbox;
                Vec *tmp = bestpos;
                bestpos = nextpos;
                nextpos = tmp;
            }
        } while (smaller);
    }

    // Rebase to zero using the saved minimal coordinates
    for (int i = 0; i < n; ++i) {
        bestpos[i].x -= bestbox.corner.x;
        bestpos[i].y -= bestbox.corner.y;
    }

    // Display results
    memset(matrix, ' ', sizeof matrix);
    for (int i = 0; i < n; ++i)
        matrix[bestpos[i].y][bestpos[i].x] = '#';  // ERCXLAJL
    for (int i = 0; i < H; ++i)
        printf("%.*s\n", W, matrix[i]);
    printf("%d\n", t);  // 10813

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
