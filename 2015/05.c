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
#include <string.h>  // strchr
#include <stdbool.h>

int main(void)
{
    FILE *f = fopen("../aocinput/2015-05-input.txt", "r");
    if (!f)
        return 1;
    char line[BUFSIZ];
    int nice = 0;
    while (fgets(line, sizeof line, f)) {
        int vowels = 0;
        bool repeated = false, naughty = false;
        char *c = line;
        while (*c) {
            vowels += vowels < 3 && strchr("aeiou", *c) != NULL;
            repeated = repeated || *c == *(c + 1);
            if (!naughty)
                switch (*c) {
                    case 'a':
                    case 'c':
                    case 'p':
                    case 'x': naughty = *(c + 1) == *c + 1; break;
                }
            ++c;
        }
        nice += vowels >= 3 && repeated && !naughty;
    }
    fclose(f);
    printf("Part 1: %d\n", nice);  // 258
    return 0;
}
