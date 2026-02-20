/**
 * Advent of Code 2016
 * Day 20: Firewall Rules
 * https://adventofcode.com/2016/day/20
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile with warnings:
 *     cc -std=c17 -Wall -Wextra -pedantic 20.c
 * Compile for speed, with timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 20.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements including result output:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 47 µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf, fprintf
#include <stdint.h>  // uint32_t
#include <stdlib.h>  // qsort
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2016-20-input.txt"
#define LINES 1024  // needed for my input: 945

typedef struct range {
    uint32_t lo, hi;
} Range;

static Range block[LINES];

// Sort ascending on lo (=first member of struct *p)
static int asc(const void *p, const void *q)
{
    const Range *const a = p;
    const Range *const b = q;
    if (a->lo < b->lo) return -1;
    if (a->lo > b->lo) return  1;
    return 0;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    int n = 0;
    for (uint32_t lo, hi; n < LINES && fscanf(f, "%u-%u", &lo, &hi) == 2; ++n)
        block[n] = (Range){lo, hi};
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Combine segments
    qsort(block, n, sizeof *block, asc);
    int i = 0;
    for (int j = 1; j < n; ) {
        for (; j < n && block[i].hi >= block[j].lo - 1; ++j)
            if (block[j].hi > block[i].hi)
                block[i].hi = block[j].hi;
        if (j < n)
            block[++i] = block[j++];
    }
    n = i + 1;
    printf("Part 1: %u\n", block[0].lo ? 0 : block[0].hi + 1);  // 14975795

    // Init: max + 1 + allow = min (use uint32_t overflow)
    // => allow = min - max - 1 (still works for max=UINT32_MAX)
    uint32_t allow = block[0].lo - block[n - 1].hi - 1;
    for (i = 1; i < n; ++i)  // start at second block i=1
        allow += block[i].lo - block[i - 1].hi - 1;  // gap to previous
    printf("Part 2: %u\n", allow);  // 101

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
