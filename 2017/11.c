/**
 * Advent of Code 2017
 * Day 6: Memory Reallocation
 * https://adventofcode.com/2017/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 26 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :  ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#include <stdint.h>  // int16_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-11-input.txt"
#define FSIZE (3 * 8 * 1024)

static char input[FSIZE];

static int dist(const int q, const int r)
{
    return (abs(q) + abs(q + r) + abs(r)) / 2;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    int n = fread(input, 1, FSIZE, f);
    for (; n > 0 && input[n - 1] < 'a'; n--);
    if (n > 0 && (input[n - 1] == 'n' || input[n - 1] == 's')) {
        input[n] = ',';  // add comma if input ends in single n or s
        input[n + 1] = '\0';
    }
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Axial coordinates (q,r)
    // ref.: https://www.redblobgames.com/grids/hexagons/
    int q = 0, r = 0;
    int d = 0, max = 0;
    for (const char *c = input; *c & 64; ) {
        switch (*(const int16_t *)c) {
            case ',n':      r--; c += 2; break;
            case ',s':      r++; c += 2; break;
            case 'en': q++; r--; c += 3; break;
            case 'es': q++;      c += 3; break;
            case 'wn': q--;      c += 3; break;
            case 'ws': q--; r++; c += 3; break;
        }
        if ((d = dist(q, r)) > max)
            max = d;
    }
    printf("%d %d\n", d, max);  // 664 1447

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
