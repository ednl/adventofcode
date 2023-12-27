/**
 * Advent of Code 2023
 * Day 9: Mirage Maintenance
 * https://adventofcode.com/2023/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 09.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 09.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Raspberry Pi 5 (2.4 GHz)                       : ? µs
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               : ? µs
 *     Raspberry Pi 4 (1.8 GHz)                       : ? µs
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : 637 µs
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf
#include "../startstoptimer.h"

#define NAME "../aocinput/2023-09-input.txt"
#define N 200  // sensors (lines in input file)
#define M  21  // measurements (numbers per line)

static int data[N][M][M];  // second dimension is for difference table

int main(void)
{
    starttimer();
    FILE* f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < M; ++j)
            fscanf(f, "%d", &data[i][0][j]);  // measurements in row 0 of difference table
    fclose(f);

    int part1 = 0, part2 = 0;
    for (int i = 0; i < N; ++i) {            // sensors
        part1 += data[i][0][M - 1];          // last measurement
        part2 += data[i][0][0];              // first measurement
        for (int k = 1, s = -1; k < M; ++k, s = -s) {  // difference table except row 0
            for (int j = 0; j < M - k; ++j)  // differences, 1 fewer every row
                data[i][k][j] = data[i][k - 1][j + 1] - data[i][k - 1][j];
            part1 += data[i][k][M - k - 1];  // last difference
            part2 += data[i][k][0] * s;      // first difference, with alternating sign
        }
    }

    printf("%d %d\n", part1, part2);  // 1861775706, 1082
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
