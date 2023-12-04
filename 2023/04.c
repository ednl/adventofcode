/**
 * Advent of Code 2023
 * Day 4: Scratchcards
 * https://adventofcode.com/2023/day/4
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fscanf, printf
#include <stdlib.h>   // qsort

#define EXAMPLE 0
#if EXAMPLE == 1
#define NAME "../aocinput/2023-04-example.txt"
#define CARDS 6
#define NWIN  5
#define NHAVE 8
#else
#define NAME "../aocinput/2023-04-input.txt"
#define CARDS 202
#define NWIN   10
#define NHAVE  25
#endif
#define N (NWIN + NHAVE)  // total amount of numbers on one card

static int copies[CARDS];
static int numbers[N];

static int asc(const void *p, const void *q)
{
    const int a = *(const int*)p;
    const int b = *(const int*)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    int part1 = 0, part2 = 0, card = 0, id;  // card = zero-based index, id unused
    while (card < CARDS && fscanf(f, " Card %d:", &id) == 1) {
        for (int i = 0; i < NWIN; ++i)  // read winning numbers
            fscanf(f, "%d", &numbers[i]);
        fgetc(f); fgetc(f);             // skip " |" divider
        for (int i = NWIN; i < N; ++i)  // read numbers "you have"
            fscanf(f, "%d", &numbers[i]);

        int match = 0;  // matching number count = intersect(win,have).length
        qsort(numbers, N, sizeof *numbers, asc);
        for (int i = 1; i < N; ++i)
            if (numbers[i] == numbers[i - 1]) {
                ++match;
                ++i;  // can't have more than two consecutive matching numbers
            }

        part1 += match ? (1 << (match - 1)) : 0;  // score from matching number count
        part2 += ++copies[card];  // count original card as one more copy

        const int m = card + match + 1;  // 1 past maximum index of extra copies to add
        for (int i = card + 1; i < m && i < CARDS; ++i)  // add extra copies
            copies[i] += copies[card];
        ++card;
    }

    printf("%d %d\n", part1, part2);  // example: 13 30, input: 24733 5422730
    return 0;
}
