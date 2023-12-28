/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 */

#include <stdio.h>    // fopen, fclose, getline, printf
#include <stdlib.h>   // free
#include <ctype.h>    // isdigit
#include <string.h>   // strncmp
#include <stdbool.h>  // bool

typedef struct Digit {
    int  val, len;
    char name[2][8];
} Digit;

// Reverse name to search reversed line
static const Digit digit[] = {
    {1, 3, {"one"  ,   "eno"}},
    {6, 3, {"six"  ,   "xis"}},
    {2, 3, {"two"  ,   "owt"}},
    {5, 4, {"five" ,  "evif"}},
    {4, 4, {"four" ,  "ruof"}},
    {9, 4, {"nine" ,  "enin"}},
    {8, 5, {"eight", "thgie"}},
    {7, 5, {"seven", "neves"}},
    {3, 5, {"three", "eerht"}},
};

static int firstdigit(const char* const msg, const int len, const bool words, const bool reversed)
{
    for (int i = 0; i < len; ++i) {
        if (isdigit(msg[i]))
            return msg[i] - '0';
        if (words)
            for (int j = 0; j < (int)(sizeof digit / sizeof *digit); ++j)
                if (!strncmp(&msg[i], digit[j].name[reversed], (size_t)digit[j].len))
                    return digit[j].val;
    }
    return 0;
}

static void rev(char* s, const int len)
{
    char *t = s + len - 1;
    while (s < t) {
        char c = *s;
        *s++ = *t;
        *t-- = c;
    }
}

int main(void)
{
    FILE* f = fopen("../aocinput/2023-01-input.txt", "r");
    if (!f)
        return 1;

    int part1 = 0, part2 = 0, len;
    char* buf = NULL;
    size_t bufsz;
    // fgets is simpler but would need another strlen()
    while ((len = (int)getline(&buf, &bufsz, f)) > 1) {
        buf[--len] = '\0';  // remove newline
        part1 += firstdigit(buf, len, 0, 0) * 10;
        part2 += firstdigit(buf, len, 1, 0) * 10;
        rev(buf, len);
        part1 += firstdigit(buf, len, 0, 1);
        part2 += firstdigit(buf, len, 1, 1);
    }
    fclose(f);
    free(buf);

    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // example: 209 281, input: 54630 54770
    return 0;
}
