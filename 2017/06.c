/**
 * Advent of Code 2017
 * Day 6: Memory Reallocation
 * https://adventofcode.com/2017/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 168 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-06-input.txt"
#define N    16u
#define MASK 15u
#define BITS  4u

static unsigned bank[N];

static void next_full(void)
{
    unsigned kmax = 0;
    unsigned vmax = bank[0];
    for (unsigned i = 1; i < N; ++i)
        if (bank[i] > vmax)
            vmax = bank[(kmax = i)];  // max value and index
    bank[kmax] = 0;
    const unsigned k = kmax + 1;      // start redistributing here
    const unsigned r = vmax & MASK;   // first r=vmax%16 banks +1
    const unsigned q = vmax >> BITS;  // every bank +q=vmax/16
    if (q) {
        for (unsigned i = k; i < k + r; ++i)
            bank[i & MASK] += q + 1;
        for (unsigned i = k + r; i < k + N; ++i)
            bank[i & MASK] += q;
    } else
        for (unsigned i = k; i < k + r; ++i)
            bank[i & MASK]++;
}

static void next_fast(void)
{
    unsigned imax = 0;
    unsigned vmax = bank[0];
    for (unsigned i = 1; i < N; ++i)
        if (bank[i] > vmax)
            vmax = bank[(imax = i)];  // max value and index
    bank[imax] = 0;
    const unsigned beg = imax + 1;             // start redistributing here
    const unsigned end = beg + (vmax & MASK);  // first vmax%16 banks +1
    for (unsigned i = beg; i < end; ++i)
        bank[i & MASK]++;
}

static void show(const int n)
{
    printf("%5d:", n);
    for (unsigned i = 0; i < N; ++i)
        printf("%3u", bank[i]);
    putchar('\n');
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    for (unsigned i = 0; i < N && fscanf(f, "%u", &bank[i]) == 1; ++i)
        ;
    fclose(f);
    show(0);

#ifdef TIMER
    starttimer();
#endif

    for (int i = 1; i < 100; ++i)
        next_full();
    for (int i = 100; i < 11265; ++i)
        next_fast();
    show(11264);
    for (int i = 11265; i < 14030; ++i)
        next_fast();
    show(14029);

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
