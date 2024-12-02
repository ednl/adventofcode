/**
 * Advent of Code 2024
 * Day 2: Red-Nosed Reports
 * https://adventofcode.com/2024/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 02.c
 *    gcc   -std=gnu17 -Wall -Wextra 02.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 02.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 02.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  88 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 121 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         : 171 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 229 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 400 µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME   "../aocinput/2024-02-input.txt"
#define REPORTS 1000  // lines in input file
#define LEVELS  8     // max numbers per line
#define BUFSIZE (LEVELS * 3 + 1)  // line buf size (2 digits + 1 space/newline per number, +'\0')
#define MINDIST 1
#define MAXDIST 3

static int data[REPORTS][LEVELS];  // input file parsed
static int count[REPORTS];         // number of levels in each report

// Parse 1- or 2-digit number, update char pointer
static inline int num(const char **const c)
{
    int x = (*(*c)++ & 15);
    if (**c >= '0' && **c <= '9')
        x = x * 10 + (*(*c)++ & 15);
    (*c)++;
    return x;
}

// Direction change from level a to b: +1 for a<b, -1 for a>b, 0 for a=b
static inline int change(const int a, const int b)
{
    return (a < b) - (a > b);
}

// Is this report safe? It has 'count' levels. If 'skip' is a valid index, skip it.
static bool issafe(const int *const level, const int count, const int skip)
{
    const int end = count - 1 - (skip == count - 1);  // limit for i when comparing level[i] and level[i+1]
    int sumchange = 0;
    for (int i = 0; i < end; ++i)
        if (i != skip) {
            const int j = i + 1 + (skip == i + 1);  // compare with next level, or skip 1
            const int distance = abs(level[i] - level[j]);
            if (distance < MINDIST || distance > MAXDIST)
                return false;
            sumchange += change(level[i], level[j]);  // -1,0,+1
        }
    const int len = count - 1 - (skip != -1);  // number of intervals between levels
    return abs(sumchange) == len;  // changes must be all -1 or all +1
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Parse input file
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    char buf[BUFSIZE];
    for (int i = 0; fgets(buf, sizeof buf, f); ++i) {
        int j = 0;
        for (const char *c = buf; *c; data[i][j++] = num(&c));
        count[i] = j;  // actual number of levels in this report
    }
    fclose(f);

    // Evaluate reports
    int safe = 0, damp = 0;
    for (int i = 0; i < REPORTS; ++i)  // for every report
        for (int skip = -1; skip < count[i]; ++skip)  // try different versions
            if (issafe(data[i], count[i], skip)) {
                safe += skip == -1;  // part 1
                ++damp;              // part 2
                break;               // stop at first safe version
            }
    printf("%d %d\n", safe, damp);  // 516 561

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
