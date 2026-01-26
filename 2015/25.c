/**
 * Advent of Code 2015
 * Day 25: Let It Snow
 * https://adventofcode.com/2015/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 25.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 25.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.63 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 1.08 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 5.17 µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t, UINT64_C
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2015-25-input.txt"
#define FSIZE 128
static char input[FSIZE];

// From puzzle description
// Ref. https://adventofcode.com/2015/day/25
#define VAL 20151125  // starting value at grid pos (1,1) = index 0
#define MUL 252533    // multiplier (base of the modular exponentiation)
#define MOD 33554393  // modulus of the modular exponentiation

// Skip to next digit, read unsigned int, advance char pointer
static int readnum(const char **s)
{
    int x = 0;
    for (; **s && (**s < '0' || **s > '9'); ++(*s));
    for (; **s >= '0' && **s <= '9'; ++(*s))
        x = x * 10 + (**s & 15);
    return x;
}

// In the puzzle, row/col are one-based
static int getindex(const int row, const int col)
{
    const int tri = row + col - 2;  // triangle base is zero-based
    return col - 1 + tri * (tri + 1) / 2;  // number of spots in de triangle
}

// Modular exponentiation: calculate remainder r = b^e mod m
// Ref. https://en.wikipedia.org/wiki/Modular_exponentiation
// Return u64 instead of u32 to avoid overflow in next calculation
static uint64_t modpow(uint64_t base, uint64_t exponent, const uint64_t modulus)
{
    if (modulus == 1)
        return 0;  // shortcut
    if (modulus == 0 || modulus > (UINT64_C(1) << 32))
        return -1;  // error (but also maximum unsigned value)
    uint64_t rem = 1;
    base %= modulus;  // now: base <= modulus-1
    for (; exponent; exponent >>= 1) {
        if (exponent & 1)  // current LSB set?
            rem = (rem * base) % modulus;  // parentheses for clarity
        base = (base * base) % modulus;  // overflow if base >= 1<<32
    }
    return rem;  // 0 <= r < 1<<32 because modulus <= 1<<32
}

int main(void)
{
    // Read input
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse input
    const char *c = input;
    const int row = readnum(&c);
    const int col = readnum(&c);

    // Part 1
    const int pos = getindex(row, col);  // destination pos (exponent of the mod. exp.)
    printf("%"PRIu64"\n", modpow(MUL, pos, MOD) * VAL % MOD);  // 19980801

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
