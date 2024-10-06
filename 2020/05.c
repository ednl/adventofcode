/**
 * Advent of Code 2020
 * Day 5: Binary Boarding
 * https://adventofcode.com/2020/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 05.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 05.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  67 µs
 *     ThinkPad Ubuntu (i5 8250U 1.8 GHz)  :  89 µs
 *     Raspberry Pi 5 (2.4 GHz)            : 107 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 125 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 462 µs
 */

#include <stdio.h>              // fopen, fclose, fgets, printf
#include <stdlib.h>             // qsort
#include "../startstoptimer.h"  // my own timing function

#define INP "../aocinput/2020-05-input.txt"
#define N 1024  // number of lines in my input file is 761
#define LEN 16  // line length = 10x F/B/L/R + '\n' + '\0'
static int seat[N];

// Qsort helper function: sort ints ascending
static int cmp_int_asc(const void *p, const void *q)
{
    const int a = *(int *)p;
    const int b = *(int *)q;
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

int main(void)
{
    starttimer();
    FILE *f = fopen(INP, "r");
    if (!f)
        return 1;
    int n = 0, maxid = 0;
    char boardingpass[LEN];
    while (n < N && fgets(boardingpass, sizeof boardingpass, f)) {
        int id = 0;
        for (int i = 0; i < 10; ++i)  // 10-bit binary number
            id = id << 1 | !(boardingpass[i] & 4);  // B or R is 1, F or L is 0
        if (id > maxid)
            maxid = id;
        seat[n++] = id;  // save complete list for part 2
    }
    fclose(f);
    printf("Part 1: %d\n", maxid);  // 861

    qsort(seat, n, sizeof *seat, cmp_int_asc);
    for (int i = 1; i < n; ++i)  // start at second seat
        if (seat[i] - 1 != seat[i - 1]) {  // gap before this seat?
            printf("Part 2: %d\n", seat[i] - 1);  // 633
            break;
        }

    printf("Time: %.0f µs\n", stoptimer_us());
    return 0;
}
