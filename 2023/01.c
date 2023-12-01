/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>   // fopen, fclose, getline, printf
#include <stdlib.h>  // free
#include <ctype.h>   // isdigit
#include <string.h>  // strlen

static const char* name[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
static int namelen[10];

static int first(const char* const msg, const ssize_t len, const int part)
{
    for (int i = 0; i < len; ++i) {
        if (isdigit(msg[i]))
            return msg[i] - '0';
        if (part == 2)
            for (int digit = 1; digit <= 9; ++digit) {
                int j = i, k = 0;
                while (j < len && k < namelen[digit] && msg[j] == name[digit][k]) {
                    ++j;
                    ++k;
                }
                if (k == namelen[digit])
                    return digit;
            }
    }
    return 0;
}

static int last(const char* const msg, const ssize_t len, const int part)
{
    for (int i = (int)len - 1; i >= 0; --i) {
        if (isdigit(msg[i]))
            return msg[i] - '0';
        if (part == 2)
            for (int digit = 1; digit <= 9; ++digit) {
                int j = i, k = namelen[digit] - 1;
                while (j >= 0 && k >= 0 && msg[j] == name[digit][k]) {
                    --j;
                    --k;
                }
                if (k == -1)
                    return digit;
            }
    }
    return 0;
}

static int calibration(const char* const msg, const ssize_t len, const int part)
{
    return first(msg, len, part) * 10 + last(msg, len, part);
}

int main(void)
{
    FILE* f = fopen("../aocinput/2023-01-input.txt", "r");
    if (!f)
        return 1;

    for (int i = 1; i <= 9; ++i)
        namelen[i] = (int)strlen(name[i]);

    int part1 = 0, part2 = 0;
    char* buf = NULL;
    size_t bufsz;
    ssize_t len;
    while ((len = getline(&buf, &bufsz, f)) > 1) {
        buf[--len] = '\0';  // remove newline
        part1 += calibration(buf, len, 1);
        part2 += calibration(buf, len, 2);
    }
    fclose(f);
    free(buf);

    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // example: 209 281, input: 54630 54770
    return 0;
}
