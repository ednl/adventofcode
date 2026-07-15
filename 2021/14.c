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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.39 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 3.59 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 9.10 µs
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
#define POLYMER 20  // length of polymer on line 1 of input file
#define RULES  100   // number of lines with rules like "XY -> Z"
#define GROW1   10  // growth cycles part 1
#define GROW2   40  // growth cycles part 2
#define ALPH  (('Z' & 31) + 1)  // array length 26+1=27 (index 0 unused)

static char input[FSIZE];
static uint8_t node[RULES][2];    // 2-letter nodes (values: 1..26)
static uint8_t insert[RULES];     // letter to insert (value: 1..26)
static uint8_t rule[ALPH][ALPH];  // rule index 0..99 per 2-letter node
static uint8_t lhs[RULES];        // node index 0..99 for LHS transformation
static uint8_t rhs[RULES];        // node index 0..99 for RHS transformation
static uint64_t count[RULES];     // node counts
static uint64_t skratch[RULES];   // skratch area for updating all counts simultaneously

// Number of cycles must be even
// for consecutive calls to make sense
static uint64_t grow(const int cycles)
{
    uint64_t *cur = count, *nxt = skratch;
    for (int cycle = 0; cycle < cycles; ++cycle) {
        // Must start at zero
        memset(nxt, 0, sizeof count);
        // Split nodes
        for (int i = 0; i < RULES; ++i) {
            nxt[lhs[i]] += cur[i];
            nxt[rhs[i]] += cur[i];
        }
        // Update counts
        uint64_t *tmp = cur;
        cur = nxt;
        nxt = tmp;
    }
    // Count individual elements
    uint64_t hist[ALPH] = {0};
    // Original head and tail were only ones not counted double
    hist[input[0] & 31] = 1;
    hist[input[POLYMER - 1] & 31]++;
    // Add node counts to both elements
    for (int i = 0; i < RULES; ++i) {
        hist[node[i][0]] += cur[i];
        hist[node[i][1]] += cur[i];
    }
    // Assignment: find diff between min and max element counts
    uint64_t min = INT64_MAX;
    uint64_t max = 0;
    for (int i = 'A' & 31; i < ALPH; ++i)
        if (hist[i]) {  // not counting unused elements
            if (hist[i] < min) min = hist[i];
            if (hist[i] > max) max = hist[i];
        }
    return (max - min) >> 1;  // all were counted double
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
    const char *c = input + POLYMER + 2;
    for (int i = 0; i < RULES; c += 8, ++i) {
        uint64_t data;
        memcpy(&data, c, sizeof data);  // rule lines are 8 chars long
        data &= UINT64_C(0x001f000000001f1f);  // assumes little-endian
        memcpy(node[i], &data, 2);
        rule[data & 255][data >> 8 & 255] = i;  // assumes little-endian
        insert[i] = data >> 48;
    }
    // Find LHS/RHS node indices
    for (int i = 0 ; i < RULES; ++i) {
        lhs[i] = rule[node[i][0]][insert[i]];
        rhs[i] = rule[insert[i]][node[i][1]];
    }
    // Count every overlapping pair from original polymer
    // This means every element is counted double, except head & tail
    for (c = input; c < input + POLYMER - 1; ++c)
        count[rule[*c & 31][*(c + 1) & 31]]++;
    // Solution
    printf("%"PRIu64" %"PRIu64"\n", grow(GROW1), grow(GROW2 - GROW1));  // 2194 2360298895777

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
