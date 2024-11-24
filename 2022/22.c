/**
 * Advent of Code 2022
 * Day 22: Monkey Map
 * https://adventofcode.com/2022/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 22.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 22.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<1000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          :   ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)            :   ? µs
 *     Raspberry Pi 4 (1.8 GHz)            :   ? µs
 */

#include <stdio.h>

#define EXAMPLE 1
#if EXAMPLE == 1
    #define NAME "../aocinput/2022-22-example.txt"
    #define ROWS 12
    #define COLS 16
#else
    #define NAME "../aocinput/2022-22-input.txt"
    #define ROWS 200
    #define COLS 150
#endif

typedef struct pos {
    int row, col;
} Pos;

static char grid[ROWS][COLS + 2];  // +\n\0

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f) return 1;
    for (int i = 0; i < ROWS; ++i)
        fgets(grid[i], sizeof *grid, f);
    fclose(f);
    Pos pos = {0};
    while (grid[0][pos.col] != '.')
        pos.col++;

    int part1 = 0;
    printf("Part 1: %d\n", part1);  // example=1000*6+4*8+0=6032, input=?

    return 0;
}
