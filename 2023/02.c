/**
 * Advent of Code 2023
 * Day 2: Cube Conundrum
 * https://adventofcode.com/2023/day/2
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, getline, sscanf, printf
#include <stdlib.h>   // free
#include <string.h>   // strtok
#include <stdbool.h>  // bool

#define R 12
#define G 13
#define B 14

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
        ++game;  // game numbers are consecutive & identical to line number
        char* token = buf;  // buf content will be destroyed but pointer is saved
        while (*token++ != ':');  // one past ':' = skip the space
        token = strtok(token, ",;");  // subsets don't matter, so tokenise at all separators
        bool possible = token != NULL;  // part 1: is the game possible with R,G,B limits?
        int maxrgb[3] = {0};  // part 2: maximum number of cubes per colour per game
        while (token) {  // for part 1 only, this could have been a shortcut "token && possible"
            int cubes;
            char colour;
            if (sscanf(token, "%d %c", &cubes, &colour) == 2) {  // %d absorbs any leading space
                switch (colour) {
                    // for part 1 only with the above shortcut, it could have been "possible = cubes <= R;" etc.
                    case 'r': possible = possible && cubes <= R; if (cubes > maxrgb[0]) maxrgb[0] = cubes; break;
                    case 'g': possible = possible && cubes <= G; if (cubes > maxrgb[1]) maxrgb[1] = cubes; break;
                    case 'b': possible = possible && cubes <= B; if (cubes > maxrgb[2]) maxrgb[2] = cubes; break;
                    default : possible = false;
                }
            } else
                possible = false;
            token = strtok(NULL, ",;");  // next pair of number+colour
        }
        if (possible)
            part1 += game;
        part2 += maxrgb[0] * maxrgb[1] * maxrgb[2];
    }
    fclose(f);
    free(buf);

    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // 2105 72422
    return 0;
}
