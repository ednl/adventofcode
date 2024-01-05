/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 01.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra -Wno-misleading-indentation 01.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                  : 100 µs
 *     Raspberry Pi 5 (2.4 GHz)                    : ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz)         : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                    : ? µs
 *     Macbook Air 2013 (i5 4250U Haswell 1.3 GHz) : 220 µs
 */

#include <stdio.h>   // fopen, fclose, fgets, printf
#include <stdint.h>  // int64_t
#include "../startstoptimer.h"

#define M3 ((INT64_C(1) << 24) - 1)
#define M4 ((INT64_C(1) << 32) - 1)
#define M5 ((INT64_C(1) << 40) - 1)

static int lookfwd(const char* s, const char* const end, const int def)
{
    int64_t h = 0;
    for (int i = 0 ; i < 4 ; ++i)
        h = h << 8 | *s++;
    while (s - 4 < end) {
        h = (h << 8 | *s++) & M5;
        switch (h >> 16) {
            case 7302757: return 10;  // "one"
            case 7563640: return 60;  // "six"
            case 7632751: return 20;  // "two"
        }
        switch (h >> 8) {
            case 1718187621: return 50;  // "five"
            case 1718580594: return 40;  // "four"
            case 1852403301: return 90;  // "nine"
        }
        switch (h) {
            case 435560081524: return 80;  // "eight"
            case 495623497070: return 70;  // "seven"
            case 499968533861: return 30;  // "three"
        }
    }
    return def;
}

static int lookback(const char* s, const char* const end, const int def)
{
    int64_t h = 0;
    for (int i = 0 ; i < 4 ; ++i)
        h = h >> 8 | ((int64_t)*--s << 32);
    while (s + 1 > end) {
        h = h >> 8 | ((int64_t)*--s << 32);
        switch (h & M3) {
            case 7302757: return 1;  // "one"
            case 7563640: return 6;  // "six"
            case 7632751: return 2;  // "two"
        }
        switch (h & M4) {
            case 1718187621: return 5;  // "five"
            case 1718580594: return 4;  // "four"
            case 1852403301: return 9;  // "nine"
        }
        switch (h) {
            case 435560081524: return 8;  // "eight"
            case 495623497070: return 7;  // "seven"
            case 499968533861: return 3;  // "three"
        }
    }
    return def;
}

int main(void)
{
    starttimer();
    FILE* f = fopen("../aocinput/2023-01-input.txt", "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    int part1 = 0, part2 = 0;
    char buf[64];  // needed for my input: 58
    while (fgets(buf, sizeof buf, f)) {  // 1000 lines
        const char* s, *t, *end;
        for (s = buf; *s > '9'; ++s);  // find first numerical digit (every line has at least one)
        const int d1 = (*s & 15) * 10;
        part1 += d1;
        part2 += lookfwd(buf, s, d1);  // look for first digit word BEFORE first numerical digit
        for (end = s + 1; *end != '\n'; ++end);  // newline = end of string
        for (t = end - 1; *t > '9' && t > s; --t);  // find last numerical digit
        const int d2 = *t & 15;
        part1 += d2;
        part2 += lookback(end, t, d2);  // look for last digit word AFTER last numerical digit
    }
    fclose(f);
    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // example: 209 281, input: 54630 54770
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
