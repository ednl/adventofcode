#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define SHOW 0

#if !SHOW
#include "startstoptimer.h"
#endif

#define EXAMPLE 0

#if EXAMPLE
#define NAME "../aocinput/2018-12-example.txt"
#else
#define NAME "../aocinput/2018-12-input.txt"
#endif

#define BITS  5
#define RULES (1 << BITS)
#define FIRST (BITS - 1)  // index from left in bitfield where the first bit is set
#define GEN1  20
#define GEN2  (50LL * 1000 * 1000 * 1000)

static __uint128_t pots;
static bool rule[RULES];

static bool read(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return false;
    // Initial potted plant pattern
    int c = 0;
    __uint128_t setbit = (__uint128_t)1 << (127 - FIRST);  // start at index 4 from left
    while (c != '.' && c != '#')
        c = fgetc(f);
    while ((c == '#' || c == '.')) {
        if (c == '#')
            pots |= setbit;
        c = fgetc(f);
        setbit >>= 1;
    }
    fgetc(f);  // empty line
    // Rules
    while (!feof(f)) {
        int r = 0;
        for (int i = 0; i < BITS; ++i)
            r = (r << 1) | (fgetc(f) == '#');
        for (int j = 0; j < 4; ++j)
            fgetc(f);  // ' => '
        rule[r] = fgetc(f) == '#';
        fgetc(f);  // newline
    }
    fclose(f);
    return true;
}

#if SHOW == 1
static void show(const int gen, const int val)
{
    __uint128_t testbit = (__uint128_t)1 << 127;
    if (gen >= 0)
        printf("%3d: ", gen);
    while (testbit) {
        printf("%c", pots & testbit ? '#' : '.');
        testbit >>= 1;
    }
    if (val >= 0)
        printf(" %d", val);
    putchar('\n');
}
#endif

// Return true if new pattern is different
static bool evolve(int * const value)
{
    static int zero = FIRST;  // pot 0 is initially at index 4 from left
    int sum = 0, k = 0;  // k is index of first bit from left that is set
    bool first = true;
    __uint128_t nextgen = 0;
    __uint128_t get = (__uint128_t)31 << (128 - BITS);  // shift rightmost 5 bits all the way left
    __uint128_t set = (__uint128_t)1 << (127 - 2);  // bit 125 = index 2
    for (int i = 2; i <= 125; ++i) {
        // Get current packet of 5 bits, test if corresponding rule says to spawn a plant here
        if (rule[(pots & get) >> (125 - i)]) {
            nextgen |= set;  // fill pot with plant
            sum += i - zero;
            if (first) {
                k = i;  // keep track of first bit that is set
                first = false;
            }
        }
        get >>= 1;
        set >>= 1;
    }
    *value = sum;
    if (!first) {  // any bits set?
        // Shift pattern so that leftmost bit that is set, is at index 4 from left
        int shift = FIRST - k;  // target index = 4
        zero += shift;  // remember new origin
        if (shift >= 0)
            nextgen >>= shift;
        else
            nextgen <<= -shift;
    }
    if (nextgen == pots)
        return false;  // no evolution
    pots = nextgen;  // set new potted plant pattern
    return true;
}

int main(void)
{
#if !SHOW
    starttimer_q();  // don't warn about performance mode on RPi
#endif

    if (!read()) {
        fputs("File not found.", stderr);
        return 1;
    }

    // Part 1
    int gen = 0, val = 0;
    while (gen < GEN1) {
        evolve(&val);
        ++gen;
#if SHOW
        show(gen, val);
#endif
    }
    printf("Part 1: %d\n", val);  // part 1: example = 325, puzzle = 3494

    // Part 2
    int prev = val;
    while (evolve(&val)) {
        ++gen;
        prev = val;
#if SHOW
        show(gen, val);
#endif
    }
#if SHOW
    show(gen + 1, val);  // identical pattern to previous gen
#endif
    printf("Part 2: %lld\n", (GEN2 - gen) * (val - prev) + prev);  // part 2: example = 999999999374, puzzle = 2850000002454

#if !SHOW
    printf("Time: %.1f us\n", stoptimer_us());
#endif
    return 0;
}
