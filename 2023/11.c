/**
 * Advent of Code 2023
 * Day 11: ?
 * https://adventofcode.com/2023/day/11
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, fgets, printf
#include <stdlib.h>   // malloc, free
#include <string.h>   // memset, memcpy
#include <stdbool.h>  // bool

#define EXAMPLE 0
#if EXAMPLE
#define NAME "../aocinput/2023-11-example.txt"
#define N 10
#else
#define NAME "../aocinput/2023-11-input.txt"
#define N 140
#endif

typedef struct vec {
    int x, y;
} Vec;

static char image[N][N + 2];

static int absi(const int a)
{
    return a < 0 ? -a : a;
}

static int manh(const Vec a, const Vec b)
{
    return absi(a.x - b.x) + absi(a.y - b.y);
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    int galaxies = 0;
    for (int i = 0; i < N; ++i) {
        fgets(image[i], sizeof *image, f);
        for (int j = 0; j < N; ++j)
            galaxies += image[i][j] == '#';
    }
    fclose(f);
    printf("galaxies=%d\n", galaxies);

    bool isempty[2][N] = {0};
    int expand[2][N] = {0};
    int addrows = 0, addcols = 0;
    for (int i = 0; i < N; ++i) {
        bool nogalaxy = true;
        for (int j = 0; j < N; ++j)
            if (image[i][j] == '#') {  // check all cols in this row
                nogalaxy = false;
                break;
            }
        if (nogalaxy) {
            isempty[0][i] = true;
            expand[0][addrows++] = i;
        }

        nogalaxy = true;
        for (int j = 0; j < N; ++j)
            if (image[j][i] == '#') {  // check all rows in this col
                nogalaxy = false;
                break;
            }
        if (nogalaxy) {
            isempty[1][i] = true;
            expand[1][addcols++] = i;
        }
    }
    expand[0][addrows] = N;
    expand[1][addcols] = N;

    #if EXAMPLE
    printf("rows=%d ", addrows);
    for (int i = 0; i < N; ++i)
        printf("%c", isempty[0][i] ? '+' : '.');
    for (int i = 0; i <= addrows; ++i)
        printf(" %d", expand[0][i]);
    printf("\ncols=%d ", addcols);
    for (int i = 0; i < N; ++i)
        printf("%c", isempty[1][i] ? '+' : '.');
    for (int i = 0; i <= addcols; ++i)
        printf(" %d", expand[1][i]);
    printf("\n");
    #endif

    int rows = addrows + N, cols = addcols + N;
    int size = rows * cols;
    char* universe = malloc((size_t)size);
    memset(universe, '.', (size_t)size);
    int r = 0;
    for (int i = 0; i < N; ++i, ++r) {
    // for (int i = 0; i < 1; ++i, ++r) {
        if (isempty[0][i])  // empty row?
            ++r;
        int src = 0, dst = 0;
        for (int j = 0; j <= addcols; ++j) {
            const int len = expand[1][j] - src;
            if (len)
                memcpy(&universe[r * cols + dst], &image[i][src], (size_t)len);
            // printf("img[%d][%3d] -> uni[%d][%3d] len=%d\n", i, src, r, dst, len);
            src += len + 1;
            dst += len + 2;
        }
    }
    #if EXAMPLE
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            printf("%c", universe[i * cols + j]);
        printf("\n");
    }
    #endif

    Vec* pos = malloc((size_t)galaxies * sizeof *pos);
    int dist = 0;
    int k = 0;
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            if (universe[i * cols + j] == '#')
                pos[k++] = (Vec){j, i};
    for (int i = 0; i < galaxies - 1; ++i)
        for (int j = i + 1; j < galaxies; ++j)
            dist += manh(pos[i], pos[j]);
    printf("Part 1: %d\n", dist);  // ex: 374, input: ?

    // int part2 = 0;
    // printf("Part 2: %d\n", part2);  // ex: ?, input: ?

    free(universe);
    free(pos);
    return 0;
}
