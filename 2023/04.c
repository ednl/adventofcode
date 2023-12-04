/**
 * Advent of Code 2023
 * Day 4: Scratchcards
 * https://adventofcode.com/2023/day/4
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, printf
#include <stdlib.h>   // qsort

#define NAME "../aocinput/2023-04-input.txt"
#define CARDS 202         // number of lines (cards) in input
#define NWIN   10         // winning numbers on one card
#define NHAVE  25         // numbers "you have" on one card
#define N (NWIN + NHAVE)  // total amount of numbers on one card

static int copies[CARDS];
static int numbers[N];

// Sort int array ascending
static int asc(const void *p, const void *q)
{
    const int a = *(const int*)p;
    const int b = *(const int*)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Convert 1 or 2 digits to number (' ' & 15 = 0, so leading space is fine)
static inline int readnum(const char* s)
{
    return ((*s & 15) * 10) + (*(s + 1) & 15);
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    int part1 = 0, part2 = 0, card = 0;  // card = zero-based index
    char buf[128];  // every line is 116 chars + '\n\0'
    while (card < CARDS && fgets(buf, sizeof buf, f)) {  // read one line at a time
        char* s = buf + 10;  // skip to first winning number
        for (int i = 0; i < NWIN; ++i, s += 3)  // read winning numbers
            numbers[i] = readnum(s);
        s += 2;  // skip divider
        for (int i = NWIN; i < N; ++i, s += 3)  // read numbers "you have"
            numbers[i] = readnum(s);

        int match = 0;  // matching number count = intersect(win,have).length
        qsort(numbers, N, sizeof *numbers, asc);
        for (int i = 1; i < N; ++i)
            if (numbers[i] == numbers[i - 1]) {
                ++match;
                ++i;  // can't have more than two consecutive matching numbers
            }

        part1 += match ? (1 << (match - 1)) : 0;  // score for current card
        part2 += ++copies[card];  // count original card as one more copy

        const int m = card + match + 1;  // 1 past maximum index of extra copies to add
        for (int i = card + 1; i < m && i < CARDS; ++i)  // add extra copies
            copies[i] += copies[card];
        ++card;
    }
    fclose(f);

    printf("%d %d\n", part1, part2);  // 24733 5422730
    return 0;
}
