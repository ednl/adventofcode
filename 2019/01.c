/**
 * Advent of Code 2019
 * Day 1: The Tyranny of the Rocket Equation
 * https://adventofcode.com/2019/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.46 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 8.69 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2019-01-input.txt"
#define FSIZE 654  // input file size in bytes

static char input[FSIZE];
static const char *const end = input + FSIZE;

// Fuel needed for one module
static int fuel(const int mass)
{
    return mass / 3 - 2;
}

// Compound fuel requirement
static int comp(int mass)
{
    int total = 0;
    while ((mass = fuel(mass)) > 0)
        total += mass;
    return total;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int sum1 = 0, sum2 = 0;
    for (const char *c = input; c != end; ++c) {  // also skip newline
        int mass = *c++ & 15;
        while (*c != '\n')
            mass = mass * 10 + (*c++ & 15);
        sum1 += fuel(mass);
        sum2 += comp(mass);
    }
    printf("%d %d\n", sum1, sum2);  // 3465154 5194864

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
    return 0;
}
