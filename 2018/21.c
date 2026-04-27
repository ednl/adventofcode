/**
 * Advent of Code 2018
 * Day 21: Chronal Conversion
 * https://adventofcode.com/2018/day/21
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 21.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 21.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements, includes parsing but not reading from disk:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  42.7 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  60.8 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 135   µs
 */

#include <stdio.h>
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-21-input.txt"
#define FSIZE 390
#define SEEN (0x1000000U >> 5)  // hashes are 3 bytes, bitfield words are 32-bit

typedef unsigned int uint;

static char input[FSIZE];
static uint seen[SEEN];  // bitfield to keep track of all hashes
static uint seed, mult;  // personalised hashing parameters

// Parse unsigned int, advance char pointer
static uint parseint(const char **str)
{
    uint x = 0;
    while (**str >= '0')
        x = x * 10 + (*(*str)++ & 15);
    return x;
}

// Hashing algorithm translated from input file
static uint hash(uint prev)
{
    uint next = (seed + (prev & 0xFFU)) * mult;
    next = (next + (prev >> 8 & 0xFFU)) * mult;
    return ((next + (prev >> 16 | 1U)) * mult) & 0xFFFFFFU;
}

int main(void)
{
    // Read input file from disk, do not include in timing
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, FSIZE, 1, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int timerloop = 0; timerloop < 1000; ++timerloop) {
    memset(seen, 0, sizeof seen);  // reset for a clean start
#endif

    // Parse seed/mult parameters from input file lines 9 and 13
    const char *c = input + 6;  // skip first line
    for (int line = 0; line < 7; ++line, ++c)  // skip 7 lines
        for (c += 10; *c != '\n'; ++c);
    c += 5;  // skip opcode
    seed = parseint(&c);  // first parameter on line 9 (8th instruction)
    c += 5;  // skip parameters and newline
    for (int line = 0; line < 3; ++line, ++c)  // skip 3 lines
        for (c += 10; *c != '\n'; ++c);
    c += 7;  // skip opcode and parameter
    mult = parseint(&c);  // second parameter on line 13 (12th instruction)

    // Part 1
    uint h = hash(0);
    printf("%u\n", h);  // part 1: 3173684

    // Part 2
    // assume that part 1 hash (or 0) isn't first duplicate
    for (uint prev = h;; prev = h) {
        h = hash(h);
        const uint i = h >> 5U;
        const uint bit = 1U << (h & 31U);
        if (seen[i] & bit) {
            printf("%u\n", prev);  // part 2: 12464363
            break;
        }
        seen[i] |= bit;
    }

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
