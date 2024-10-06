/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 01.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 01.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  56 µs (45 µs without reading the input file)
 *     Raspberry Pi 5 (2.4 GHz)            : 103 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 106 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 219 µs
 */

#include <stdio.h>   // fopen, fclose, fread, printf
#include "../startstoptimer.h"

#define N 32768  // greater than size of input file (21985)
#define BITS15 ((1U << 15) - 1U)  // 15-bit mask
#define BITS20 ((1U << 20) - 1U)  // 20-bit mask
#define BITS25 ((1U << 25) - 1U)  // 25-bit mask

static char input[N];  // complete input file as one blob

// Forwards sliding window of compounded chars to look for substrings
// between start (inclusive) and end (exclusive).
static int lookfwd(const char *start, const char *const end, const int def)
{
    if (start + 3 > end) return def;
    // Startup 3/4/3.
    unsigned win = 0;
    for (int i = 0 ; i < 3 ; ++i)
        win = win << 5 | (unsigned)(*start++ - 'a');
    switch (win) {
        case 14756u: return 10;  // "one"
        case 18711u: return 60;  // "six"
        case 20174u: return 20;  // "two"
    }
    if (start == end) return def;
    win = win << 5 | (unsigned)(*start++ - 'a');
    switch (win) {
        case 172708u: return 50;  // "five"
        case 178833u: return 40;  // "four"
        case 434596u: return 90;  // "nine"
    }
    switch (win & BITS15) {
        case 14756u: return 10;  // "one"
        case 18711u: return 60;  // "six"
        case 20174u: return 20;  // "two"
    }
    // Loop 5/4/3, 5/4/3, etc.
    while (start < end) {
        win = win << 5 | (unsigned)(*start++ - 'a');
        switch (win & BITS25) {
            case  4462835u: return 80;  // "eight"
            case 19027085u: return 70;  // "seven"
            case 20169860u: return 30;  // "three"
        }
        switch (win & BITS20) {
            case 172708u: return 50;  // "five"
            case 178833u: return 40;  // "four"
            case 434596u: return 90;  // "nine"
        }
        switch (win & BITS15) {
            case 14756u: return 10;  // "one"
            case 18711u: return 60;  // "six"
            case 20174u: return 20;  // "two"
        }
    }
    return def;
}

// Backwards sliding window of compounded chars to look for substrings
// between start (inclusive) and end (exclusive).
static int lookback(const char *start, const char *const end, const int def)
{
    if (start - 3 < end) return def;
    // Startup 3/4/3.
    unsigned win = 0;
    for (int i = 0 ; i < 3 ; ++i)
        win = win >> 5 | ((unsigned)(*start-- - 'a') << 20);
    switch (win >> 10) {
        case 14756u: return 1;  // "one"
        case 18711u: return 6;  // "six"
        case 20174u: return 2;  // "two"
    }
    if (start == end) return def;
    win = win >> 5 | ((unsigned)(*start-- - 'a') << 20);
    switch (win >> 5) {
        case 172708u: return 5;  // "five"
        case 178833u: return 4;  // "four"
        case 434596u: return 9;  // "nine"
    }
    switch (win >> 10) {
        case 14756u: return 1;  // "one"
        case 18711u: return 6;  // "six"
        case 20174u: return 2;  // "two"
    }
    // Loop 5/4/3, 5/4/3, etc.
    while (start > end) {
        win = win >> 5 | ((unsigned)(*start-- - 'a') << 20);
        switch (win) {
            case  4462835u: return 8;  // "eight"
            case 19027085u: return 7;  // "seven"
            case 20169860u: return 3;  // "three"
        }
        switch (win >> 5) {
            case 172708u: return 5;  // "five"
            case 178833u: return 4;  // "four"
            case 434596u: return 9;  // "nine"
        }
        switch (win >> 10) {
            case 14756u: return 1;  // "one"
            case 18711u: return 6;  // "six"
            case 20174u: return 2;  // "two"
        }
    }
    return def;
}

int main(void)
{
    starttimer();
    FILE *f = fopen("../aocinput/2023-01-input.txt", "rb");  // fread requires binary mode
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file in one go
    fclose(f);

    int part1 = 0, part2 = 0;
    for (const char *line = input, *end, *s1, *s2; *line; line = end + 2) {
        for (s1 = line; *s1 > '9'; ++s1);  // find first numerical digit (every line has at least one)
        const int d1 = (*s1 & 15) * 10;
        part1 += d1;
        part2 += lookfwd(line, s1, d1);  // look for first digit word BEFORE first numerical digit
        for (end = s1 + 1; *end != '\n'; ++end);  // newline char = end of this line
        for (s2 = --end; *s2 > '9' && s2 > s1; --s2);  // find last numerical digit backwards
        const int d2 = *s2 & 15;
        part1 += d2;
        part2 += lookback(end, s2, d2);  // look for last digit word AFTER last numerical digit
    }
    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // example: 209 281, input: 54630 54770
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
