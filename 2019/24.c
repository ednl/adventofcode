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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 11 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 19 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 46 µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-24-input.txt"
#define FSIZE 32  // >= 5*(5+1)+1
#define DIM 5
#define LEN (DIM * DIM)

typedef unsigned int u32;

static char input[FSIZE];
static u32 seen[(1 << LEN) >> 5];

static bool isnew(const u32 state)
{
    u32 *const i = &seen[state >> 5];
    const u32 bit = 1U << (state & 31U);
    if (*i & bit)
        return false;
    *i |= bit;
    return true;
}

static u32 evolve(const u32 prev)
{
    const u32 U = prev >> DIM;               // row 6 (shifted in) was already zero
    const u32 D = prev << DIM & 0x01ffffe0;  // reset row 6 (and 1) = 0000 0001 1111 1111 1111 1111 1110 0000
    const u32 L = prev >> 1   & 0x00f7bdef;  // reset right column  = 0000 0000 1111 0111 1011 1101 1110 1111
    const u32 R = prev << 1   & 0x01ef7bde;  // reset left  column  = 0000 0001 1110 1111 0111 1011 1101 1110
    u32 next = 0;
    for (u32 i = 0, bit = 1; i != LEN; bit <<= 1, ++i) {
        const u32 nb = ((U & bit) + (D & bit) + (L & bit) + (R & bit)) >> i;
        if (nb == 1 || (nb == 2 && !(prev & bit)))
            next |= bit;
    }
    return next;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    fread(input, 1, FSIZE, f);
    fclose(f);

    starttimer();

    u32 state = 0, bit = 1;
    for (const char *c = input; *c; ++c)
        switch (*c) {
            case '#' : state |= bit; /* fall-through */
            case '.' : bit <<= 1; break;
        }

    // Part 1
    while (isnew(state))
        state = evolve(state);
    printf("%u\n", state);  // example: 2129920, input: 7543003

    printf("Time: %.0f us\n", stoptimer_us());
}
