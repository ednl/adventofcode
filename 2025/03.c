/**
 * Advent of Code 2025
 * Day 3: Lobby
 * https://adventofcode.com/2025/day/3
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 03.c
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 03.c
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  41 µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2025-03-example.txt"
    #define ROWS 4
    #define COLS 15
#else
    #define FNAME "../aocinput/2025-03-input.txt"
    #define ROWS 200
    #define COLS 100
#endif

static char input[ROWS][COLS + 1];  // +'\n'

static int64_t joltage(const int len)
{
    int64_t sum = 0;
    for (int i = 0; i < ROWS; ++i) {  // process all rows separately
        int64_t num = 0;  // len-digit number on this row
        for (int key = -1, last = COLS - len; last < COLS; ++last) {  // count digits 1..len
            char max = 0;  // best value found
            for (int j = key + 1; j <= last; ++j)  // leave room for remaining digits
                if (input[i][j] > max && (max = input[i][(key = j)]) == '9')
                    break;  // can't get higher than '9'
            num = num * 10 + (max & 15);
        }
        sum += num;
    }
    return sum;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    printf("%"PRId64" %"PRId64"\n",
        joltage(2),    // example: 357, input: 17324
        joltage(12));  // example: 3121910778619, input: 171846613143331

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
