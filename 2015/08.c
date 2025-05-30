/**
 * Advent of Code 2015
 * Day 8: Matchsticks
 * https://adventofcode.com/2015/day/8
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 08.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 08.c
 */

#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    FILE *f = fopen("../aocinput/2015-08-input.txt", "r");
    if (!f)
        return 1;

    char *buf = NULL;
    size_t bufsz;
    ssize_t len;
    int lines = 0, decode = 0, encode = 0;

    while ((len = getline(&buf, &bufsz, f)) > 2) {  // line must be at least ""+EOL (len=3)
        ++lines;                                    // count lines
        for (int i = 1; i < len - 2; ++i)           // skip first and last ""
            if (buf[i] == '\\') {                   // start of encoded char?
                if (buf[++i] == 'x') {              // hex char?
                    i += 2;                         // skip the hex digits
                    decode += 3;                    // 4 char in code = 1 char in memory
                    encode += 1;                    // only encode the '\'
                } else {
                    decode += 1;                    // 2 char in code = 1 char in memory
                    encode += 2;                    // encode both '\' and next char
                }
            }
    }
    free(buf);
    fclose(f);

    decode += lines * 2;  // remove first and last "" on every line
    encode += lines * 4;  // wrap first and last "" on every line

    printf("Part 1: %d\n", decode);  // 1350
    printf("Part 2: %d\n", encode);  // 2085
    return 0;
}
