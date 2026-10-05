/**
 * Advent of Code 2024
 * Day 5: Print Queue
 * https://adventofcode.com/2024/day/5
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 05.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 05.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 11.9 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 26.1 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 83.6 µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdlib.h>  // qsort
#include <stdint.h>  // uint8_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-05-input.txt"
#define FSIZE ((1<<14)|(1<<8))  // 16640, needed for my input: 16408
#define UPDATES 209
#define PAGES   23

static char input[FSIZE];
// Rule[a][b] is true when a should come before b
static bool rule[100][100];           // every pair of 2-digit numbers (and 1)
static uint8_t page[UPDATES][PAGES];  // every page of every "update"
static unsigned pagecount[UPDATES];   // actual number of pages in this "update"

// Parse 2-digit number
static unsigned readnum(const char *const s)
{
    return *s * 10 + *(s + 1) - '0' * 11;
}

// Order by the rules
static int cmp(const void *p, const void *q)
{
    const uint8_t a = *(const uint8_t *)p;
    const uint8_t b = *(const uint8_t *)q;
    if (rule[a][b]) return -1;  // a must come before b
    if (rule[b][a]) return  1;  // b must come before a
    return 0;  // don't care
}

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");  // fread() requires binary mode
        if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
        fread(input, 1, sizeof input, f);  // read single bytes until EOF
        fclose(f);
    } else
        // Read input or example file from pipe or redirected stdin
        fread(input, 1, sizeof input, stdin);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    for (; *c & 16; c += 6)  // until blank line
        rule[readnum(c)][readnum(c + 3)] = true;  // pair ordered as a<b
    // Start second part at blank line: *c=='\n'
    for (unsigned i = 0; *(c + 1); ++i) {
        unsigned j = 0;
        do {
            page[i][j++] = (uint8_t)readnum(c + 1);
            c += 3;
        } while (*c == ',');
        pagecount[i] = j;
    }

    unsigned sum1 = 0, sum2 = 0;
    for (unsigned i = 0; i < UPDATES; ++i) {
        for (unsigned j = 1; j < pagecount[i]; ++j)
            // No need to check every pair, only consecutive ones; without loops,
            // ordering is transitive (if a<b and b<c then a<c) and so, for the final
            // order to be uniquely determined, there can be no loops. Or at least not
            // across the middle element we want; but my input was nice enough.
            if (rule[ page[i][j] ][ page[i][j - 1] ]) {
                // Pages out of order, so this is part 2
                qsort(&page[i][0], pagecount[i], sizeof **page, cmp);
                sum2 += page[i][pagecount[i] >> 1];  // pick middle element
                goto next_i;  // break + continue
            }
        // All pages were ordered, so this is part 1
        sum1 += page[i][pagecount[i] >> 1];  // pick middle element
    next_i:;
    }
    printf("%u %u\n", sum1, sum2);  // 5747 5502

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
