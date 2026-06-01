/**
 * Advent of Code 2020
 * Day 1: Report Repair
 * https://adventofcode.com/2020/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  5.00 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  9.4  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 19.6  µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <stdint.h>  // uint64_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-01-input.txt"
#define FSIZE 992  // must be divisible by 8. Needed for my input: 990=>992
#define N     200  // numbers in input file
#define SUM  2020  // from puzzle description

static char input[FSIZE];
static int data[N];

static int sort_int_asc(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return -1;
    if (a > b) return +1;
    return 0;
}

// Part 1: find product of two numbers that sum to 2020
static int twosum(const int *a, const int *b, const int sum)
{
    while (a < b) {
        if (*a + *b == sum)
            return *a * *b;
        while (a < b && *a + *b > sum)
            b--;
        while (a < b && *a + *b < sum)
            a++;
    }
    return 0;
}

int main(void)
{
    // Read input file
    FILE *f = fopen(FNAME, "rb");
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Parse and sort input
    {
        uint64_t *inp64 = (uint64_t *)input;
        for (int i = 0; i < (FSIZE >> 3); ++i)
            inp64[i] &= UINT64_C(0x0f0f0f0f0f0f0f0f);

        const char *c = input;
        for (int i = 0; i < N; ++i)
            if (*(c + 4) == '\n') {  // 4-digit number +newline
                data[i] = *c * 1000
                    + *(c + 1) * 100
                    + *(c + 2) * 10
                    + *(c + 3);
                c += 5;
            } else {  // 3-digit number +newline
                data[i] = *c * 100
                    + *(c + 1) * 10
                    + *(c + 2);
                c += 4;
            }
        qsort(data, N, sizeof *data, sort_int_asc);
    }

    // Part 1
    printf("%d\n", twosum(data, data + (N - 1), SUM));  // 802011

    // Part 2
    const int *a = data, *b = data + 2;
    const int r = SUM - (*a + *(a + 1));
    while (*b < r)
        b++;
    int p2;
    for (; !(p2 = twosum(a + 1, b, SUM - *a)); ++a);
    printf("%d\n", *a * p2);  // 248607374

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
