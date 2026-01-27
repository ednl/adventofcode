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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.417 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :     ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 2.96  µs
 */

#include <stdio.h>     // fopen, fclose, fread, FILE
#include <unistd.h>    // write, STDOUT_FILENO
#include <stdlib.h>    // div, div_t
#include <stdint.h>    // uint64_t, UINT64_C
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Input file
#define FNAME "../aocinput/2015-25-input.txt"
#define FSIZE 100
#define ROW 80  // index of start of row number in input file
#define COL 93  // index of start of col number in input file
static char input[FSIZE];

// From puzzle description
// Ref. https://adventofcode.com/2015/day/25
#define MUL UINT64_C(252533)    // multiplier (base of the modular exponentiation)
#define VAL UINT64_C(20151125)  // starting value at triangle pos (1,1) = index 0
#define MOD UINT64_C(33554393)  // modulus of the modular exponentiation

// Read unsigned int, minus 1 because row/col are one-based
static unsigned readnum(const char *s)
{
    unsigned x = 0;
    while (*s >= '0')
        x = x * 10 + (*s++ & 15);
    return x - 1;
}

static void printint(unsigned x)
{
    char buf[sizeof x * 4], *end = buf + sizeof buf, *pc = end;
    *--pc = '\n';
	do {
        const div_t qr = div(x, 10);
		*--pc = '0' + qr.rem;  // remainder of negative number is negative
		x = qr.quot;
	} while (x);
    for (ssize_t n; pc < end && (n = write(STDOUT_FILENO, pc, end - pc)) != -1; pc += n);
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
    const unsigned row = readnum(input + ROW);
    const unsigned col = readnum(input + COL);

    // Index of position on triangle grid
    const unsigned tri = row + col;
    uint64_t exp = col + (tri * (tri + 1) >> 1);

    // https://en.wikipedia.org/wiki/Modular_exponentiation
    uint64_t rem = VAL, base = MUL;
    for (; exp; exp >>= 1) {
        if (exp & 1)
            rem = rem * base % MOD;
        base = base * base % MOD;
    }

    // Solution: 19980801
    printint(rem);

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
