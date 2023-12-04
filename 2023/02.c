/**
 * Advent of Code 2023
 * Day 2: Cube Conundrum
 * https://adventofcode.com/2023/day/2
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, getline, sscanf, printf
#include <stdlib.h>   // free
#include <string.h>   // strtok

// Limits per colour as imposed by the puzzle
static const int rgblim[3] = {12, 13, 14};

int main(void)
{
    FILE* f = fopen("../aocinput/2023-02-input.txt", "r");
    if (!f)
        return 1;

    char* buf = NULL;
    size_t bufsz;
    ssize_t len;
    int part1 = 0, part2 = 0, game = 0;

    while ((len = getline(&buf, &bufsz, f)) > 1) {
        ++game;  // game numbers are consecutive & identical to line number, so no parsing
        char* token = buf;  // buf content will be destroyed but pointer is saved
        while (*token++ != ':');  // goes one past ':'
        token = strtok(token, ",;");  // subsets don't matter, so tokenise at all separators
        int rgbmax[3] = {0};  // maximum number of cubes per colour per game
        while (token) {
            int cubes;
            char colour;
            if (sscanf(token, "%d %c", &cubes, &colour) == 2)  // %d absorbs leading space
                switch (colour) {
                    case 'r': if (cubes > rgbmax[0]) rgbmax[0] = cubes; break;
                    case 'g': if (cubes > rgbmax[1]) rgbmax[1] = cubes; break;
                    case 'b': if (cubes > rgbmax[2]) rgbmax[2] = cubes; break;
                }
            token = strtok(NULL, ",;");  // next pair of number+colour
        }
        if (rgbmax[0] <= rgblim[0] && rgbmax[1] <= rgblim[1] && rgbmax[2] <= rgblim[2])
            part1 += game;
        part2 += rgbmax[0] * rgbmax[1] * rgbmax[2];
    }
    fclose(f);
    free(buf);

    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // 2105 72422
    return 0;
}
