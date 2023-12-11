/**
 * Advent of Code 2023
 * Day 11: ?
 * https://adventofcode.com/2023/day/11
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fgets, printf

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-11-example.txt"
#define N 20
#else
#define NAME "../aocinput/2023-11-input.txt"
#define N 200
#endif

static int data[N];

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < N; ++i)
        fgets(data[i], sizeof *data, f);
    fclose(f);

    int part1 = 0, part2 = 0;
    printf("Part 1: %d\n", part1);  // ex: ?, input: ?
    printf("Part 2: %d\n", part2);  // ex: ?, input: ?
    return 0;
}
