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
#define N    6
#define WIN  5
#define HAVE 8
#else
#define NAME "../aocinput/2023-04-input.txt"
#define N    202
#define WIN   10
#define HAVE  25
#endif

static int copies[N];
static int win[WIN], have[HAVE];

static int asc(const void *p, const void *q)
{
    const int a = *(const int*)p;
    const int b = *(const int*)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Matching number count = intersect(win,have).length
static int match(void)
{
    int count = 0;
    qsort(win, WIN, sizeof *win, asc);
    qsort(have, HAVE, sizeof *have, asc);
    for (int i = 0, j = 0; i < WIN && j < HAVE; ++i) {
        while (j < HAVE && win[i] > have[j])
            ++j;
        if (j < HAVE && win[i] == have[j])
            ++count;
    }
    return count;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;

    int part1 = 0, part2 = 0, n = 0, id;
    while (n < N && fscanf(f, " Card %d:", &id) == 1) {
        for (int i = 0; i < WIN; ++i)  // read winning numbers
            fscanf(f, "%d", &win[i]);
        fgetc(f); fgetc(f);  // skip " |"
        for (int i = 0; i < HAVE; ++i)  // read numbers "you have"
            fscanf(f, "%d", &have[i]);
        const int m = match();  // matching number count = intersect(win,have).length
        part1 += m ? (1 << (m - 1)) : 0;  // score from matching number count
        part2 += ++copies[n];  // count original card as one more copy
        const int k = n + m + 1;  // 1 past maximum index of extra copies to add
        for (int i = n + 1; i < k && i < N; ++i)  // add extra copies
            copies[i] += copies[n];
        ++n;
    }

    printf("%d %d\n", part1, part2);  // example: 13 30, input: 24733 5422730
    return 0;
}
