/**
 * Advent of Code 2023
 * Day 9: Mirage Maintenance
 * https://adventofcode.com/2023/day/9
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf

#define NAME "../aocinput/2023-09-input.txt"
#define N 200  // sensors
#define M  21  // measurements

static int data[N][M][M];  // second dimension is for difference table

int main(void)
{
    FILE* f = fopen(NAME, "r");
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            fscanf(f, "%d", &data[i][0][j]);  // measurements in row 0 of difference table
    fclose(f);

    int part1 = 0, part2 = 0;
    for (int i = 0; i < N; ++i) {             // sensors
        part1 += data[i][0][M - 1];           // last measurement
        part2 += data[i][0][0];               // first measurement
        for (int k = 1, s = -1; k < M; ++k, s = -s) {  // difference table except row 0
            for (int j = 0; j < M - k; ++j)   // differences, 1 fewer every row
                data[i][k][j] = data[i][k - 1][j + 1] - data[i][k - 1][j];
            part1 += data[i][k][M - k - 1];   // last difference
            part2 += data[i][k][0] * s;       // first difference, with alternating sign
        }
    }

    printf("%d %d\n", part1, part2);  // 1861775706, 1082
    return 0;
}
