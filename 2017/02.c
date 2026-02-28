/**
 * Advent of Code 2017
 * Day 2: Corruption Checksum
 * https://adventofcode.com/2017/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.83 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :    ? µs
 */

#include <stdio.h>
#include <stdint.h>    // uint16_t
#include <inttypes.h>  // SCNu16
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-02-input.txt"
#define COLS 16
#define ROWS 16

static uint16_t sheet[ROWS][COLS];

static uint16_t chkdiff(const uint16_t *const row)
{
    uint16_t min = row[0];
    uint16_t max = row[0];
    for (int i = 1; i < COLS; ++i) {
        if (row[i] < min) min = row[i];
        if (row[i] > max) max = row[i];
    }
    return max - min;
}

static uint16_t chkdiv(const uint16_t *const row)
{
    for (int i = 0; i < COLS - 1; ++i)
        for (int j = i + 1; j < COLS; ++j) {
            if (row[i] % row[j] == 0) return row[i] / row[j];
            if (row[j] % row[i] == 0) return row[j] / row[i];
        }
    return 0;
}

int main(void)
{
    FILE *f = fopen(FNAME, "r");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS && fscanf(f, "%"SCNu16, &sheet[i][j]) == 1; ++j);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Part 1: sum differences of min/max per row
    unsigned checksum = 0;
    for (int i = 0; i < ROWS; ++i)
        checksum += chkdiff(&sheet[i][0]);
    printf("%u\n", checksum);  // 34925

    // Part 2: sum one division per row where remainder is zero
    checksum = 0;
    for (int i = 0; i < ROWS; ++i)
        checksum += chkdiv(&sheet[i][0]);
    printf("%u\n", checksum);  // 221

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
