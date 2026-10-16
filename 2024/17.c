/**
 * Advent of Code 2024
 * Day 17: Chronospatial Computer
 * https://adventofcode.com/2024/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 17.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 17.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.47 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t, UINT64_C
#include <inttypes.h>  // PRIu64, PRIo64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-17-input.txt"
#define FSIZE 256  // needed for my input: 91 +1

static char input[FSIZE];

static unsigned readnum(const char **s)
{
    unsigned x = 0;
    while (**s & 16)  // until newline
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

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
static uint64_t readcsv(const char *s)
{
    uint64_t x = *s++ & 15;
    while (*s++ == ',')
        x = x << 3 | (*s++ & 15);
    return x;
}

// Program manually translated from input
static uint64_t run(uint64_t a)
{
    uint64_t x = 0, b = 0, c = 0;
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

// Return minimal A for which program is quine, or zero if not found.
// Ref. https://en.wikipedia.org/wiki/Quine_(computing)
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

    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *s = input + 12;  // skip "Register A: "
    const unsigned regA = readnum(&s);
    s += 39;  // skip until CSV
    const uint64_t prog = readcsv(s);

    // 2,7,4,7,2,1,7,5,1 37221274271220
    printf("%s %"PRIu64"\n", out(run(regA)), rev(prog, 0, 1));

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
