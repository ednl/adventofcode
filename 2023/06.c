/**
 * Advent of Code 2023
 * Day 6: Wait For It
 * https://adventofcode.com/2023/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 06.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 06.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)               : 12 µs
 *     Raspberry Pi 5 (2.4 GHz)                 : 23 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : 39 µs
 *     Raspberry Pi 4 (1.8 GHz)                 : 53 µs
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <math.h>      // sqrt, floor, ceil
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#include "../startstoptimer.h"

#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2023-06-example.txt"
    #define RACES 3
#else
    #define NAME "../aocinput/2023-06-input.txt"
    #define RACES 4
#endif
#define T 0  // time index
#define D 1  // dist index
#define L 2  // number of parameters

static int64_t race[RACES][L];

// Given: t = race time, d = race distance (t>0, d>0)
// Find: x = button time => remaining time = t - x
//   boat speed = v = x => travel = v . time = x . (t - x)
// Condition: travel > d
// Find roots: travel = d
// <=> x(t - x) = d
//     x^2 - tx + d = 0
//     x1,2 = (t +/- sqrt(t^2 - 4d)) / 2
//     x1,2 = t/2 +/- sqrt((t/2)^2 - d)
static int64_t ways2win(int raceid)
{
    const int64_t t = race[raceid][T];  // local variables for convenience
    const int64_t d = race[raceid][D];
    const double t_mid = (double)t / 2;
    const double sq = t_mid * t_mid - d;
    // if (sq < 0)  // no real solutions; doesn't happen in example or my input
    //     return 0;
    const double width = sqrt(sq);
    int64_t button1 = (int64_t)(ceil(t_mid - width));
    int64_t button2 = (int64_t)(floor(t_mid + width));
    if (button1 * (t - button1) == d) { ++button1; --button2; }  // remove integer roots
    // if (button1 <= 0) button1 = 1;  // two impossible cases because t>0,d>0
    // if (button2 >= t) button2 = t - 1;
    const int64_t len = button2 - button1 + 1;  // might be -1 for identical integer roots
    return len > 0 ? len : 0;
}

int main(void)
{
    starttimer();
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    for (int i = 0; i < L; ++i) {  // two lines
        while (fgetc(f) != ':');  // skip to first number
        for (int j = 0; j < RACES; ++j)
            fscanf(f, "%"PRId64, &race[j][i]);  // transpose
    }
    fclose(f);

    // Part 1
    int64_t prod = 1;
    for (int i = 0 ; i < RACES; ++i)
        prod *= ways2win(i);

    // Part 2
    for (int i = 0; i < L; ++i)
        for (int j = 1; j < RACES; ++j) {  // collate into first race
            int64_t mult = 10;
            while (mult <= race[j][i])
                mult *= 10;
            race[0][i] = race[0][i] * mult + race[j][i];
        }

    printf("Part 1: %"PRId64"\n", prod);         // example:   288, input:  2449062
    printf("Part 2: %"PRId64"\n", ways2win(0));  // example: 71503, input: 33149631
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
