/**
 * Advent of Code 2023
 * Day 1: Trebuchet?!
 * https://adventofcode.com/2023/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Ofast -march=native -Wall -Wextra 01.c ../startstoptimer.c
 *    gcc   -std=gnu17 -Ofast -march=native -Wall -Wextra 01.c ../startstoptimer.c
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo $m;done
 * Minimum runtime:
 *     Apple M1 Mac Mini 2020 (3.2 GHz)               : ? µs
 *     Raspberry Pi 5 (2.4 GHz)                       : ? µs
 *     Apple iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : 213 µs
 *     Raspberry Pi 4 (1.8 GHz)                       : ? µs
 */

#include <stdio.h>    // fopen, fclose, getline, printf
#include <stdlib.h>   // free
#include <string.h>   // strncmp
#include <stdbool.h>  // bool
#include "../startstoptimer.h"

static int firstdigit(const char* s, const bool words, const bool reversed)
{
    while (*s) {
        if (*s >= '1' && *s <= '9')
            return *s & 15;
        if (words)
            if (reversed) {
                switch (*s++) {
                    case 'e': if (!strncmp(s, "no"  , 2)) return 1;         // 1
                              if (!strncmp(s, "erht", 4)) return 3;         // 3
                              if (!strncmp(s, "vif" , 3)) return 5;         // 5
                              if (!strncmp(s, "nin" , 3)) return 9; break;  // 9
                    case 'o': if (!strncmp(s, "wt"  , 2)) return 2; break;  // 2
                    case 'r': if (!strncmp(s, "uof" , 3)) return 4; break;  // 4
                    case 'x': if (!strncmp(s, "is"  , 2)) return 6; break;  // 6
                    case 'n': if (!strncmp(s, "eves", 4)) return 7; break;  // 7
                    case 't': if (!strncmp(s, "hgie", 4)) return 8; break;  // 8
                }
            } else {
                switch (*s++) {
                    case 'o': if (!strncmp(s, "ne"  , 2)) return 1; break;  // 1
                    case 't': if (!strncmp(s, "wo"  , 2)) return 2;         // 2
                              if (!strncmp(s, "hree", 4)) return 3; break;  // 3
                    case 'f': if (!strncmp(s, "our" , 3)) return 4;         // 4
                              if (!strncmp(s, "ive",  3)) return 5; break;  // 5
                    case 's': if (!strncmp(s, "ix"  , 2)) return 6;         // 6
                              if (!strncmp(s, "even", 4)) return 7; break;  // 7
                    case 'e': if (!strncmp(s, "ight", 4)) return 8; break;  // 8
                    case 'n': if (!strncmp(s, "ine" , 3)) return 9; break;  // 9
                }
            }
        else
            ++s;
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
    starttimer();
    FILE* f = fopen("../aocinput/2023-01-input.txt", "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }

    int part1 = 0, part2 = 0, len;
    char* buf = NULL;
    size_t bufsz;
    // fgets is simpler but would need another strlen()
    while ((len = (int)getline(&buf, &bufsz, f)) > 1) {
        buf[--len] = '\0';  // remove newline
        part1 += firstdigit(buf, 0, 0) * 10;
        part2 += firstdigit(buf, 1, 0) * 10;
        rev(buf, len);
        part1 += firstdigit(buf, 0, 1);
        part2 += firstdigit(buf, 1, 1);
    }
    fclose(f);
    free(buf);

    printf("Part 1: %d\nPart 2: %d\n", part1, part2);  // example: 209 281, input: 54630 54770
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
