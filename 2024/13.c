/**
 * Advent of Code 2024
 * Day 13: Claw Contraption
 * https://adventofcode.com/2024/day/13
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 13.c
 *    gcc   -std=gnu17 -Wall -Wextra 13.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 13.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 13.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 5.46 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv_t
#include <stdint.h>    // int64_t, INT64_C
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-13-example.txt"
    #define N 4
#else
    #define FNAME "../aocinput/2024-13-input.txt"
    #define N 320
#endif
#define MAXPRESS 100
#define CONVERSION INT64_C(10000000000000)

typedef struct vec {
    int64_t x, y;
} Vec;

typedef struct claw {
    Vec a, b, p;
} Claw;

static Claw claw[N];

// Equation to solve:
//   |ax bx|   |n|   |px|
//   |ay by| . |m| = |py|
// Matrix inversion:
//   |n|                        |by -bx|   |px|
//   |m| = (ax.by - ay.bx)^-1 . |-ay ax| . |py|
// =>
//   n = (by.px - bx.py) / (ax.by - ay.bx)
//   m = (ax.py - ay.px) / (ax.by - ay.bx)
// Restrictions: n,m integer >= 0. For part 1 also: n,m <= 100.
// Return: sum of all valid token counts = 3n + m
static int64_t tokens(const int part)
{
    int64_t sum = 0;
    for (int i = 0; i < N; ++i) {
        const Claw *const c = &claw[i];
    #if EXAMPLE || DEBUG
        printf("\n%3d: a(%"PRId64",%"PRId64") b(%"PRId64",%"PRId64") p(%"PRId64",%"PRId64")\n",
            i, c->a.x, c->a.y, c->b.x, c->b.y, c->p.x, c->p.y);
    #endif
        const int d = c->a.x * c->b.y - c->a.y * c->b.x;
        lldiv_t n = lldiv(c->b.y * c->p.x - c->b.x * c->p.y, d);
        lldiv_t m = lldiv(c->a.x * c->p.y - c->a.y * c->p.x, d);
        if (!n.rem && !m.rem && n.quot >= 0 && m.quot >= 0 && (part == 2 || (n.quot <= MAXPRESS && m.quot <= MAXPRESS))) {
            const int64_t tok = 3 * n.quot + m.quot;
            sum += tok;
        #if EXAMPLE || DEBUG
            printf("     n=%"PRId64" m=%"PRId64" => tok=%"PRId64" sum=%"PRId64"\n",
                n.quot, m.quot, tok, sum);
        #endif
        }
    #if EXAMPLE || DEBUG
        else
            printf("XXX  n=%"PRId64" rem %"PRId64", m=%"PRId64" rem %"PRId64"\n",
                n.quot, n.rem, m.quot, m.rem);
    #endif
    }
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    for (int i = 0; i < N; ++i)
        fscanf(f, " Button A: X+%"PRId64", Y+%"PRId64" Button B: X+%"PRId64", Y+%"PRId64" Prize: X=%"PRId64", Y=%"PRId64,
            &claw[i].a.x, &claw[i].a.y, &claw[i].b.x, &claw[i].b.y, &claw[i].p.x, &claw[i].p.y);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1
    printf("Part 1: %"PRId64"\n", tokens(1));  // example: 480, input: 29598

    // Part 2
    for (int i = 0; i < N; ++i) {
        claw[i].p.x += CONVERSION;
        claw[i].p.y += CONVERSION;
    }
    printf("Part 2: %"PRId64"\n", tokens(2));  // example: 875318608908, input: 93217456941970


#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_ns());
#endif
    return 0;
}
