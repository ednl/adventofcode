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
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 10 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 21 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :  ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 54 µs
 */

#include <stdio.h>
#include <stdint.h>    // uint64_t, UINT64_C
#include <inttypes.h>  // PRIu64, PRIo64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

typedef uint64_t u64;
typedef struct prog {
    u64 val;
    int len;
} Prog;

static const char *input = "2,4,1,2,7,5,4,7,1,3,5,5,0,3,3,0";

// Output prog.val as octal with digits separated by commas.
static char *out(Prog p)
{
    static char csv[64] = {0};
    char buf[32], *b = buf, *c = csv;
    snprintf(buf, sizeof buf, "%"PRIo64, p.val);
    if (*b)
        *c++ = *b++;
    while (*b) {
        *c++ = ',';
        *c++ = *b++;
    }
    *c = '\0';
    return csv;
}

// Interpret CSV line as mumber with big-endian octal digits.
static Prog inp(const char *csv)
{
    Prog prog = {0};
    if (csv && *csv) {
        prog.val = *csv++ & 15;
        prog.len = 1;
        while (*csv++ == ',' && prog.len < 21) {
            prog.val = prog.val << 3 | (*csv++ & 15);
            prog.len++;
        }
    }
    return prog;
}

// prog: 2,4,1,2,7,5,4,7,1,3,5,5,0,3,3,0
static Prog run(u64 a)
{
    Prog p = {0};
    u64 b = 0, c = 0;
    do {
        b = a & 7;                     //  0: bst A
        b ^= 2;                        //  2: bxl 2 (B=0..7 -> 2,3,0,1,6,7,4,5)
        c = a >> b;                    //  4: cdv B
        b ^= c;                        //  6: bxc
        b ^= 3;                        //  8: bxl 3 (B=0..7 -> 3,2,1,0,7,6,5,4)
        p.val = p.val << 3 | (b & 7);  // 10: out B
        p.len++;
        a >>= 3;                       // 12: adv 3
    } while (a);                       // 14: jnz 0
    return p;
}

// Returns minimal A for which program is quine, or zero if not found.
static u64 rev(const Prog goal)
{
    Prog stack[16] = {(Prog){0, 1}};  // a=0, len=1 (size needed for my input: 10)
    int sp = 1;
    u64 min = -1;
    while (sp) {
        const Prog a = stack[--sp];
        const u64 mask = (UINT64_C(1) << (a.len * 3)) - 1;
        for (unsigned i = 0; i < 8; ++i) {
            const u64 try = a.val << 3 | i;
            const Prog res = run(try);
            if ((goal.val & mask) == res.val) {
                if (goal.len > res.len)
                    stack[sp++] = (Prog){try, a.len + 1};
                else if (goal.len == res.len && try < min)
                    min = try;
            }
        }
    }
    return min;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    printf("%s\n", out(run(35200350)));  // 2,7,4,7,2,1,7,5,1
    printf("%"PRIu64"\n", rev(inp(input)));  // 37221274271220

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
