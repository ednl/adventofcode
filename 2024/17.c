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
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 9.1 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : ? µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t, UINT64_C
#include <inttypes.h>  // PRIu64, PRIo64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

typedef uint64_t u64;
static const char *input = "2,4,1,2,7,5,4,7,1,3,5,5,0,3,3,0";
static u64 prog;

// Output prog value as octal with digits separated by commas.
static char *out(u64 x)
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
static u64 inp(const char *csv)
{
    u64 x = *csv++ & 15;
    while (*csv++ == ',')
        x = x << 3 | (*csv++ & 15);
    return x;
}

// prog: 2,4,1,2,7,5,4,7,1,3,5,5,0,3,3,0
static u64 run(u64 a)
{
    u64 x = 0;
    u64 b = 0, c = 0;
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
static u64 rev(const u64 a, int len)
{
    const u64 mask = (UINT64_C(1) << (len * 3)) - 1;
    for (unsigned i = 0; i < 8; ++i) {
        const u64 try = a << 3 | i;
        const u64 res = run(try);
        if ((prog & mask) == res) {
            if (prog == res)
                return try;
            const u64 next = rev(try, len + 1);
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
    prog = inp(input);
    printf("%"PRIu64"\n", rev(0, 1));  // 37221274271220

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    return 0;
}
