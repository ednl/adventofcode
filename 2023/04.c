/**
 * Advent of Code 2023
 * Day 4: Scratchcards
 * https://adventofcode.com/2023/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 04.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 04.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :  24 µs
 *     Raspberry Pi 5 (2.4 GHz)            :  52 µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :  59 µs
 *     Raspberry Pi 4 (1.8 GHz)            : 124 µs
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <string.h>   // memset
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

#define NAME "../aocinput/2023-04-input.txt"
#define N 202  // lines (cards) in input

static bool win[100];  // numbers in range [0..99]
static int copies[N];

static int min(const int a, const int b)
{
    return a < b ? a : b;
}

// Convert 1 or 2 digits to number (' ' & 15 = 0, so leading space is fine)
static inline int readnum(const char *s)
{
    return ((*s & 15) * 10) + (*(s + 1) & 15);
}

int main(void)
{
    starttimer();
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    int part1 = 0, part2 = 0;
    char buf[128];  // every line is 116 chars + '\n\0'
    for (int card = 0; fgets(buf, sizeof buf, f); ) {  // read one line at a time
        memset(win, 0, sizeof win);       // reset winning number scoring
        char *s = buf + 10;               // skip to first winning number
        for (; *s != '|'; s += 3)         // until '|', skip to next winning number
            win[readnum(s)] = true;       // mark as winning number

        int match = 0;                    // how many winning numbers I have on this card
        for (s += 2; *s; s += 3)          // skip divider, until '\0', skip to next number I have
            match += win[readnum(s)];     // match numbers I have with winning

        part1 += match ? (1 << (match - 1)) : 0;  // score for current card
        const int add = ++copies[card];   // count original card as one more copy
        part2 += add;

        const int lim = min(++card + match, N);  // 1 past maximum index of extra copies to add
        for (int i = card; i < lim; ++i)  // add extra copies
            copies[i] += add;
    }
    fclose(f);
    printf("%d %d\n", part1, part2);  // 24733 5422730
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
