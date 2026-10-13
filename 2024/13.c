/**
 * Advent of Code 2024
 * Day 13: Claw Contraption
 * https://adventofcode.com/2024/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 13.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 13.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  2.15 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  3.58 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 10.92 µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv_t
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-13-input.txt"
#define FSIZE (3<<13)         // 24576, needed for my input: 20971 (+1 to avoid UB from pointer out of bounds)
#define CLAWS 320             // claw machines in input file
#define PRESS 100             // part 1 condition
#define CORR  10000000000000  // part 2 change

typedef struct vec {
    int64_t x, y;
} Vec;

typedef struct claw {
    Vec a, b;    // buttons A and B directions
    Vec p1, p2;  // prize locations parts 1 and 2
} Claw;

static char input[FSIZE];
static Claw claw[CLAWS];

static unsigned readnum(const char **s)
{
    unsigned x = 0;
    while (**s & 16)  // until ','=44 or '\n'=10
        x = x * 10 + (*(*s)++ & 15);
    return x;
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

    const char *s = input;
    for (int i = 0; i < CLAWS; ++i) {
        Claw *const c = &claw[i];  // convenience pointer
        c->a.x = *(s + 12) * 10 + *(s + 13) - 11 * '0';
        c->a.y = *(s + 18) * 10 + *(s + 19) - 11 * '0';
        c->b.x = *(s + 33) * 10 + *(s + 34) - 11 * '0';
        c->b.y = *(s + 39) * 10 + *(s + 40) - 11 * '0';
        s += 51;
        c->p1.x = readnum(&s); s += 4;
        c->p1.y = readnum(&s); s += 2;
        // Part 2: correction for "a unit conversion error in your measurements"
        c->p2.x = c->p1.x + CORR;
        c->p2.y = c->p1.y + CORR;
    }

    // Matrix equation to solve for n,m:
    //   |ax bx|   |n|   |px|
    //   |ay by| . |m| = |py|
    // Matrix inversion, or https://en.wikipedia.org/wiki/Cramer%27s_rule :
    //   |n|                           | by -bx|   |px|
    //   |m| = 1/(ax . by - ay . bx) . |-ay  ax| . |py|
    // =>
    //   n = (by . px - bx . py) / det
    //   m = (ax . py - ay . px) / det
    // Restrictions: n,m integer >= 0. For part 1 also: n,m <= 100.
    int64_t part1 = 0, part2 = 0;
    for (int i = 0; i < CLAWS; ++i) {
        const Claw *const c = &claw[i];  // convenience pointer
        // Determinant = divisor of inverted matrix, same for parts 1 and 2
        // Must not be zero, but it never is for my input
        const int64_t det = c->a.x * c->b.y - c->a.y * c->b.x;
        // Divisions must have no remainders, and quot<=100 for part 1
        // For my input, every n and m that have no remainder also have non-negative quotients,
        // (strictly positive, even) so no extra test for that
        lldiv_t n = lldiv(c->b.y * c->p1.x - c->b.x * c->p1.y, det);
        lldiv_t m = lldiv(c->a.x * c->p1.y - c->a.y * c->p1.x, det);
        if (!n.rem && !m.rem && n.quot <= PRESS && m.quot <= PRESS)
            part1 += 3 * n.quot + m.quot;
        n = lldiv(c->b.y * c->p2.x - c->b.x * c->p2.y, det);
        m = lldiv(c->a.x * c->p2.y - c->a.y * c->p2.x, det);
        if (!n.rem && !m.rem)
            part2 += 3 * n.quot + m.quot;
    }
    printf("%"PRIu64" %"PRIu64"\n", part1, part2);  // 29598 93217456941970

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
