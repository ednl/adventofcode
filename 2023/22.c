/**
 * Advent of Code 2023
 * Day 22: Sand Slabs
 * https://adventofcode.com/2023/day/22
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define EXAMPLE 1
#if EXAMPLE
#define NAME "../aocinput/2023-22-example.txt"
#define N 7
#define M 3  // x/y-range=[0..2]
#else
#define NAME "../aocinput/2023-22-input.txt"
#define N 1250
#define M 10  // x/yy-range=[0..9]
#endif

static uint16_t brick[N][6];
static uint16_t level[M][M];

static int z_asc(const void *p, const void *q)
{
    const uint16_t *a = (const uint16_t*)p;
    const uint16_t *b = (const uint16_t*)q;
    if (a[2] < b[2]) return -1;
    if (a[2] > b[2]) return  1;
    if (a[5] < b[5]) return -1;
    if (a[5] > b[5]) return  1;
    return 0;
}

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f)
        return 1;
    char buf[32];
    for (int i = 0; i < N && fgets(buf, sizeof buf, f); ++i) {
        char *s = buf;
        for (int j = 0; j < 6; ++j, ++s) {
            uint16_t x = 0;
            while (*s >= '0' && *s <= '9')
                x = x * 10 + (*s++ & 15);
            brick[i][j] = x;
        }
    }
    fclose(f);

    qsort(brick, N, sizeof *brick, z_asc);
    for (int i = 0; i < N; ++i)
        printf("%u,%u,%u~%u,%u,%u\n", brick[i][0], brick[i][1], brick[i][2], brick[i][3], brick[i][4], brick[i][5]);
    printf("\n");

    for (int i = 0; i < N; ++i) {
        uint16_t top = 0;
        for (uint16_t x = brick[i][0]; x <= brick[i][3]; ++x)
            for (uint16_t y = brick[i][1]; y <= brick[i][4]; ++y)
                if (level[x][y] > top)
                    top = level[x][y];
        uint16_t drop = brick[i][2] - top - 1;
        brick[i][2] -= drop;
        brick[i][5] -= drop;
        for (uint16_t x = brick[i][0]; x <= brick[i][3]; ++x)
            for (uint16_t y = brick[i][1]; y <= brick[i][4]; ++y)
                level[x][y] = brick[i][5];
    }

    for (int i = 0; i < N; ++i)
        printf("%u,%u,%u~%u,%u,%u\n", brick[i][0], brick[i][1], brick[i][2], brick[i][3], brick[i][4], brick[i][5]);
    printf("\n");

    return 0;
}
