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
 *     Macbook Pro 2024 (M4 4.4 GHz) :  5.75 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  9.4  µs
 *     Raspberry Pi 5 (2.4 GHz)      : 19.8  µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-01-input.txt"
#define FSIZE 1024  // needed for my input: 990
#define N      200  // numbers in input file
#define SUM   2020  // from puzzle description

static char input[FSIZE];
static int data[N];

static int parseint(const char **s)
{
    int x = 0;
    while (**s & '0')
        x = x * 10 + (*(*s)++ & 15);
    (*s)++;  // skip newline
    return x;
}

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
    const int *a = data;
    int *b = data;
    for (const char *c = input; *c; *b++ = parseint(&c));
    qsort(data, N, sizeof *data, sort_int_asc);

    // Part 1
    printf("%d\n", twosum(a, --b, SUM));  // 802011

    // Part 2
    int p;
    for (; !(p = twosum(a + 1, b, SUM - *a)); ++a);
    printf("%d\n", *a * p);  // 248607374

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
