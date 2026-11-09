/**
 * Advent of Code 2025
 * Day 9: Movie Theater
 * https://adventofcode.com/2025/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 09.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  6.35 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  9.12 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 17.6  µs
 */

#include <stdio.h>
#include <stdlib.h>    // abs
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-09-input.txt"
#define N 512  // points, needed for my input: 496
#define FSIZE (512 * 2 * (5 + 1))  // max 5 digits, needed for my input: 5766

typedef struct vec {
    int x, y;
} Vec;

static char input[FSIZE];

// Red tiles are consecutive nodes of a closed, non-intersecting path
// with alternating horizontal and vertical edges
static Vec red[N];

static int readnum(const char **const s)
{
    int x = 0;
    while (**s & 16)  // until comma or newline
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip comma or newline
    return x;
}

// Integer rectangle area with borders, from any two points
static int64_t rectarea(const Vec a, const Vec b)
{
    return (int64_t)(abs(a.x - b.x) + 1) * (abs(a.y - b.y) + 1);
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME"\n", stderr); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int n = 0;
    for (const char *s = input; *s; n++) {
        red[n].x = readnum(&s);
        red[n].y = readnum(&s);
    }

    // Part 1
    // Looked at data: mostly a circle, so try opposite points
    // with arbitrary margin (bandwidth)
    const int bw = 32;
    int64_t largest = 0;
    for (int i = bw, k = i + (n - bw) / 2; i < n; ++i) {
        for (int j = i - bw; j < i; ++j) {
            const int64_t area = rectarea(red[j], red[k]);
            if (area > largest)
                largest = area;
        }
        if (++k == n)
            k = 0;
    }
    printf("%"PRIu64" ", largest);  // 4739623064

    // Part 2
    // Looked at data: large jump in x for two points, making a gap into a circle
    // Vec corner1 = red[248];  // large jump to the right from prev to here
    // Vec corner2 = red[249];  // large jump to the left from here to next, dy<0 from corner1
    largest = 0;
    {
        const int k = 249;
        const Vec a = red[k];
        int i = k + 200;  // opposite vertical quadrant
        for (; red[i].x < a.x; ++i);
        const int ymin = red[i].y;
        // from first with dy!=0 from corner
        for (int j = k + 2; red[j].y >= ymin; ++j) {
            const int64_t area = rectarea(a, red[j]);
            if (area > largest)
                largest = area;
        }
    }
    {
        const int k = 248;
        const Vec a = red[k];
        int i = k - 200;  // opposite vertical quadrant
        for (; red[i].x < a.x; --i);
        const int ymax = red[i].y;
        // from first with dy!=0 from corner
        for (int j = k - 2; red[j].y <= ymax; --j) {
            const int64_t area = rectarea(a, red[j]);
            if (area > largest)
                largest = area;
        }
    }
    printf("%"PRIu64"\n", largest);  // 1654141440

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
