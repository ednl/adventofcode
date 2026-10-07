/**
 * Advent of Code 2024
 * Day 7: Bridge Repair
 * https://adventofcode.com/2024/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 07.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 07.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 116 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 251 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 460 µs
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv
#include <stdint.h>    // uint64_t, uint16_t
#include <inttypes.h>  // PRIu64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-07-input.txt"
#define FSIZE (1 << 15) // needed for my input: 25162
#define N 850  // lines in input file
#define M 12  // max number count per equation

typedef struct equation {
    uint16_t num[M];
    uint16_t mag[M];  // order of magnitude of numbers
    uint64_t test;
    unsigned last;
} Equation;

static const uint16_t mag[4] = {1, 10, 100, 1000};  // every num is less than 1000

static char input[FSIZE];
static Equation equation[N];

// Recursively reduce equation in reverse
//   res = residue, starts at test value, should end at num[0]
//   ix = index of num/mag, starts at count - 1, should end at 0
// Returns true at first solution, false if impossible
static bool reduce1(const Equation *const eq, const uint64_t res, const unsigned ix)
{
    if (ix == 0)
        return res == eq->num[0];
    // Addition
    if (res > eq->num[ix] && reduce1(eq, res - eq->num[ix], ix - 1))
        return true;
    // Multiplication
    const lldiv_t dv = lldiv(res, eq->num[ix]);
    if (!dv.rem && reduce1(eq, dv.quot, ix - 1))
        return true;
    return false;  // this branch failed
}

static bool reduce2(const Equation *const eq, const uint64_t res, const unsigned ix)
{
    if (ix == 0)
        return res == eq->num[0];
    // Addition
    if (res > eq->num[ix] && reduce2(eq, res - eq->num[ix], ix - 1))
        return true;
    // Multiplication
    const lldiv_t d1 = lldiv(res, eq->num[ix]);
    if (!d1.rem && reduce2(eq, d1.quot, ix - 1))
        return true;
    // Concatenation
    const lldiv_t d2 = lldiv(res, eq->mag[ix]);  // divide by order of magnitude of num
    if (d2.rem == eq->num[ix] && reduce2(eq, d2.quot, ix - 1))
        return true;
    return false;  // this branch failed
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, sizeof input, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    for (Equation *eq = equation; *c; ++eq) {
        uint64_t test = 0;
        while (*c != ':')
            test = test * 10 + (*c++ & 15);
        eq->test = test;
        c++;  // skip ':'
        unsigned i = 0;
        for (; *c++ & 32; ++i) {  // until newline
            unsigned num = 0, len = 0;
            for (; *c & 16; ++len)  // until space or newline
                num = num * 10 + (*c++ & 15);
            eq->num[i] = num;
            eq->mag[i] = mag[len];  // order of magnitude
        }
        eq->last = i - 1;  // last index = count-1
    }

    uint64_t cal1 = 0, cal2 = 0;
    for (int i = 0; i < N; ++i) {
        const Equation *const eq = &equation[i];
        if (reduce1(eq, eq->test, eq->last)) {
            cal1 += eq->test;
            cal2 += eq->test;
        } else if (reduce2(eq, eq->test, eq->last))
            cal2 += eq->test;
    }
    printf("%"PRIu64" %"PRIu64"\n", cal1, cal2);  // 303766880536 337041851384440

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
