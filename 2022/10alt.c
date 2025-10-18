/**
 * Advent of Code 2022
 * Day 10: Cathode-Ray Tube
 * https://adventofcode.com/2022/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=c17 -Wall -Wextra -pedantic 10alt.c
 *    gcc   -std=c17 -Wall -Wextra -pedantic 10alt.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native -mtune=native 10alt.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native -mtune=native 10alt.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)                    :  3.67 µs
 *     Raspberry Pi 5 (2.4 GHz)                         :  7.54 µs
 *     Mac Mini 2020 (M1 3.2 GHz)                       :    ?  µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         :    ?  µs
 */

#include <stdio.h>    // fopen, fread, fclose, fputc, printf
#include <stdlib.h>   // abs
#include <stdbool.h>  // bool
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define LINE 40
#define HALF (LINE >> 1)

static char input[1024];

static int readnum(char **s)
{
    const bool neg = **s == '-';
    *s += neg;
    int x = *(*s)++ & 15;
    while (**s != '\n')
        x = x * 10 + (*(*s)++ & 15);
    return neg ? -x : x;
}

int main(void)
{
    FILE *f = fopen("../aocinput/2022-10-input.txt", "r");
    fread(input, sizeof input, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int cycle = 0, beam = 0, regx = 1, sigsum = 0;
    for (char *c = input; *c; ) {
        fputc(abs(regx - beam) < 2 ? '#' : ' ', stdout);  // part 2: EFUGLPAP
        ++cycle;
        ++beam;
        if (beam == HALF)
            sigsum += cycle * regx;
        else if (beam == LINE) {
            beam = 0;
            fputc('\n', stdout);
        }
        if (*c & 64)  // 'a' or 'n'
            c += 5;
        else {        // '-' or '0'..'9'
            regx += readnum(&c);
            c += 1;
        }
    }
    printf("%d\n", sigsum);  // part 1: example=13140, input=15020

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
