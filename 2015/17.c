/**
 * Advent of Code 2015
 * Day 17: No Such Thing as Too Much
 * https://adventofcode.com/2015/day/17
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 17.c ../combperm.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 17.c ../combperm.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  3.78 ms
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  5.39 ms
 *     ThinkPad Ubuntu (i5 8250U 1.8 GHz)  :  6.55 ms
 *     Raspberry Pi 5 (2.4 GHz)            :  9.41 ms
 *     Raspberry Pi 4 (1.8 GHz)            : 55.4  ms
 */

#include <stdio.h>              // fopen, fclose, fscanf, printf
#include <stdlib.h>             // qsort
#include "../combperm.h"        // my own combinations function
#include "../startstoptimer.h"  // my own timing function

#define N       20  // number of lines in input file
#define EGGNOG 150  // total amount of eggnog to put in containers, from puzzle description

// Set of eggnog containers of varying sizes (volumes)
static int container[N];

// Qsort helper function: sort ints ascending
static int cmp_int_asc(const void *p, const void *q)
{
    const int a = *(int *)p;
    const int b = *(int *)q;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

int main(void)
{
    starttimer();
    // Read input file
    FILE *f = fopen("../aocinput/2015-17-input.txt", "r");
    if (!f)
        return 1;
    int n = 0;
    while (n < N && fscanf(f, "%d", &container[n]) == 1)
        ++n;
    fclose(f);
    if (n != N)
        return 2;

    // Sort containers by size (ascending)
    qsort(container, N, sizeof *container, cmp_int_asc);

    // Determine minimum and maximum amount of containers to take
    int mintake = 1, maxtake = 0;
    int sumsmall = 0, sumlarge = 0;
    for (int i = 0; i < N; ++i) {
        sumsmall += container[i];  // add small container sizes
        if (sumsmall <= EGGNOG)
            maxtake++;
        sumlarge += container[N - 1 - i];  // add large container sizes
        if (sumlarge < EGGNOG)
            mintake++;
    }
    if (mintake > maxtake)
        return 3;

    int part1 = 0, part2 = 0, firstbatch = 0;
    for (int k = mintake; k <= maxtake; ++k) {
        int *index = NULL;
        while ((index = combinations(N, k))) {
            int i = 0, sum = 0;
            while (i < k && sum < EGGNOG)
                sum += container[index[i++]];
            if (i == k && sum == EGGNOG) {
                ++part1;  // count any combination that works
                if (!firstbatch) {
                    firstbatch = k;
                    ++part2;
                } else if (k == firstbatch)
                    ++part2;  // count working combinations with minimum number of containers
            }
        }
    }
    printf("%d %d\n", part1, part2);  // 1304 18
    printf("Time: %.0f µs\n", stoptimer_us());
    return 0;
}
