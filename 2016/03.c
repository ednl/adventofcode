/**
 * Advent of Code 2016
 * Day 3: Squares With Three Sides
 * https://adventofcode.com/2016/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *   cc -std=c17 -Wall -Wextra -pedantic -O3 -march=native -mtune=native 03.c
 */

#include <stdio.h>  // FILE, fopen, fclose, fscanf, printf

static int istriangle(const int a, const int b, const int c)
{
    return (a + b > c) && (a + c > b) && (b + c > a);
}

int main(void)
{
    FILE *f = fopen("../aocinput/2016-03-input.txt", "r");
    if (!f) return 1;
    int n = 0, m = 0;
    for (int a[9]; fscanf(f, "%d %d %d %d %d %d %d %d %d", a, a+1, a+2, a+3, a+4, a+5, a+6, a+7, a+8) == 9;) {
        n += istriangle(a[0], a[1], a[2]);
        n += istriangle(a[3], a[4], a[5]);
        n += istriangle(a[6], a[7], a[8]);
        m += istriangle(a[0], a[3], a[6]);
        m += istriangle(a[1], a[4], a[7]);
        m += istriangle(a[2], a[5], a[8]);
    }
    fclose(f);
    printf("%d %d\n", n, m);  // 982 1826
}
