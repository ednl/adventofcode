// AoC 2021 Day 14: Extended Polymerization
// https://adventofcode.com/2021/day/14

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
#include <stdlib.h>    // atoi
#include <stdint.h>    // uint64_t
#include <limits.h>    // UINT64_MAX
#include <inttypes.h>  // PRIu64
#include "../startstoptimer.h"

#define STEPS1 10  // take so many growth steps, part 1
#define STEPS2 40  // take so many growth steps, part 2
#define PSIZE  20  // original polymer length = 20 in my input
#define RULES 100  // number of pair replacement rules in my input
#define ELMS ('Z' - 'A' + 1)  // number of elements = letters in the alphabet

// Representation of rule AB->C where pair=AB, ins=C.
// "left" then points to the rule for pair AC, "right" to pair CB.
// "count" is how many pairs of AB there are now in the polymer, "next" how many next.
// (Counts can't be updated in place because every pair transforms at the same time.)
typedef struct Rule {
    struct Rule *left, *right;  // pointers to replacement pairs (NULL = no replacement)
    uint64_t count, next;       // how many of this pair are there now and next round
    char pair[3], ins;          // pair (=2 elements) to replace, 1 element to insert
} Rule;

static Rule rule[RULES] = {0};   // all transformation rules
static char polymer[PSIZE + 1];  // must be same size as in input file, +1 for \0

// Get index 0-25 from element A-Z.
static int ix(const char c)
{
    return (int)(c - 'A');
}

// Find the transformed pair rules for every pair in the rule set
// to initialise the left/right pointers.
static Rule *findrule_c(const char a, const char b)
{
    for (int i = 0; i < RULES; ++i)
        if (rule[i].pair[0] == a && rule[i].pair[1] == b)
            return &rule[i];
    return NULL;  // should never get here
}

// Find the corresponding rule for every element pair in the given polymer
// to initialise its count.
static Rule *findrule_s(const char *s)
{
    return findrule_c(s[0], s[1]);
}

// Read the input file, initialise the pair count and left/right pointers.
static void parse(const char *inp)
{
    FILE *f = fopen(inp, "r");
    fscanf(f, "%s ", polymer);
    for (int i = 0; i < RULES && fscanf(f, "%2s -> %c", rule[i].pair, &rule[i].ins) == 2; ++i);
    fclose(f);

    // Count elements by pairs in initial polymer; the first and
    // last elements will be the only ones not counted double.
    findrule_s(polymer)->count++;          // first pair
    for (int i = 1; i < PSIZE - 1; ++i)
        findrule_s(&polymer[i])->count++;  // next overlapping pairs

    // Save links to replacement pairs
    for (int i = 0; i < RULES; ++i) {
        rule[i].left  = findrule_c(rule[i].pair[0], rule[i].ins    );
        rule[i].right = findrule_c(rule[i].ins    , rule[i].pair[1]);
    }
}

// Grow the polymer by a number of steps where every pair transforms
// into two other pairs.
// First calculate all new counts, then set them as current counts.
static void grow(const int cycles)
{
    static int cycle = 0;
    for (; cycle < cycles; ++cycle) {
        for (int i = 0; i < RULES; ++i) {
            rule[i].left->next  += rule[i].count;
            rule[i].right->next += rule[i].count;
        }
        for (int i = 0; i < RULES; ++i) {
            rule[i].count = rule[i].next;
            rule[i].next = 0;  // always reset for next growth cycle!
        }
    }
}

// Execute a number of growth cycles, then count every element.
// Return the range of element counts as requested by the puzzle.
static uint64_t range(void)
{
    // Count individual elements from pairs and add
    // the two original elements from head and tail,
    // the only ones not counted double so far.
    uint64_t hist[ELMS] = {0};  // histogram of elements
    hist[ix(polymer[0])] = 1;
    hist[ix(polymer[PSIZE - 1])]++;  // can be the same as element 0, hence ++
    for (int i = 0; i < RULES; ++i) {
        hist[ix(rule[i].pair[0])] += rule[i].count;
        hist[ix(rule[i].pair[1])] += rule[i].count;
    }

    // Determine minimum (non-zero) and maximum
    uint64_t min = UINT64_MAX, max = 0;
    for (int i = 0; i < ELMS; ++i)
        if (hist[i]) {
            if (hist[i] < min) min = hist[i];
            if (hist[i] > max) max = hist[i];
        }

    // Return range, divide by 2 to undo the double counting
    return (max - min) >> 1;
}

int main(void)
{
    starttimer();
    parse("../aocinput/2021-14-input.txt");
    grow(STEPS1);
    printf("Part 1: %"PRIu64"\n", range());  // 2194
    grow(STEPS2);
    printf("Part 2: %"PRIu64"\n", range());  // 2360298895777
    printf("%.0f µs\n", stoptimer_us());
    return 0;
}
