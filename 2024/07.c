/**
 * Advent of Code 2024
 * Day 7: Bridge Repair
 * https://adventofcode.com/2024/day/7
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 07.c
 *    gcc   -std=gnu17 -Wall -Wextra 07.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 07.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 07.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  178 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  262 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  429 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 1045 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 1070 µs (strangely slow, normally closer to Pi5)
 */

#include <stdio.h>
#include <stdlib.h>    // lldiv
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-07-example.txt"
    #define N 9  // lines in example file
#else
    #define FNAME "../aocinput/2024-07-input.txt"
    #define N 850  // lines in input file
#endif
#define M 12  // max number count per equation

typedef struct equation {
    int64_t test;
    int count;
    int num[M];
    int mag[M];  // order of magnitude of numbers
} Equation;

static Equation equation[N];

// Parse calibration equation = one line from input file
static void parseline(Equation *const eq, const char *s)
{
    static const int mag[4] = {1, 10, 100, 1000};  // every num is less than 1000
    int64_t test = *s++ & 15;
    while (*s != ':')
        test = test * 10 + (*s++ & 15);
    ++s;  // skip ':'
    eq->test = test;
    int n = 0;
    for (; *s++ == ' '; ++n) {
        int num = *s++ & 15, len = 1;
        for (; *s >= '0' && *s <= '9'; ++len)
            num = num * 10 + (*s++ & 15);
        eq->num[n] = num;
        eq->mag[n] = mag[len];  // order of magnitude
    }
    eq->count = n;
}

// Recursively reduce equation in reverse
//   res = residue, starts at test value, should end at num[0]
//   ix = index of num/mag, starts at count - 1, should end at 0
// Returns true at first solution, false if impossible
static bool reduce(const Equation *const eq, const int64_t res, const int ix, const int part)
{
    if (ix == 0)
        return res == eq->num[0];
    // Addition
    if (res > eq->num[ix] && reduce(eq, res - eq->num[ix], ix - 1, part))
        return true;
    // Multiplication
    lldiv_t dv = lldiv(res, eq->num[ix]);
    if (!dv.rem && reduce(eq, dv.quot, ix - 1, part))
        return true;
    // Concatenation
    if (part == 2) {
        dv = lldiv(res, eq->mag[ix]);  // divide by order of magnitude of num
        if (dv.rem == eq->num[ix] && reduce(eq, dv.quot, ix - 1, part))
            return true;
    }
    return false;  // this branch failed
}

int main(void)
{
    // Read & parse input file
    FILE *f = fopen(FNAME, "r");
    if (!f) return 1;
    char buf[256];
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i)
        parseline(&equation[i], buf);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int64_t cal1 = 0, cal2 = 0;
    for (int i = 0; i < N; ++i) {
        const Equation *const eq = &equation[i];
        if (reduce(eq, eq->test, eq->count - 1, 1)) {
            cal1 += eq->test;
            cal2 += eq->test;
        } else if (reduce(eq, eq->test, eq->count - 1, 2))
            cal2 += eq->test;
    }
    printf("%"PRId64" %"PRId64"\n", cal1, cal2);  // test: 3749 11387, input: 303766880536 337041851384440

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
