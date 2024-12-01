/**
 * Advent of Code 2024
 * Day 1: Historian Hysteria
 * https://adventofcode.com/2024/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 01.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 01.c
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, abs

#define N 1000
static int a[N], b[N];

// Qsort comparison: lowest to highest
static int cmp(const void *p, const void *q)
{
    const int a = *(const int *const)p;
    const int b = *(const int *const)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2024-01-input.txt", "r");
    if (!f) return 1;
    for (int i = 0; i < N && fscanf(f, "%d %d", &a[i], &b[i]) == 2; ++i);
    fclose(f);

    // Sort a, sort b
    qsort(a, N, sizeof *a, cmp);
    qsort(b, N, sizeof *b, cmp);

    // Sum distances of pairs a[i],b[i]
    int distsum = 0;
    for (int i = 0; i < N; ++i)
        distsum += abs(a[i] - b[i]);
    printf("Part 1: %d\n", distsum);  // 1320851

    // Similarity is sum of all products a[i] * count(a[i] in b)
    int simil = 0;
    for (int i = 0, j = 0, k; i < N; ) {
        for (; j < N && a[i] > b[j]; ++j);        // find first j where b[j] is at least a[i]
        for (k = j; j < N && a[i] == b[j]; ++j);  // count all j where b[j] equal to a[i]
        const int part = a[i] * (j - k);          // a[i] * count(a[i] in b)
        for (k = i; i < N && a[i] == a[k]; ++i)   // more than once if a[i] not unique in a
            simil += part;                        // (turns out, all a[i] are unique in a...)
    }
    printf("Part 2: %d\n", simil);  // 26859182
    return 0;
}
