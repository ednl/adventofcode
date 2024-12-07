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
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
#include <stdint.h>  // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-07-example.txt"
    #define N 9  // lines in input file
#else
    #define FNAME "../aocinput/2024-07-input.txt"
    #define N 850  // lines in input file
#endif
#define M 12  // max number count to combine

typedef struct equation {
    int64_t test;
    int testlen, count;
    int num[M], len[M];
} Equation;

static const int pow3[M] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147};
static const int64_t pow10[] = {1LL, 10LL, 100LL, 1000LL, 10000LL, 100000LL, 1000000LL,
    10000000LL, 100000000LL, 1000000000LL, 10000000000LL, 100000000000LL, 1000000000000LL,
    10000000000000LL, 100000000000000LL, 1000000000000000LL, 10000000000000000LL,
    100000000000000000LL, 1000000000000000000LL
};

static Equation equation[N];

static int intlen(int64_t x)
{
    if (x < 0) return 0;
    if (!x) return 1;
    int len = 1;
    x /= 10;
    while (x % 10) {
        ++len;
        x /= 10;
    }
    return len;
}

static int64_t add(const int64_t a, const int64_t b)
{
    return a + b;
}

static int64_t mul(const int64_t a, const int64_t b)
{
    return a * b;
}

static int64_t cat(const int64_t a, const int64_t b)
{
    return a + b;
}

static inline int64_t reduce2(const int *const num, const int count, int pat)
{
    int64_t r = num[0];
    for (int i = 1; i < count; ++i, pat >>= 1)
        if (pat & 1) r *= num[i];
        else         r += num[i];
    return r;
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Parse input file
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    char buf[BUFSIZ];

    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        const char *c = buf;
        Equation *const eq = &equation[i];
        eq->test = *c++ & 15;
        while (*c != ':')
            eq->test = eq->test * 10 + (*c++ & 15);
        c++;
        while (*c++ == ' ' && eq->count < M) {
            int x = *c++ & 15;
            while (*c >= '0' && *c <= '9')
                x = x * 10 + (*c++ & 15);
            eq->num[eq->count++] = x;
        }
    }
    fclose(f);

    // Part 1
    int64_t cal1 = 0, cal2 = 0;
    for (int i = 0; i < N; ++i) {
        const int64_t test = equation[i].test;
        const int count = equation[i].count;
        const int *const num = equation[i].num;
        const int end = 1 << (count - 1);
        bool found = false;
        for (int pat = 0; pat < end; ++pat) {
            if (reduce2(num, count, pat) == test) {
                found = true;
                break;
            }
        }
        if (found) {
            cal1 += test;
            cal2 += test;
        } else {
            //TODO
        }
    }
    printf("%"PRId64" %"PRId64"\n", cal1, cal2);  // test: 3749 11387, input: 303766880536 ?

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
