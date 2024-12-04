/**
 * Advent of Code 2024
 * Day 6: ?
 * https://adventofcode.com/2024/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 06.c
 *    gcc   -std=gnu17 -Wall -Wextra 06.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 06.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 06.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         :   ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         :   ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 1
#if EXAMPLE
    #define FNAME "../aocinput/2024-06-example.txt"
    #define N 10  // lines in example file
#else
    #define FNAME "../aocinput/2024-06-input.txt"
    #define N 1000  // lines in input file
#endif

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Parse input file
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    char buf[BUFSIZ];
    for (int i = 0; fgets(buf, sizeof buf, f); ++i) {
        //TODO
    }
    fclose(f);

    // Part 1
    int part1 = 0;
    printf("Part 1: %d\n", part1);  // ?

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
