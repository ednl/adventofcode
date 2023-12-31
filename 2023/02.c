/**
 * Advent of Code 2023
 * Day 2: Cube Conundrum
 * https://adventofcode.com/2023/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 02.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 02.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)          : ? µs
 *     Raspberry Pi 5 (2.4 GHz)            : ? µs
 *     iMac 2013 (i5 Haswell 4570 3.2 GHz) : 182 µs
 *     Raspberry Pi 4 (1.8 GHz)            : ? µs
 */

#include <stdio.h>    // fopen, fclose, getline, sscanf, printf
#include <string.h>   // strtok
#include "../startstoptimer.h"

// Limits per colour as imposed by the puzzle
static const unsigned rgblim[3] = {12, 13, 14};

static unsigned max(const unsigned a, const unsigned b)
{
    return a > b ? a : b;
}

int main(void)
{
    starttimer();
    FILE* f = fopen("../aocinput/2023-02-input.txt", "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    char buf[256];
    unsigned part1 = 0, part2 = 0, game = 0;
    while (fgets(buf, sizeof buf, f)) {
        ++game;  // game numbers are consecutive & identical to line number, so no parsing
        char* token = buf;  // buf content will be destroyed but pointer is saved
        while (*token++ != ':');  // goes one past ':'
        token = strtok(token, ",;");  // subsets don't matter, so tokenise at all separators
        unsigned rgbmax[3] = {0};  // maximum number of cubes per colour per game
        while (token) {
            unsigned cubes;
            char colour;
            if (sscanf(token, "%u %c", &cubes, &colour) == 2)  // %d absorbs leading space
                switch (colour) {
                    case 'r': rgbmax[0] = max(rgbmax[0], cubes); break;
                    case 'g': rgbmax[1] = max(rgbmax[1], cubes); break;
                    case 'b': rgbmax[2] = max(rgbmax[2], cubes); break;
                }
            token = strtok(NULL, ",;");  // next pair of number+word
        }
        if (rgbmax[0] <= rgblim[0] && rgbmax[1] <= rgblim[1] && rgbmax[2] <= rgblim[2])
            part1 += game;
        part2 += rgbmax[0] * rgbmax[1] * rgbmax[2];
    }
    fclose(f);
    printf("Part 1: %u\nPart 2: %u\n", part1, part2);  // 2105 72422
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
