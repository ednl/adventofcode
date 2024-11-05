/**
 * Advent of Code 2015
 * Day 5: Doesn't He Have Intern-Elves For This?
 * https://adventofcode.com/2015/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 05.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 05.c
 */

#include <stdio.h>
#include <stdint.h>  // int16_t
#include <stdbool.h>

#define LEN 16  // string length of every line in input

int main(void)
{
    FILE *f = fopen("../aocinput/2015-05-input.txt", "r");
    if (!f)
        return 1;
    char line[LEN << 1];
    int nice1 = 0, nice2 = 0;
    while (fgets(line, sizeof line, f)) {
        // Part 1
        bool repeat = false;
        for (int i = 0; !repeat && i < LEN - 1; ++i)
            repeat = line[i] == line[i + 1];
        if (repeat) {
            int vowels = 0;
            for (int i = 0; vowels < 3 && i < LEN; ++i)
                switch (line[i]) {
                    case 'a':
                    case 'e':
                    case 'i':
                    case 'o':
                    case 'u': ++vowels; break;
                }
            if (vowels == 3) {
                bool naughty = false;
                for (int i = 0; !naughty && i < LEN - 1; ++i)
                    switch (line[i]) {
                        case 'a':
                        case 'c':
                        case 'p':
                        case 'x': naughty = line[i + 1] == line[i] + 1; break;
                    }
                nice1 += !naughty;
            }
        }
        // Part 2
        bool between = false;
        for (int i = 0; !between && i < LEN - 2; ++i)
            between = line[i] == line[i + 2];
        if (between) {
            bool twopair = false;
            for (int i = 0; !twopair && i < LEN - 3; ++i) {
                const int16_t pair1 = *(int16_t *)&line[i];
                for (int j = i + 2; !twopair && j < LEN - 1; ++j)
                    twopair = pair1 == *(int16_t *)&line[j];
            }
            nice2 += twopair;
        }
    }
    fclose(f);
    printf("%d %d\n", nice1, nice2);  // 258 53
    return 0;
}
