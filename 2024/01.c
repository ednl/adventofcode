/**
 * Advent of Code 2024
 * Day 1: ?
 * https://adventofcode.com/2024/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 01.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 01.c
 */

#include <stdio.h>

int main(void)
{
    FILE *f = fopen("../aocinput/2024-01-input.txt", "r");
    if (!f) return 1;

    int part1 = 0, part2 = 0;
    char buf[BUFSIZ];
    while (fgets(buf, sizeof buf, f)) {
        //TODO
    }
    fclose(f);

    printf("%d %d\n", part1, part2);  // ? ?
    return 0;
}
