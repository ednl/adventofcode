/**
 * Advent of Code 2018
 * Day 2: Inventory Management System
 * https://adventofcode.com/2018/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  55 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  95 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 304 µs
 */

#include <stdio.h>
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2018-02-input.txt"
#define IDS 250  // number of IDs (lines in input file)
#define LEN 26   // ID length (characters per line in input file)
#define ALF ('z' - 'a' + 1)  // letters in the alphabet

static char id[IDS][LEN + 1];  // +'\n'

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    fread(id, sizeof id, 1, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int sum2 = 0, sum3 = 0;
    for (int i = 0; i < IDS; ++i) {
        unsigned char count[ALF] = {0};  // histogram with 26 bins, one for each letter
        for (int j = 0; j < LEN; ++j)
            count[id[i][j] - 'a']++;
        bool has2 = false, has3 = false;
        for (int j = 0; j < ALF; ++j) {
            has2 = has2 || count[j] == 2;
            has3 = has3 || count[j] == 3;
        }
        sum2 += has2;
        sum3 += has3;
    }
    printf("%u\n", sum2 * sum3);  // 5750

    for (int i = 0; i < IDS - 1; ++i)
        for (int j = i + 1; j < IDS; ++j) {
            int diff = 0;
            for (int k = 0; k < LEN; ++k)
                if (id[i][k] != id[j][k])
                    if (++diff == 2)
                        goto nextj;  // only interest4ed in diff=1
            for (int k = 0; k < LEN; ++k)  // diff=0 check not needed for my input
                if (id[i][k] == id[j][k])
                    putchar(id[i][k]);  // tzyvunogzariwkpcbdewmjhxi
            putchar('\n');
        #ifdef TIMER
            printf("Time: %.0f us\n", stoptimer_us());
        #endif
            return 0;
        nextj:;
        }
    return 1;  // no pair of IDs found that differ by one letter
}
