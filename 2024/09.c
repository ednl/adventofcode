/**
 * Advent of Code 2024
 * Day 9: Disk Fragmenter
 * https://adventofcode.com/2024/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 09.c
 *    gcc   -std=gnu17 -Wall -Wextra 09.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 09.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 09.c ../startstoptimer.c
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
    #define FNAME "../aocinput/2024-09-example.txt"
    #define N 19
    static const char *map = "2333133121414131402";
#else
    #define FNAME "../aocinput/2024-09-input.txt"
    #define N 19999
    static char map[N];
#endif


int main(void)
{
#if !EXAMPLE
    FILE *f = fopen("../aocinput/2024-09-input.txt", "rb");
    fread(map, sizeof map, 1, f);
    fclose(f);
#endif

#ifdef TIMER
    starttimer();
#endif

    size_t chksum = 0;
    size_t beg = 0, end = N - 1;  // inclusive range
    size_t pos = 0;
    while (beg < end) {
        if (beg & 1) {
            // free space
        } else {
            size_t id = beg >> 1;
            size_t len = map[beg] & 15;
            chksum += id * pos * len + id * ((len * (len - 1)) >> 1);
            pos += len;
        }
        ++beg;
    }
    printf("%.3s\n", &map[N - 3]);

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
