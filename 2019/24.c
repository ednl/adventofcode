/**
 * Advent of Code 2019
 * Day 24: The Tyranny of the Rocket Equation
 * https://adventofcode.com/2019/day/24
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 24.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 24.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.07 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    : 1.65 ms
 *     Raspberry Pi 5 (2.4 GHz)      : 3.30 ms
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2019-24-example.txt"
    #define GEN 10
#else
    #define FNAME "../aocinput/2019-24-input.txt"
    #define GEN 200
#endif
#define FSIZE 32  // >= 5*(5+1)+1
#define DIM 5
#define LEN (DIM * DIM)
#define MID ((GEN >> 1) + 1)
#define LEN2 ((MID << 1) + 1)

typedef unsigned int u32;

typedef enum index {
    ZERO = 0,
    OUT_U, OUT_D, OUT_L, OUT_R,
     IN_U,  IN_D,  IN_L,  IN_R,
    INDICES
} Index;

// Which locations to add for part 2
static const Index index[LEN][2] = {
    {OUT_U, OUT_L}, {OUT_U, ZERO }, {OUT_U, ZERO }, {OUT_U, ZERO }, {OUT_U, OUT_R},
    {ZERO , OUT_L}, {ZERO , ZERO }, { IN_D, ZERO }, {ZERO , ZERO }, {ZERO , OUT_R},
    {ZERO , OUT_L}, { IN_R, ZERO }, {ZERO , ZERO }, { IN_L, ZERO }, {ZERO , OUT_R},
    {ZERO , OUT_L}, {ZERO , ZERO }, { IN_U, ZERO }, {ZERO , ZERO }, {ZERO , OUT_R},
    {OUT_D, OUT_L}, {OUT_D, ZERO }, {OUT_D, ZERO }, {OUT_D, ZERO }, {OUT_D, OUT_R},
};

static char input[FSIZE];
static u32 seen[(1 << LEN) >> 5];
static u32 state0[LEN2];
static u32 state1[LEN2];

#if EXAMPLE
static void show(const u32 *const state, const int index0, const int index1)
{
    putchar('\n');
    for (int i = 0; i < DIM; ++i) {
        for (int j = index0; j <= index1; ++j) {
            u32 x = state[j] >> (i * DIM) & ((1U << DIM) - 1U);
            for (int k = 0; k < DIM; x >>= 1, ++k)
                if (index0 == index1 || i != (DIM >> 1) || k != (DIM >> 1))
                    putchar(x & 1 ? '#' : '.');
                else
                    putchar('?');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}
#endif

static bool isnew(const u32 state)
{
    u32 *const i = &seen[state >> 5];  // not DIM but 2^5=32 bits in u32
    const u32 bit = 1U << (state & 31U);  // 2^5-1=31
    if (*i & bit)
        return false;
    *i |= bit;
    return true;
}

static u32 evolve1(const u32 prev)
{
    const u32 U = prev >> DIM;               // row 6 (shifted in) was already zero
    const u32 D = prev << DIM & 0x01ffffe0;  // reset row 6 (and 1) = 0000 0001 1111 1111 1111 1111 1110 0000
    const u32 L = prev >> 1   & 0x00f7bdef;  // reset right column  = 0000 0000 1111 0111 1011 1101 1110 1111
    const u32 R = prev << 1   & 0x01ef7bde;  // reset left  column  = 0000 0001 1110 1111 0111 1011 1101 1110
    u32 next = 0;
    for (u32 bit = 1, i = 0; i != LEN; bit <<= 1, ++i)
        switch (((U & bit) + (D & bit) + (L & bit) + (R & bit)) >> i) {
            case 2: if (!(prev & bit))
            case 1:     next |= bit;
        }
    return next;
}

static u32 evolve2(const u32 prev, const u32 outside, const u32 inside)
{
    // Same as evolve1() but also reset centre location                v
    const u32 U = prev >> DIM & 0x01ffefff;  // 0000 0001 1111 1111 1110 1111 1111 1111
    const u32 D = prev << DIM & 0x01ffefe0;  // 0000 0001 1111 1111 1110 1111 1110 0000
    const u32 L = prev >> 1   & 0x00f7adef;  // 0000 0000 1111 0111 1010 1101 1110 1111
    const u32 R = prev << 1   & 0x01ef6bde;  // 0000 0001 1110 1111 0110 1011 1101 1110
    const u32 add[INDICES] = {
        [ZERO] = 0,
        // Add outside to border rows/cols (udlr as seen from inside)
        [OUT_U] = outside >>  7 & 1,  // above centre
        [OUT_D] = outside >> 17 & 1,  // below centre
        [OUT_L] = outside >> 11 & 1,  // left of centre
        [OUT_R] = outside >> 13 & 1,  // right of centre
        // Add inside to central locations (udlr as seen from outside)
        [IN_U] = __builtin_popcount(inside & 0x01f00000U),  // bottom row = 0000 0001 1111 0000 0000 0000 0000 0000
        [IN_D] = __builtin_popcount(inside & 0x0000001fU),  // top    row = 0000 0000 0000 0000 0000 0000 0001 1111
        [IN_L] = __builtin_popcount(inside & 0x01084210U),  // right  col = 0000 0001 0000 1000 0100 0010 0001 0000
        [IN_R] = __builtin_popcount(inside & 0x00108421U),  // left   col = 0000 0000 0001 0000 1000 0100 0010 0001
        // This takes about the same time as popcount, maaaybe a little slower
        // [IN_U] = (inside >> 20 & 1) + (inside >> 21 & 1) + (inside >> 22 & 1) + (inside >> 23 & 1) + (inside >> 24 & 1),
        // [IN_D] = (inside >>  0 & 1) + (inside >>  1 & 1) + (inside >>  2 & 1) + (inside >>  3 & 1) + (inside >>  4 & 1),
        // [IN_L] = (inside >>  4 & 1) + (inside >>  9 & 1) + (inside >> 14 & 1) + (inside >> 19 & 1) + (inside >> 24 & 1),
        // [IN_R] = (inside >>  0 & 1) + (inside >>  5 & 1) + (inside >> 10 & 1) + (inside >> 15 & 1) + (inside >> 20 & 1),
    };
    u32 next = 0;
    for (u32 bit = 1, i = 0; i != LEN; bit <<= 1, ++i) {
        const u32 nb = (((U & bit) + (D & bit) + (L & bit) + (R & bit)) >> i)
            + add[index[i][0]]
            + add[index[i][1]];
        switch (nb) {
            case 2: if (!(prev & bit))
            case 1:     next |= bit;
        }
    }
    return next;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    u32 bugs = 0, bit = 1;
    for (const char *c = input; *c; ++c)
        switch (*c) {
            case '#' : bugs |= bit; /* fall-through */
            case '.' : bit <<= 1; break;
        }
#if EXAMPLE
    show(&bugs, 0, 0);
    printf("Input: %u\n", bugs);  // 1205552
#endif

    // Part 1
    u32 state = bugs;
    while (isnew(state))
        state = evolve1(state);
#if EXAMPLE
    show(&state, 0, 0);
    printf("Part 1: ");
#endif
    printf("%u\n", state);  // example: 2129920, input: 7543003

    // Part 2
    state0[MID] = bugs;
    u32 *a = state0;
    u32 *b = state1;
    for (int i = 0; i < GEN; ++i) {
        const int spread = (i >> 1) + 1;  // 2 generations to traverse a level, +1 = new level both sides
        for (int j = MID - spread; j <= MID + spread; ++j)
            b[j] = evolve2(a[j], a[j - 1], a[j + 1]);
        u32 *const tmp = a;
        a = b;
        b = tmp;
    }
    int sum = 0;
    for (int i = 1; i < LEN2 - 1; ++i)
        sum += __builtin_popcount(a[i]);
#if EXAMPLE
    show(a, 1, LEN2 - 2);
    printf("Part 2: ");
#endif
    printf("%d\n", sum);  // example: 99, input: 1975

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
