/**
 * Advent of Code 2023
 * Day 4: Scratchcards
 * https://adventofcode.com/2023/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 04.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 04.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               :  24 µs
 *     Raspberry Pi 5 (2.4 GHz)                       :  52 µs
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) :  59 µs
 *     Raspberry Pi 4 (1.8 GHz)                       : 124 µs
 */

#include <stdio.h>   // fopen, fclose, fgets, printf
#include <string.h>  // memset
#include <stdbool.h>
#include "../startstoptimer.h"

#define NAME "../aocinput/2023-04-input.txt"
#define CARDS 202         // number of lines (cards) in input
#define NWIN   10         // winning numbers on one card
#define NHAVE  25         // numbers "you have" on one card
#define N (NWIN + NHAVE)  // total amount of numbers on one card

static int copies[CARDS];
static bool win[128];

// Convert 1 or 2 digits to number (' ' & 15 = 0, so leading space is fine)
static inline int readnum(const char* s)
{
    return ((*s & 15) * 10) + (*(s + 1) & 15);
}

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    int part1 = 0, part2 = 0, card = 0;  // card = zero-based index
    char buf[128];  // every line is 116 chars + '\n\0'
    while (card < CARDS && fgets(buf, sizeof buf, f)) {  // read one line at a time
        char* s = buf + 10;  // skip to first winning number
        memset(win, 0, sizeof win);
        for (int i = 0; i < NWIN; ++i, s += 3)  // read winning numbers
            win[readnum(s)] = true;

        s += 2;  // skip divider
        int match = 0;  // matching number count = intersect(win,have).length
        for (int i = NWIN; i < N; ++i, s += 3)  // read numbers "you have"
            match += win[readnum(s)];

        part1 += match ? (1 << (match - 1)) : 0;  // score for current card
        part2 += ++copies[card];  // count original card as one more copy

        const int m = card + match + 1;  // 1 past maximum index of extra copies to add
        for (int i = card + 1; i < m && i < CARDS; ++i)  // add extra copies
            copies[i] += copies[card];
        ++card;
    }
    fclose(f);
    printf("%d %d\n", part1, part2);  // 24733 5422730
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
