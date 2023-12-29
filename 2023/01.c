/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 01.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra -Wno-misleading-indentation 01.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               : 124 µs
 *     Raspberry Pi 5 (2.4 GHz)                       : 159 µs
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : 199 µs
 *     Raspberry Pi 4 (1.8 GHz)                       : 339 µs
 */

#include <stdio.h>    // fopen, fclose, getline, printf
#include <stdlib.h>   // free
#include <string.h>   // strncmp
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

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
    starttimer();
    FILE* f = fopen("../aocinput/2023-01-input.txt", "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    int part1 = 0, part2 = 0, len;
    char* buf = NULL;
    size_t bufsz;
    // fgets is simpler but would need another strlen() to reverse
    while ((len = (int)getline(&buf, &bufsz, f)) > 1) {
        buf[--len] = '\0';  // remove newline

        char* s = buf;
        while (*s > '9') ++s;  // every line has at least one numerical digit
        int num = (*s & 15) * 10;
        int val = 0;
        for (char* t = buf; t < s - 2 && !val; )
            switch (*t++) {
                case 'o': if (!strncmp(t, "ne"  , 2)) val = 10; break;  // 1
                case 't': if (!strncmp(t, "wo"  , 2)) val = 20; else    // 2
                          if (!strncmp(t, "hree", 4)) val = 30; break;  // 3
                case 'f': if (!strncmp(t, "our" , 3)) val = 40; else    // 4
                          if (!strncmp(t, "ive",  3)) val = 50; break;  // 5
                case 's': if (!strncmp(t, "ix"  , 2)) val = 60; else    // 6
                          if (!strncmp(t, "even", 4)) val = 70; break;  // 7
                case 'e': if (!strncmp(t, "ight", 4)) val = 80; break;  // 8
                case 'n': if (!strncmp(t, "ine" , 3)) val = 90; break;  // 9
            }
        part1 += num;
        part2 += val ? val : num;

        rev(buf, len);

        s = buf;
        while (*s > '9') ++s;
        num = *s & 15;
        val = 0;
        for (char* t = buf; t < s - 2 && !val; )
            switch (*t++) {
                case 'e': if (!strncmp(t, "no"  , 2)) val = 1; else    // 1
                          if (!strncmp(t, "erht", 4)) val = 3; else    // 3
                          if (!strncmp(t, "vif" , 3)) val = 5; else    // 5
                          if (!strncmp(t, "nin" , 3)) val = 9; break;  // 9
                case 'o': if (!strncmp(t, "wt"  , 2)) val = 2; break;  // 2
                case 'r': if (!strncmp(t, "uof" , 3)) val = 4; break;  // 4
                case 'x': if (!strncmp(t, "is"  , 2)) val = 6; break;  // 6
                case 'n': if (!strncmp(t, "eves", 4)) val = 7; break;  // 7
                case 't': if (!strncmp(t, "hgie", 4)) val = 8; break;  // 8
            }
        part1 += num;
        part2 += val ? val : num;
    }
    fclose(f);
    free(buf);

    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // example: 209 281, input: 54630 54770
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
