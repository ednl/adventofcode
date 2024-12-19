/**
 * Advent of Code 2024
 * Day 17: Chronospatial Computer
 * https://adventofcode.com/2024/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 17.c
 *    gcc   -std=gnu17 -Wall -Wextra 17.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 17.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 17.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  2.9 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  9.1 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  9.8 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 26.9 µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t, UINT64_C
#include <inttypes.h>  // PRIu64, PRIo64
#include "../aocinput/2024-17-input.h"  // input as CSV string called 'input'
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Output prog value as octal with digits separated by commas.
static char *out(uint64_t x)
{
    static char csv[64] = {0};
    char buf[32], *b = buf, *c = csv;
    snprintf(buf, sizeof buf, "%"PRIo64, x);
    *c++ = *b++;
    while (*b) {
        *c++ = ',';
        *c++ = *b++;
    }
    *c = '\0';
    return csv;
}

// Interpret CSV line as mumber with big-endian octal digits.
static uint64_t inp(const char *csv)
{
    uint64_t x = *csv++ & 15;
    while (*csv++ == ',')
        x = x << 3 | (*csv++ & 15);
    return x;
}

// Translated from input
static uint64_t run(uint64_t a)
{
    uint64_t x = 0;
    uint64_t b = 0, c = 0;
    do {
        b = a & 7;             //  0: bst A
        b ^= 2;                //  2: bxl 2
        c = a >> b;            //  4: cdv B
        b ^= c;                //  6: bxc
        b ^= 3;                //  8: bxl 3
        x = x << 3 | (b & 7);  // 10: out B
        a >>= 3;               // 12: adv 3
    } while (a);               // 14: jnz 0
    return x;
}

// Returns minimal A for which program is quine, or zero if not found.
static uint64_t rev(const uint64_t prog, const uint64_t a, const int octlen)
{
    const uint64_t mask = (UINT64_C(1) << (octlen * 3)) - 1;  // len=1: 111, len=2: 111111, etc.
    for (unsigned i = 0; i < 8; ++i) {
        const uint64_t try = a << 3 | i;
        const uint64_t res = run(try);
        if ((prog & mask) == res) {
            if (prog == res)
                return try;
            const uint64_t next = rev(prog, try, octlen + 1);
            if (next)
                return next;
        }
    }
    return 0;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    printf("%s\n", out(run(35200350)));  // 2,7,4,7,2,1,7,5,1
    printf("%"PRIu64"\n", rev(inp(input), 0, 1));  // 37221274271220

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    return 0;
}
