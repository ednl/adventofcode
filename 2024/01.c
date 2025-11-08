/**
 * Advent of Code 2024
 * Day 1: Historian Hysteria
 * https://adventofcode.com/2024/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 01.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 01.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  64 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       : 108 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         : 181 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 189 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 237 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 504 µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, abs
#include "../startstoptimer.h"

#define FNAME "../aocinput/2024-01-input.txt"
#define FSIZE 16384  // >= input file size in bytes
#define N     1000   // number of lines in input file

static char input[FSIZE];
static int a[N], b[N];  // two columns of numbers

// Qsort comparison: lowest to highest
static int cmp_int_asc(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Read 5-digit number, update char pointer
static int num5(const char **const c)
{
    int x  = (*(*c)++ & 15) * 10000;
        x += (*(*c)++ & 15) * 1000;
        x += (*(*c)++ & 15) * 100;
        x += (*(*c)++ & 15) * 10;
        x += (*(*c)++ & 15);
    return x;
}

int main(void)
{
    starttimer();

    // Read input file
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

    // Read numbers into columns
    const char *c = input;
    for (int i = 0; i < N; ++i) {
        a[i] = num5(&c); c += 3;  // 5 digits, 3 spaces
        b[i] = num5(&c); c++;     // 5 digits, newline
    }

    // Sort columns
    qsort(a, N, sizeof *a, cmp_int_asc);
    qsort(b, N, sizeof *b, cmp_int_asc);

    // Part 1: sum distances of pairs a[i],b[i]
    int distsum = 0;
    for (int i = 0; i < N; ++i)
        distsum += abs(a[i] - b[i]);
    printf("Part 1: %d\n", distsum);  // 1320851

    // Part 2: "similarity" is sum of all products a[i] * count(a[i] in b)
    int simil = 0;
    for (int i = 0, j = 0; i < N; ++i) {  // for each a[i]
        while (j < N && a[i] > b[j])      // find matching b[j]
            ++j;
        while (j < N && a[i] == b[j])     // add each matching b[j]
            simil += b[j++];
    }
    printf("Part 2: %d\n", simil);  // 26859182

    // Time including reading/parsing of input
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
