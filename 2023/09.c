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

#if EXAMPLE || defined(DEBUG)
    // Show reduced matrix, example:
    // 0 0 0 0 3 15
    // 0 0 0 1 6 21
    // 0 0 2 6 15 45
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j)
            printf(" %d", diff[i][j]);
        printf("\n");
    }
#endif

    // Sum all values of reduced matrix
    int sum = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            sum += diff[i][j];
    printf("Part 1: %d\n", sum);  // example: 114, input: 1861775706

    // Part 2
    memcpy(diff, data, sizeof data);
    // Reduce backward from right, collapse difference table pyramid into one line
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M - 1; ++j)  // M-1 iterations
            for (int k = M - 1; k > j; --k)  // leave first value untouched, then modify 1 fewer each iteration
                diff[i][k] = diff[i][k] - diff[i][k - 1];

#if EXAMPLE || defined(DEBUG)
    // Show reduced matrix, example:
    // 0 3 0 0 0 0
    // 1 2 1 0 0 0
    // 10 3 0 2 0 0
    // Sum: 0-3+0-0+0-0 + 1-2+1-0+0-0 + 10-3+0-2+0-0 = -3 + 0 + 5 = 2
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j)
            printf(" %d", diff[i][j]);
        printf("\n");
    }
#endif

    // Sum all values of reduced matrix by adding even columns and subtracting odd columns
    sum = 0;
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M - 1; j +=2)  // skips last column if M is odd, but last value is always zero anyway
            sum += diff[i][j] - diff[i][j + 1];
    printf("Part 2: %d\n", sum);  // example: 2, input: 1082
    return 0;
}
