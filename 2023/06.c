/**
 * Advent of Code 2023
 * Day 6: Wait For It
 * https://adventofcode.com/2023/day/6
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>     // fopen, fclose, fscanf, printf
#include <math.h>      // sqrt, floor, ceil
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64

#define EXAMPLE 0
#if EXAMPLE == 1
    #define NAME "../aocinput/2023-06-example.txt"
    #define RACES 3
#else
    #define NAME "../aocinput/2023-06-input.txt"
    #define RACES 4
#endif
#define T 0  // time index
#define D 1  // dist index
#define L 2  // number of parameters

static int64_t race[RACES][L];

static int64_t ways2win(int raceid)
{
    const int64_t t = race[raceid][T];
    const int64_t d = race[raceid][D];
    const double sq = (double)t / 4 * t - d;
    if (sq < 0)
        return 0;
    const double t_mid = (double)t / 2;
    const double width = sqrt(sq);
    const double f1 =  ceil(t_mid - width);
    const double f2 = floor(t_mid + width);
    int64_t button1 = (int64_t)f1;
    int64_t button2 = (int64_t)f2;
    if (button1 * (t - button1) == d) {
        ++button1;
        --button2;
    }
    if (button1 <= 0)
        button1 = 1;
    return button1 <= button2 ? button2 - button1 + 1 : 0;
}

int main(void)
{
    FILE* f = fopen(NAME, "r");
    if (!f)
        return 1;
    for (int i = 0; i < L; ++i) {  // two lines
        while (fgetc(f) != ':');  // skip to first number
        for (int j = 0; j < RACES; ++j)
            fscanf(f, "%"PRId64, &race[j][i]);  // transpose
    }
    fclose(f);

    // Part 1
    int64_t prod = 1;
    for (int i = 0 ; i < RACES; ++i)
        prod *= ways2win(i);

    // Part 2
    for (int i = 0; i < L; ++i)
        for (int j = 1; j < RACES; ++j) {
            // Collate into first race
            int64_t mult = 10;
            while (mult <= race[j][i])
                mult *= 10;
            race[0][i] = race[0][i] * mult + race[j][i];
        }

    printf("Part 1: %"PRId64"\n", prod);         // example:   288, input:  2449062
    printf("Part 2: %"PRId64"\n", ways2win(0));  // example: 71503, input: 33149631
    return 0;
}
