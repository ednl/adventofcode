/**
 * Advent of Code 2024
 * Day 2: Red-Nosed Reports
 * https://adventofcode.com/2024/day/2
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 02.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 02.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 18.9 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 31.8 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 48.7 µs
 */

#include <stdio.h>
#include <stdlib.h>  // abs
#include <stdint.h>  // uint8_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-02-input.txt"
#define FSIZE ((1<<14)|(1<<12))  // needed for my input: 19161
#define REPORTS 1000  // lines in input file
#define LEVELS  8  // max numbers per line
#define MINDIST 1
#define MAXDIST 3

static char input[FSIZE];
static uint8_t data[REPORTS][LEVELS];  // input file parsed
static uint8_t count[REPORTS];         // number of levels in each report

// Parse 1- or 2-digit number, update string pointer
static unsigned readnum(const char **const s)
{
    unsigned x = *(*s)++ & 15;
    if (**s & 16)  // not space or newline
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

// Direction change from level a to b: +1 for a<b, -1 for a>b, 0 for a=b
static int change(const int a, const int b)
{
    return (a < b) - (a > b);
}

// Is this report safe? It has 'count' levels. If 'skip' is a valid index, skip it.
static bool issafe(const uint8_t *const level, const int count, const int skip)
{
    const int end = count - 1 - (skip == count - 1);  // limit for i when comparing level[i] and level[i+1]
    int sumchange = 0;
    for (int i = 0; i < end; ++i)
        if (i != skip) {
            const int j = i + 1 + (skip == i + 1);  // compare with next level, or skip 1
            const int distance = abs(level[i] - level[j]);
            if (distance < MINDIST || distance > MAXDIST)
                return false;
            sumchange += change(level[i], level[j]);  // -1,0,+1
        }
    const int len = count - 1 - (skip != -1);  // number of intervals between levels
    return abs(sumchange) == len;  // changes must be all -1 or all +1
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fputs("File not found: "FNAME, stderr); return EXIT_FAILURE; }
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    const char *c = input;
    for (int i = 0; *c; ++i)
        for (int j = 0;;) {
            data[i][j++] = readnum(&c);
            if (*c++ == '\n') {
                count[i] = j;
                break;
            }
        }

    int safe = 0, damp = 0;
    for (int i = 0; i < REPORTS; ++i)  // for every report
        for (int skip = -1; skip < count[i]; ++skip)  // try different versions
            if (issafe(data[i], count[i], skip)) {
                safe += skip == -1;  // part 1
                damp++;              // part 2
                break;               // stop at first safe version
            }
    printf("%u %u\n", safe, damp);  // 516 561

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
