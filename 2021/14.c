/**
 * Advent of Code 2021
 * Day 14: Extended Polymerization
 * https://adventofcode.com/2021/day/14
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 14.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 14.c
 * Test output with timer enabled:
 *     ./a.out | tail -n7
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.40 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

// Solution idea: treat every transformation rule as replacing 1 pair of elements
// with 2 pairs of elements. E.g. a rule given as AB->C becomes: AB->(AC,CB). So,
// n pairs of AB transform into n pairs of AC plus n pairs of CB. Element C gets
// counted double, but so do A and B because the original pairs overlap. Only the
// two single elements at the head and tail of the original polymer are not, so
// add them later and divide the totals by 2.
// In the whole input file only 10 different elements are present, and there are
// 100 tranformation rules, so every possible pair always has a corresponding
// rule. This simplifies the lookup and counting process, since a result is
// guaranteed and one pair will always be split in two.

#include <stdio.h>
#include <string.h>    // memcpy, memset
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#include <limits.h>    // INT64_MAX
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-14-input.txt"
#define FSIZE 1024  // needed for my input: 822
#define RULES 100
#define ALPH (('Z' & 31) + 1)  // 'A'=1,..,'Z'=26 => length=26+1
#define INISIZE 20  // length of polymer on line 1 of input file
#define GROW1 10  // growth cycles part 1
#define GROW2 40  // growth cycles part 2

static char input[FSIZE];
static uint8_t node[RULES][2];    // 2-letter nodes (values: 1..26)
static uint8_t insert[RULES];     // letter to insert (value: 1..26)
static uint8_t rule[ALPH][ALPH];  // rule index 0..99 per 2-letter node
static uint8_t lhs[RULES];        // node index 0..99 for LHS transformation
static uint8_t rhs[RULES];        // node index 0..99 for RHS transformation
static uint64_t count[RULES];     // node count
static uint64_t tmp[RULES];       // skratch area for updating all counts simultaneously

// Swap pointers (not values)
static void swap(uint64_t *restrict *p, uint64_t *restrict *q)
{
    uint64_t *const tmp = *p;
    *p = *q;
    *q = tmp;
}

// Number of cycles must be even
static uint64_t grow(const int cycles)
{
    uint64_t *cur = count, *next = tmp;
    for (int cycle = 0; cycle < cycles; ++cycle) {
        // Must start at zero
        memset(next, 0, sizeof count);
        // Split nodes
        for (int i = 0; i < RULES; ++i) {
            next[lhs[i]] += cur[i];
            next[rhs[i]] += cur[i];
        }
        // Update counts
        swap(&cur, &next);
    }
    uint64_t hist[ALPH] = {0};
    // Add two original elements from head and tail
    // = only ones not counted double so far
    hist[input[0] & 31] = 1;
    hist[input[INISIZE - 1] & 31]++;
    for (int i = 0; i < RULES; ++i) {
        hist[node[i][0]] += cur[i];
        hist[node[i][1]] += cur[i];
    }
    uint64_t min = INT64_MAX;
    uint64_t max = 0;
    for (int i = 'A' & 31; i < ALPH; ++i)
        if (hist[i]) {
            if (hist[i] < min) min = hist[i];
            if (hist[i] > max) max = hist[i];
        }
    return (max - min) >> 1;
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
    memset(count, 0, sizeof count);
#endif

    // Parse
    const char *c = input + INISIZE + 2;
    for (int i = 0; i < RULES; c += 8, ++i) {
        uint64_t data;
        memcpy(&data, c, sizeof data);
        data &= UINT64_C(0x001f000000001f1f);  // assumes little-endian
        memcpy(node[i], &data, 2);
        rule[data & 255][data >> 8 & 255] = i;
        insert[i] = data >> 48;
    }
    // Find LHS/RHS node indices
    for (int i = 0 ; i < RULES; ++i) {
        lhs[i] = rule[node[i][0]][ insert[i]];
        rhs[i] = rule[ insert[i]][node[i][1]];
    }
    // Count every overlapping pair from original polymer
    // This means every element is counted double, except head & tail
    for (c = input; c < input + INISIZE - 1; ++c)
        count[rule[*c & 31][*(c + 1) & 31]]++;

    printf("%"PRIu64" %"PRIu64"\n", grow(GROW1), grow(GROW2 - GROW1));  // 2194 2360298895777

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
