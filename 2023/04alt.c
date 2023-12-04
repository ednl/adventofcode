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

static char input[CARDS][128];
static int numbers[CARDS][N];
static int copies[CARDS + NWIN];

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
    for (int i = 0; i < CARDS; ++i)
        fgets(input[i], sizeof *input, f);
    fclose(f);

    for (int i = 0; i < CARDS; ++i) {
        const char* s = input[i] + 10;  // skip to first winning number
        for (int j = 0; j < NWIN; ++j, s += 3)  // read winning numbers
            numbers[i][j] = readnum(s);
    }

    for (int i = 0; i < CARDS; ++i) {
        const char* s = input[i] + 42;  // skip to first number "you have"
        for (int j = NWIN; j < N; ++j, s += 3)  // read numbers "you have"
            numbers[i][j] = readnum(s);
    }

    for (int i = 0; i < CARDS; ++i)
        qsort(numbers[i], N, sizeof **numbers, asc);

    for (int i = 0; i < CARDS; ++i)
        copies[i] = 1;  // count original card as one more copy

    int part1 = 0;
    for (int i = 0; i < CARDS; ++i) {
        int match = 0;  // matching number count = intersect(win,have).length
        for (int j = 1; j < N; ++j)
            match += numbers[i][j] == numbers[i][j - 1];
        if (match) {
            part1 += 1 << (match - 1);  // score for current card
            const int k = i + match + 1;  // 1 past maximum index of extra copies to add
            for (int j = i + 1; j < k; ++j)  // add extra copies (index overflow is ok)
                copies[j] += copies[i];
        }
    }

    int part2 = 0;
    for (int i = 0; i < CARDS; ++i)
        part2 += copies[i];

    printf("%d %d\n", part1, part2);  // 24733 5422730
    return 0;
}
