/**
 * Advent of Code 2024
 * Day 5: Print Queue
 * https://adventofcode.com/2024/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 05.c
 *    gcc   -std=gnu17 -Wall -Wextra 05.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 05.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 05.c ../startstoptimer.c
 * Get minimum runtime from timer output in Bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :  95 µs
 *     Raspberry Pi 5 (2.4 GHz)                         : 142 µs
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : 241 µs
 *     Raspberry Pi 4 (1.8 GHz)                         : 496 µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE
    #define FNAME "../aocinput/2024-05-example.txt"
    #define UPDATES 6
    #define PAGES   5
#else
    #define FNAME "../aocinput/2024-05-input.txt"
    #define UPDATES 209
    #define PAGES   23
#endif

// rule[n][m] is true when n comes before m
static bool rule[100][100];
static int page[UPDATES][PAGES];
static int pages[UPDATES];

// Parse 2-digit number
static inline int num(const char *const s)
{
    return (*s & 15) * 10 + (*(s + 1) & 15);
}

// Order by the rules
static int cmp(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (rule[a][b]) return -1;  // a must come before b
    if (rule[b][a]) return  1;  // b must come before a
    return 0;  // don't care
}

int main(void)
{
#ifdef TIMER
    starttimer();
#endif

    // Parse input file
    FILE *f = fopen(FNAME, "r");
    if (!f) { fputs("File not found.\n", stderr); return 1; }
    char buf[BUFSIZ];
    for (int i = 0; fgets(buf, sizeof buf, f); ) {
        if (buf[0] == '\n')
            continue;
        if (buf[2] == '|')
            rule[num(buf)][num(buf + 3)] = true;
        else {
            page[i][0] = num(buf);
            int n = 1;
            const char *s = buf + 3;
            while (*s) {
                page[i][n++] = num(s);
                s += 3;
            }
            pages[i++] = n;
        }
    }
    fclose(f);

    int sum1 = 0, sum2 = 0;
    for (int i = 0; i < UPDATES; ++i) {
        bool ordered = true;
        for (int j = 1; j < pages[i]; ++j)
            // No need to check every pair; ordering is transitive
            if (rule[page[i][j]][page[i][j - 1]]) {  // rule says they must be swapped
                ordered = false;
                goto done;
            }
    done:;
        if (ordered) {
            sum1 += page[i][pages[i] >> 1];
        } else {
            qsort(page[i], pages[i], sizeof **page, cmp);
            sum2 += page[i][pages[i] >> 1];
        }
    }
    printf("%d %d\n", sum1, sum2);  // example: 143 123, input: 5747 5502

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
    return 0;
}
