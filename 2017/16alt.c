/**
 * Advent of Code 2017
 * Day 16: Permutation Promenade
 * https://adventofcode.com/2017/day/16
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 16.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 16.c
 * Test with timer enabled, without many lines of identical output:
 *     ./a.out | tail -n2                 built-in file name
 *     ./a.out myinput.txt | tail -n2     custom file name
 *     ./a.out < myinput.txt | tail -n2   redirected input
 * Get minimum runtime from timer output on stderr:
 *     ./a.out 2>&1 1>/dev/null
 * Minimum runtime measurements, includes parsing and output:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.54 ms
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? ms
 *     Raspberry Pi 5 (2.4 GHz)      :    ? ms
 */

#include <stdio.h>
#include <unistd.h>  // fileno, isatty
#include <stdint.h>  // uint64_t
#ifdef TIMER
    #include "../startstoptimer.h"
    #define LOOPS 5000
#endif

#define FNAME "../aocinput/2017-16-input.txt"
#define FSIZE (48 * 1024)  // needed for my input: 48522
#define MOVES (10 * 1000)  // comma-separated fields in my input
#define CYCLE 127          // needed for my input: 60
#define BILLION (1000 * 1000 * 1000)  // part 2

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
static Move move[MOVES];
static const Move *endmove = move;
static uint64_t state[CYCLE + 1];

// Move bits from back to front (shl: 4..60, shr: 64-shr)
static uint64_t spin(const uint64_t x, const int shl, const int shr)
{
    return (x << shl) | (x >> shr);
}

// Swap nibbles at 2 different positions (0..15) where pos1 < pos2
static uint64_t exchange(const uint64_t x, const int pos1, const int pos2)
{
    const int shift = (pos2 - pos1) << 2;  // x4: from nibbles to bits
    const uint64_t base = x & clr[pos1] & clr[pos2];
    const uint64_t val1 = (x & sel[pos1]) << shift;
    const uint64_t val2 = (x & sel[pos2]) >> shift;
    return base | val1 | val2;
}

// Nibble index (0..15) of nibble value (0..15)
static int nibpos(uint64_t x, const int val)
{
    int nib = 0;
    for (; (x & sel[0]) != val; x >>= 4, nib++);
    return nib;
}

// Swap nibbles with 2 different values (0..15)
static uint64_t partner(const uint64_t x, const int val1, const int val2)
{
    int pos1 = nibpos(x, val1);
    int pos2 = nibpos(x, val2);
    if (pos1 > pos2) {
        const int tmp = pos1;
        pos1 = pos2;
        pos2 = tmp;
    }
    return exchange(x, pos1, pos2);
}

static int readnum(const char **s)
{
    int x = 0;
    while (**s & 16)  // until slash, comma or newline where 16-bit is not set
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip slash/comma/newline
    return x;
}

static void showline(uint64_t x)
{
    for (int i = 0; i < 16; i++, x >>= 4)
        putchar('a' + (x & sel[0]));
    putchar('\n');
}

static uint64_t dance(uint64_t x)
{
    for (const Move *m = move; m != endmove; ++m)
        x = m->fun(x, m->a, m->b);
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
    int moves = 0;
    for (const char *c = input; c != endinput && moves < MOVES; ) {
        switch (*c++) {  // skip p/s/x
        case 'p':
            move[moves++] = (Move){partner, *c - 'a', *(c + 2) - 'a'};
            c += 4;  // skip char, slash, char, comma/newline
            break;
        case 's': {
                const int bits = readnum(&c) << 2;  // x4: from nibbles to bits
                move[moves++] = (Move){spin, bits, 64 - bits};
            } break;
        case 'x': {
                int pos1 = readnum(&c);
                int pos2 = readnum(&c);
                if (pos1 > pos2) {
                    const int tmp = pos1;
                    pos1 = pos2;
                    pos2 = tmp;
                }
                move[moves++] = (Move){exchange, pos1, pos2};
            } break;
        }
    }
    endmove = move + moves;

    // Part 1
    state[0] = 0xfedcba9876543210ULL;
    state[1] = dance(state[0]);
    showline(state[1]);  // cgpfhdnambekjiol

    // Part 2
    int cycle = 1;
    for (; cycle < CYCLE && state[cycle] != state[0]; ++cycle)
        state[cycle + 1] = dance(state[cycle]);
    showline(state[BILLION % cycle]);  // gjmiofcnaehpdlbk

#ifdef TIMER
        const double looptime = stoptimer_us();
        if (looptime < mintime)
            mintime = looptime;
    }
    fprintf(stderr, "Time: %.0f us\n", mintime);
#endif
}
