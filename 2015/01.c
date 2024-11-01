/**
 * Advent of Code 2015
 * Day 1: Not Quite Lisp
 * https://adventofcode.com/2015/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 01.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 01.c
 */

#include <stdio.h>

int main(void)
{
    FILE *f = fopen("../aocinput/2015-01-input.txt", "r");
    if (!f) return 1;
    int sum = 0, neg = 0;
    for (int i = 1, c; (c = fgetc(f)) != EOF; ++i) {
        sum += 1 - ((c & 1) << 1);
        if (sum < 0 && !neg)
            neg = i;
    }
    fclose(f);
    printf("%d %d\n", sum, neg);  // 138 1771
    return 0;
}
