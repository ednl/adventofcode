/**
 * Advent of Code 2023
 * Day 9: Mirage Maintenance
 * https://adventofcode.com/2023/day/9
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf
#include <string.h>  // memcpy

#define EXAMPLE 0
#if EXAMPLE
    #define NAME "../aocinput/2023-09-example.txt"
    #define N   3  // sensors
    #define M   6  // measurements
#else
    #define NAME "../aocinput/2023-09-input.txt"
    #define N 200  // sensors
    #define M  21  // measurements
#endif

static int data[N][M], diff[N][M];

// Show reduced matrix = difference table on one line
static void showtable(void)
{
#if EXAMPLE || defined(DEBUG)
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j)
            printf(" %d", diff[i][j]);
        printf("\n");
    }
    printf("\n");
#endif
}

int main(void)
{
    // Read data from file
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            fscanf(f, "%d", &data[i][j]);
    fclose(f);

    // Part 1
    memcpy(diff, data, sizeof data);

    // Reduce forward from left, collapse difference table pyramid into one line
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M - 1; ++j)  // M-1 iterations
            for (int k = 0; k < M - j - 1; ++k)  // leave last value untouched, then modify 1 fewer each iteration
                diff[i][k] = diff[i][k + 1] - diff[i][k];

    showtable();
    // 0 0 0 0 3 15
    // 0 0 0 1 6 21
    // 0 0 2 6 15 45

    // Sum all values of reduced matrix
    int part1 = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            part1 += diff[i][j];

    // Part 2
    memcpy(diff, data, sizeof data);

    // Reduce backward from right, collapse difference table pyramid into one line
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M - 1; ++j)  // M-1 iterations
            for (int k = M - 1; k > j; --k)  // leave first value untouched, then modify 1 fewer each iteration
                diff[i][k] = diff[i][k] - diff[i][k - 1];

    showtable();
    // 0 3 0 0 0 0
    // 1 2 1 0 0 0
    // 10 3 0 2 0 0

    // Sum all values of reduced matrix by adding even columns and subtracting odd columns
    // Example: 0-3+0-0+0-0 + 1-2+1-0+0-0 + 10-3+0-2+0-0 = -3 + 0 + 5 = 2
    int part2 = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M - 1; j +=2)  // skips last column if M is odd, but last value is always zero anyway
            part2 += diff[i][j] - diff[i][j + 1];

    printf("Part 1: %d\n", part1);  // example: 114, input: 1861775706
    printf("Part 2: %d\n", part2);  // example: 2  , input: 1082
    return 0;
}
