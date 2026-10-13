/**
 * Advent of Code 2024
 * Day 13: Claw Contraption
 * https://adventofcode.com/2024/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 13alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 13alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.44 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv_t
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-13-input.txt"
#define FSIZE (3<<13)         // 24576, needed for my input: 20971 +2
#define CLAWS 320             // claw machines in input file
#define PRESS 100             // part 1 condition
#define CORR  10000000000000  // part 2 change

typedef struct vec {
    int64_t x, y;
} Vec;

static char input[FSIZE];

static unsigned readnum(const char **s)
{
    unsigned x = 0;
    while (**s & 16)  // until ','=44 or '\n'=10
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

static void add_r(Vec *const a, const Vec b)
{
    a->x += b.x;
    a->y += b.y;
}

static Vec tokens(const char **s)
{
    int64_t ax = *(*s + 12) * 10 + *(*s + 13) - 11 * '0';
    int64_t ay = *(*s + 18) * 10 + *(*s + 19) - 11 * '0';
    int64_t bx = *(*s + 33) * 10 + *(*s + 34) - 11 * '0';
    int64_t by = *(*s + 39) * 10 + *(*s + 40) - 11 * '0';
    *s += 51;
    int64_t px = readnum(s); *s += 4;
    int64_t py = readnum(s); *s += 2;

    int64_t det = ax * by - ay * bx;
    lldiv_t n = lldiv(by * px - bx * py, det);
    lldiv_t m = lldiv(ax * py - ay * px, det);
    int64_t part1 = !n.rem && !m.rem && n.quot <= PRESS && m.quot <= PRESS ? 3 * n.quot + m.quot : 0;

    px += CORR;
    py += CORR;
    n = lldiv(by * px - bx * py, det);
    m = lldiv(ax * py - ay * px, det);
    int64_t part2 = !n.rem && !m.rem ? 3 * n.quot + m.quot : 0;

    return (Vec){part1, part2};
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    Vec sum = {0};
    const char *s = input;
    for (int i = 0; i < CLAWS; ++i)
        add_r(&sum, tokens(&s));
    printf("%"PRIu64" %"PRIu64"\n", sum.x, sum.y);  // 29598 93217456941970

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
