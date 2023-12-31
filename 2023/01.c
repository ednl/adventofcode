/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 01.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 01.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  60 µs
 *     Raspberry Pi 5 (2.4 GHz)            : 107 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 110 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 226 µs
 */

#include <stdio.h>   // fopen, fclose, fgets, printf
#include <stdint.h>  // int64_t
#include "../startstoptimer.h"

#define N 32768  // greater than size of input file (21985)
#define M3 ((INT64_C(1) << 24) - 1)  // 3-byte mask
#define M4 ((INT64_C(1) << 32) - 1)  // 4-byte mask
#define M5 ((INT64_C(1) << 40) - 1)  // 5-byte mask

static char input[N];  // complete input file as one blob

// Forwards sliding window of compounded chars to look for substrings between start and end
static int lookfwd(const char* start, const char* const end, const int def)
{
    int64_t win = 0;
    for (int i = 0 ; i < 4 ; ++i)
        win = win << 8 | *start++;
    while (start - 4 < end) {
        win = (win << 8 | *start++) & M5;
        switch (win >> 16) {
            case 7302757: return 10;  // "one"
            case 7563640: return 60;  // "six"
            case 7632751: return 20;  // "two"
        }
        switch (win >> 8) {
            case 1718187621: return 50;  // "five"
            case 1718580594: return 40;  // "four"
            case 1852403301: return 90;  // "nine"
        }
        switch (win) {
            case 435560081524: return 80;  // "eight"
            case 495623497070: return 70;  // "seven"
            case 499968533861: return 30;  // "three"
        }
    }
    return def;
}

// Backwards sliding window of compounded chars to look for substrings between start and end
static int lookback(const char* start, const char* const end, const int def)
{
    int64_t win = 0;
    for (int i = 0 ; i < 4 ; ++i)
        win = win >> 8 | ((int64_t)*--start << 32);
    while (start + 1 > end) {
        win = win >> 8 | ((int64_t)*--start << 32);
        switch (win & M3) {
            case 7302757: return 1;  // "one"
            case 7563640: return 6;  // "six"
            case 7632751: return 2;  // "two"
        }
        switch (win & M4) {
            case 1718187621: return 5;  // "five"
            case 1718580594: return 4;  // "four"
            case 1852403301: return 9;  // "nine"
        }
        switch (win) {
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
    FILE* f = fopen("../aocinput/2023-01-input.txt", "rb");  // binary for fread
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file in one go
    fclose(f);

    int part1 = 0, part2 = 0;
    for (const char* line = input, *end, *s1, *s2; *line; line = end + 1) {
        for (s1 = line; *s1 > '9'; ++s1);  // find first numerical digit (every line has at least one)
        const int d1 = (*s1 & 15) * 10;
        part1 += d1;
        part2 += lookfwd(line, s1, d1);  // look for first digit word BEFORE first numerical digit
        for (end = s1 + 1; *end != '\n'; ++end);  // newline = end of line
        for (s2 = end - 1; *s2 > '9' && s2 > s1; --s2);  // find last numerical digit
        const int d2 = *s2 & 15;
        part1 += d2;
        part2 += lookback(end, s2, d2);  // look for last digit word AFTER last numerical digit
    }
    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // example: 209 281, input: 54630 54770
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
