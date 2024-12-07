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
#include <stdlib.h>    // atoll
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
#define M 12  // max number count to combine

typedef struct equation {
    int64_t test;
    int maxlen, count;
    int num[M], len[M];
    int64_t maxadd[M], maxmul[M];
} Equation;

static const int pow3[M] = {1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147};
static const int64_t pow10[] = {INT64_C(1), INT64_C(10), INT64_C(100), INT64_C(1000),
    INT64_C(10000), INT64_C(100000), INT64_C(1000000), INT64_C(10000000), INT64_C(100000000),
    INT64_C(1000000000), INT64_C(10000000000), INT64_C(100000000000), INT64_C(1000000000000),
    INT64_C(10000000000000), INT64_C(100000000000000), INT64_C(1000000000000000),
    INT64_C(10000000000000000), INT64_C(100000000000000000), INT64_C(1000000000000000000)};

static Equation equation[N];

static inline int64_t reduce2(const int *const num, const int count, int pat)
{
    int64_t r = num[0];
    for (int i = 1; i < count; ++i, pat >>= 1)
        switch (pat & 1) {
            case 0: r += num[i]; break;
            case 1: r *= num[i]; break;
        }
    return r;
}

static inline int64_t reduce3(const Equation *const eq, int pat)
{
    int64_t r = eq->num[0];
    int len = eq->len[0];
    for (int i = 1; i < eq->count; ++i, pat /= 3)
        switch (pat % 3) {
            case 0:
                if (r > eq->maxadd[i]) return 0;
                r += eq->num[i];
                break;
            case 1:
                if (r > eq->maxmul[i]) return 0;
                r *= eq->num[i];
                break;
            case 2:
                len += eq->len[i];
                if (len > eq->maxlen) return 0;
                r = r * pow10[eq->len[i]] + eq->num[i];
                break;
        }
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
        Equation *const eq = &equation[i];
        const char *c = buf;
        eq->test = *c++ & 15;
        int len = 1;
        while (*c != ':') {
            eq->test = eq->test * 10 + (*c++ & 15);
            ++len;
        }
        eq->maxlen = len;
        ++c;  // skip ':'
        while (*c++ == ' ' && eq->count < M) {
            int x = *c++ & 15;
            len = 1;
            while (*c >= '0' && *c <= '9') {
                x = x * 10 + (*c++ & 15);
                ++len;
            }
            eq->num[eq->count] = x;
            eq->len[eq->count] = len;
            eq->maxadd[eq->count] = eq->test - x;
            eq->maxmul[eq->count] = eq->test / x;
            eq->count++;
        }
    }
    fclose(f);

    int64_t cal1 = 0, cal2 = 0;
    for (int i = 0; i < N; ++i) {
        const Equation *const eq = &equation[i];
        const int end2 = 1 << (eq->count - 1);
        for (int pat = 0; pat < end2; ++pat) {
            if (reduce2(eq->num, eq->count, pat) == eq->test) {
                cal1 += eq->test;
                cal2 += eq->test;
                goto next_i;  // break + continue
            }
        }
        const int end3 = pow3[eq->count - 1];
        for (int pat = 0; pat < end3; ++pat)
            if (reduce3(eq, pat) == eq->test)
                cal2 += eq->test;
    next_i:;
    }
    printf("%"PRId64" %"PRId64"\n", cal1, cal2);  // test: 3749 11387, input: 303766880536 (351666462220445 too high)

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
