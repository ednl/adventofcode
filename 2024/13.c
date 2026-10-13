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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 3.50 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv_t
#include <stdint.h>    // int64_t, INT64_C
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-13-input.txt"
#define FSIZE (3<<13) // =24576, needed for my input: 20971 +1
#define N 320  // claw machines in input file
#define MAXPRESS 100
#define CORRECTION INT64_C(10000000000000)

typedef struct vec {
    int64_t x, y;
} Vec;

// Buttons A & B directions, prize location
typedef struct claw {
    Vec a, b, p;
} Claw;

static char input[FSIZE];
static Claw claw[N];  // claw machines

static unsigned readnum(const char **s)
{
    unsigned x = 0;
    while (**s & 16)  // until ','=44 or '\n'=10
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

// Matrix equation to solve for n,m:
//   |ax bx|   |n|   |px|
//   |ay by| . |m| = |py|
// Matrix inversion, or https://en.wikipedia.org/wiki/Cramer%27s_rule :
//   |n|                           | by -bx|   |px|
//   |m| = 1/(ax . by - ay . bx) . |-ay  ax| . |py|
// =>
//   n = (by . px - bx . py) / (ax . by - ay . bx)
//   m = (ax . py - ay . px) / (ax . by - ay . bx)
// Restrictions: n,m integer >= 0. For part 1 also: n,m <= 100.
// Return: sum of all valid token counts 3n+m
static int64_t tokens(const int part)
{
    int64_t sum = 0;
    for (int i = 0; i < N; ++i) {
        const Claw *const c = &claw[i];  // convenience pointer
        // Determinant = divisor of inverted matrix. Must not be zero, but never is for my input.
        // Possibly faster to calculate only once, because it's constant between parts 1 & 2,
        // but it made no measurable difference on my Apple M1; still 5.2 µs.
        const int64_t det = c->a.x * c->b.y - c->a.y * c->b.x;
        // Divisions must have no remainders, and quot<=100 for part 1.
        // For my input, every n and m that have no remainder also have non-negative quotients,
        // (strictly positive, even) so no extra test for that.
        const lldiv_t n = lldiv(c->b.y * c->p.x - c->b.x * c->p.y, det);
        const lldiv_t m = lldiv(c->a.x * c->p.y - c->a.y * c->p.x, det);
        if (!n.rem && !m.rem && (part == 2 || (n.quot <= MAXPRESS && m.quot <= MAXPRESS)))
            sum += 3 * n.quot + m.quot;
    }
    return sum;
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

    // Example claw entry:
    // Button A: X+24, Y+19  (always 2 digits)
    // Button B: X+32, Y+86
    // Prize: X=1072, Y=2062  (3-5 digits each), followed by blank line or EOF
    const char *c = input;
    for (int i = 0; i < N; ++i) {
        claw[i].a.x = *(c + 12) * 10 + *(c + 13) - 11 * '0';
        claw[i].a.y = *(c + 18) * 10 + *(c + 19) - 11 * '0';
        claw[i].b.x = *(c + 33) * 10 + *(c + 34) - 11 * '0';
        claw[i].b.y = *(c + 39) * 10 + *(c + 40) - 11 * '0';
        c += 51;
        claw[i].p.x = readnum(&c); c += 4;
        claw[i].p.y = readnum(&c); c += 2;
    }

    // Part 1
    printf("%"PRIu64" ", tokens(1));  // 29598

    // Part 2
    // Correction for "a unit conversion error in your measurements"
    for (int i = 0; i < N; ++i) {
        claw[i].p.x += CORRECTION;
        claw[i].p.y += CORRECTION;
    }
    printf("%"PRIu64"\n", tokens(2));  // 93217456941970

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
