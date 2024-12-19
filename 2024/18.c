/**
 * Advent of Code 2024
 * Day 18: RAM Run
 * https://adventofcode.com/2024/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 18.c
 *    gcc   -std=gnu17 -Wall -Wextra 18.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 18.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 18.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                         : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                         : ? µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif


int main(void)
{
#ifdef TIMER
    starttimer();
#endif


#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
