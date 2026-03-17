/**
 * Advent of Code 2017
 * Day 16: Permutation Promenade
 * https://adventofcode.com/2017/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 16alt.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16alt.c
 * Test with timer enabled, avoid many lines of identical output:
 *     ./a.out | tail -n2                 built-in file name
 *     ./a.out myinput.txt | tail -n2     custom file name
 *     ./a.out < myinput.txt | tail -n2   redirected input
 * Get minimum runtime from timer output on stderr:
 *     ./a.out 2>&1 1>/dev/null
 * Minimum runtime measurements, includes parsing and output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 253 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 696 µs
 */

#include <stdio.h>
#include <unistd.h>  // fileno, isatty
#include <stdint.h>  // uint64_t
#ifdef TIMER
    #include "../startstoptimer.h"
    #define LOOPS 10000  // loop time of ~250 µs => program runtime of ~2.5 s
#endif

#define FNAME "../aocinput/2017-16-input.txt"
#define FSIZE (48 * 1024)  // needed for my input: 48522
#define PERMMOVE 8192      // s+x moves, needed for my input: 7810
#define SWAPMOVE 2560      // p moves, needed for my input: 2190
#define CYCLE 127          // state array size-1, needed for my input: 15
#define DANCE (1000 * 1000 * 1000)  // multiplier for part 2
#define START 0xfedcba9876543210ULL  // programs lined up to start dancing

// Move function, applies to current state with 2 params, returns next state
typedef uint64_t (*movefun)(const uint64_t, const int, const int);

typedef struct move {
    movefun fun;
    int a, b;
} Move;

static const uint64_t sel[16] = {
    0x000000000000000fULL,
    0x00000000000000f0ULL,
    0x0000000000000f00ULL,
    0x000000000000f000ULL,
    0x00000000000f0000ULL,
    0x0000000000f00000ULL,
    0x000000000f000000ULL,
    0x00000000f0000000ULL,
    0x0000000f00000000ULL,
    0x000000f000000000ULL,
    0x00000f0000000000ULL,
    0x0000f00000000000ULL,
    0x000f000000000000ULL,
    0x00f0000000000000ULL,
    0x0f00000000000000ULL,
    0xf000000000000000ULL,
};

static const uint64_t clr[16] = {
    0xfffffffffffffff0ULL,
    0xffffffffffffff0fULL,
    0xfffffffffffff0ffULL,
    0xffffffffffff0fffULL,
    0xfffffffffff0ffffULL,
    0xffffffffff0fffffULL,
    0xfffffffff0ffffffULL,
    0xffffffff0fffffffULL,
    0xfffffff0ffffffffULL,
    0xffffff0fffffffffULL,
    0xfffff0ffffffffffULL,
    0xffff0fffffffffffULL,
    0xfff0ffffffffffffULL,
    0xff0fffffffffffffULL,
    0xf0ffffffffffffffULL,
    0x0fffffffffffffffULL,
};

static char input[FSIZE];
static Move permmove[PERMMOVE];
static Move swapmove[SWAPMOVE];
static const Move *endpermmove = permmove;
static const Move *endswapmove = swapmove;
static uint64_t perm[CYCLE + 1] = {START, 0};
static uint64_t swap[CYCLE + 1] = {START, 0};

// Greatest Common Divisor
// https://en.wikipedia.org/wiki/Euclidean_algorithm
static int gcd(int a, int b)
{
    while (b) {
        const int tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}

// Least Common Multiple
// https://en.wikipedia.org/wiki/Least_common_multiple#Using_the_greatest_common_divisor
static int lcm(const int a, const int b)
{
    return a / gcd(a, b) * b;
}

// Swap if a > b
static void orderint(int *const a, int *const b)
{
    if (*a > *b) {
        const int tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

static int readnum(const char **s)
{
    int x = 0;
    while (**s & 16)  // until slash, comma or newline where 16-bit is not set
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip slash/comma/newline
    return x;
}

static void show(uint64_t x)
{
    for (int i = 0; i < 16; i++, x >>= 4)
        putchar('a' + (x & sel[0]));
    putchar('\n');
}

// Move bits from back to front (shl: 4..60, shr: 64-shr)
static uint64_t spin(const uint64_t x, const int shl, const int shr)
{
    return (x << shl) | (x >> shr);
}

// Swap nibbles at 2 different positions (0..15) where pos1 < pos2
static uint64_t xchg(const uint64_t x, const int pos1, const int pos2)
{
    const int shift = (pos2 - pos1) << 2;  // x4: from nibbles to bits
    const uint64_t val1 = (x & sel[pos1]) << shift;
    const uint64_t val2 = (x & sel[pos2]) >> shift;
    const uint64_t base = x & clr[pos1] & clr[pos2];
    return base | val1 | val2;
}

// Apply all "spin" and "exchange" moves to permutate program values
static uint64_t permdance(uint64_t x)
{
    for (const Move *m = permmove; m != endpermmove; ++m)
        x = m->fun(x, m->a, m->b);
    return x;
}

// Apply all "partner" moves to swap program labels
static uint64_t swapdance(uint64_t x)
{
    for (const Move *m = swapmove; m != endswapmove; ++m)
        x = m->fun(x, m->a, m->b);
    return x;
}

// Nibble index (0..15) of nibble value (0..15)
static uint64_t nibpos(uint64_t x, const uint64_t val)
{
    uint64_t nib = 0;
    for (; (x & sel[0]) != val; x >>= 4, nib++);
    return nib;
}

// Make complete dance from "spin+exchange" state (perm) and "partner" state (swap)
static uint64_t dance(uint64_t perm, const uint64_t swap)
{
    uint64_t x = 0;
    for (int i = 0; i < 16; i++, perm >>= 4)
        x |= nibpos(swap, perm & sel[0]) << (i << 2);
    return x;
}

int main(int argc, char *argv[])
{
    size_t fsize = 0;
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        const char *fname = argc > 1 ? argv[1] : FNAME;  // file name as argument, or built-in
        FILE *f = fopen(fname, "rb");
        if (!f) {
            fprintf(stderr, "File not found: %s\n", fname);
            return 1;
        }
        fsize = fread(input, 1, FSIZE, f);
        fclose(f);
    } else
        // Read from pipe or redirected input
        fsize = fread(input, 1, FSIZE, stdin);
    // End of input file
    const char *const endinput = input + fsize;

#ifdef TIMER
    double mintime = 1e9;
    for (int loops = 0; loops < LOOPS; ++loops) {
        starttimer();
#endif

    // Parse input
    int perms = 0, swaps = 0;
    for (const char *c = input; c != endinput; ) {
        switch (*c++) {  // skip s/x/p
        case 's': {
                const int bits = readnum(&c) << 2;  // x4: from nibbles to bits
                permmove[perms++] = (Move){spin, bits, 64 - bits};
            } break;
        case 'x': {
                int pos1 = readnum(&c);
                int pos2 = readnum(&c);
                orderint(&pos1, &pos2);
                permmove[perms++] = (Move){xchg, pos1, pos2};
            } break;
        case 'p': {
                int label1 = *c - 'a';
                int label2 = *(c + 2) - 'a';
                orderint(&label1, &label2);
                swapmove[swaps++] = (Move){xchg, label1, label2};
                c += 4;  // skip char, slash, char, comma/newline
            } break;
        }
    }
    endpermmove = permmove + perms;
    endswapmove = swapmove + swaps;

    // Part 1
    perm[1] = permdance(perm[0]);
    swap[1] = swapdance(swap[0]);
    show(dance(perm[1], swap[1]));  // cgpfhdnambekjiol

    // Part 2
    int permcycle = 1;
    for (; perm[permcycle] != START && permcycle < CYCLE; permcycle++)
        perm[permcycle + 1] = permdance(perm[permcycle]);
    int swapcycle = 1;
    for (; swap[swapcycle] != START && swapcycle < CYCLE; swapcycle++)
        swap[swapcycle + 1] = swapdance(swap[swapcycle]);
    const int dancecycle = lcm(permcycle, swapcycle);  // LCM(15,12) = 60
    const int danceindex = DANCE % dancecycle;         // 1B mod 60 = 40
    const int permindex = danceindex % permcycle;      // 40 mod 15 = 10
    const int swapindex = danceindex % swapcycle;      // 40 mod 12 = 4
    show(dance(perm[permindex], swap[swapindex]));  // gjmiofcnaehpdlbk

#ifdef TIMER
        const double looptime = stoptimer_us();
        if (looptime < mintime)
            mintime = looptime;
    }
    fprintf(stderr, "Time: %.0f us\n", mintime);
#endif
}
