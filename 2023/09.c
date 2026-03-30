/**
 * Advent of Code 2023
 * Day 9: Mirage Maintenance
 * https://adventofcode.com/2023/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 09.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09.c
 * Get minimum runtime from timer output in bash:
 *     m=9999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 168 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 253 µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>   // fopen, fclose, fscanf, printf
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define NAME "../aocinput/2023-09-input.txt"
#define N 200  // sensors (lines in input file)
#define M  21  // measurements (numbers per line)

static int table[M][M];  // difference table

int main(void)
{
    FILE *f = fopen(NAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

#ifdef TIMER
    starttimer();
#endif

    int part1 = 0, part2 = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j)
            fscanf(f, "%d", &table[0][j]);   // measurements in row 0 of difference table
        part1 += table[0][M - 1];            // last measurement
        part2 += table[0][0];                // first measurement
        for (int k = 1, s = -1; k < M; ++k, s = -s) {  // difference table except row 0
            for (int j = 0; j < M - k; ++j)  // differences, 1 fewer every row
                table[k][j] = table[k - 1][j + 1] - table[k - 1][j];
            part1 += table[k][M - k - 1];    // last difference
            part2 += table[k][0] * s;        // first difference, with alternating sign
        }
    }
    printf("%d %d\n", part1, part2);  // 1861775706, 1082

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
