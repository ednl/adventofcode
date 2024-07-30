/**
 * Advent of Code 2020
 * Day 10: Adapter Array
 * https://adventofcode.com/2020/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 10.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 10.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          : 34 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  ? µs
 *     Air 2013 (i5 Haswell 4250U 1.3 GHz) : 90 µs
 *     ThinkPad Ubuntu (i5 8250U 1.8 GHz)  :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :  ? µs
 *     Raspberry Pi 4 (1.8 GHz)            :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include <stdbool.h>
#include "../startstoptimer.h"

#define FNAME "../aocinput/2020-10-input.txt"
#define JOLTS  256  // max length of input file +2
#define GAPMIN 1
#define GAPMAX 3

static int jolts[JOLTS];  // adapters from input file, +charging outlet, +my device
static int N;  // length of input file +2

// Comparison function for qsort() of int array
int cmp(const void *p, const void *q)
{
    const int a = *(const int*)p;
    const int b = *(const int*)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Find element in sorted data by binary search
bool ispresent(const int k)
{
    if (k < jolts[0] || k > jolts[N - 1])
        return false;
    if (k == jolts[0] || k == jolts[N - 1])
        return true;
    int l = 0, r = N - 1;
    while (r - l > 1) {
        const int m = ((l + 1) >> 1) + (r >> 1);
        if (k < jolts[m]) r = m;
        else if (k > jolts[m]) l = m;
        else return true;
    }
    return false;
}

int main(void)
{
    starttimer();

    FILE *f = fopen(FNAME, "r");
    if (!f)
        return 1;
    N = 1;  // charging outlet: jolts[0] = 0
    while (N < JOLTS - 1 && fscanf(f, "%d", &jolts[N]) == 1)  // JOLTS-1 to add my device
        ++N;
    fclose(f);
    qsort(jolts, N, sizeof *jolts, cmp);  // sort input (with charging outlet = 0 jolts)
    N++;  // space for my device
    jolts[N - 1] = jolts[N - 2] + GAPMAX;  // my device: adaptermax + 3

    int gap1 = 0, gap3 = 0;
    for (int i = 1; i < N; ++i)
        switch (jolts[i] - jolts[i - 1]) {  // adapters must be sorted in ascending order
            case GAPMIN: ++gap1; break;
            case GAPMAX: ++gap3; break;
        }
    printf("Part 1: %d\n", gap1 * gap3);  // 2574

    int pathlen = jolts[N - 1] + 1;
    int64_t *path = malloc(pathlen * sizeof *path);
    path[0] = 1;
    for (int i = 1; i < pathlen; ++i) {
        path[i] = 0;
        for (int j = i - GAPMAX; j < i; ++j)
            if (ispresent(j))
                path[i] += path[j];
    }
    printf("Part 2: %"PRId64"\n", path[pathlen - 1]);  // 2644613988352
    free(path);

    printf("Time: %.0f µs\n", stoptimer_us());
    return 0;
}
