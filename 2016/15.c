/**
 * Advent of Code 2016
 * Day 15: Timing is Everything
 * https://adventofcode.com/2016/day/15
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 15.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 15.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.75 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-15-input.txt"
#define DISCS 6  // 6 discs in input file

typedef struct bezout {
    int64_t m1, m2;
} Bezout;

typedef struct disc {
    int64_t id, size, pos, rem;
} Disc;

static Disc disc[DISCS];

// Disc remainder depends on its ID (=vertical position), position (=initial rotation)
static int64_t rem(const int64_t id, const int64_t pos)
{
    return -(id + pos);
}

#ifdef DEBUG
static void show(int n)
{
    putchar('\n');
    for (int i = 0; i < n; ++i) {
        const Disc * const d = &disc[i];
        printf("disc %"PRId64" size %2"PRId64" pos %2"PRId64
            "  =>  (t+%"PRId64"+%2"PRId64") %% %2"PRId64" = 0"
            "  =>  t = %3"PRId64" %% %2"PRId64
            "  =>  t \u2261 %2"PRId64" (mod %2"PRId64")\n",
            d->id, d->size, d->pos,
            d->id, d->pos, d->size,
            d->rem, d->size,
            (d->rem % d->size + d->size) % d->size, d->size);
    }
    putchar('\n');
}
#endif

// Bézout coefficients of two numbers a,b
// Ref.: https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Computation
//       https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Pseudocode
static Bezout bezout(const int64_t a, const int64_t b)
{
    int64_t q, tmp;
    int64_t r0 = a, r = b;
    int64_t s0 = 1, s = 0;
    int64_t t0 = 0, t = 1;

    while (r) {
        q = r0 / r;
        tmp = r0 - q * r; r0 = r; r = tmp;
        tmp = s0 - q * s; s0 = s; s = tmp;
        tmp = t0 - q * t; t0 = t; t = tmp;
    }
    return (Bezout){s0, t0};
}

// Reduce two congruence pairs of the same variable to one
// Ref.: https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Existence_(constructive_proof)
static void reduce(int64_t *const a1, int64_t *const n1, const int64_t a2, const int64_t n2)
{
    Bezout bz = bezout(*n1, n2);  // Bézout coefficients of two moduli n1,n2
    int64_t n = *n1 * n2;  // new modulus
    int64_t a = (*a1 * bz.m2 * n2 + a2 * bz.m1 * *n1) % n;  // new remainder
    if (a < 0)  // times can't be negative
        a += n;
    *a1 = a;
    *n1 = n;
}

int main(void)
{
    // Parse input file
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    int n = 0;
    int id, size, pos;
    while (n < DISCS && fscanf(f, "Disc #%d has %d positions; at time=0, it is at position %d. ", &id, &size, &pos) == 3)
        disc[n++] = (Disc){id, size, pos, rem(id, pos)};
    fclose(f);

#ifdef DEBUG
    show(n);
#endif
#ifdef TIMER
    starttimer();
#endif

    // Part 1
    for (int i = 1; i < n; ++i)
        reduce(&disc[0].rem, &disc[0].size, disc[i].rem, disc[i].size);
#ifdef DEBUG
    printf("Part 1: t \u2261 %"PRId64" (mod %"PRId64")\n", disc[0].rem, disc[0].size);  // 400589 (mod 440895)
#else
    printf("%"PRId64"\n", disc[0].rem);  // 400589
#endif

    // Part 2: add 1 disc of size 11 at position 0
    reduce(&disc[0].rem, &disc[0].size, rem(n + 1, 0), 11);
#ifdef DEBUG
    printf("Part 2: t \u2261 %"PRId64" (mod %"PRId64")\n", disc[0].rem, disc[0].size);  // 3045959 (mod 4849845)
#else
    printf("%"PRId64"\n", disc[0].rem);  // 3045959
#endif

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
