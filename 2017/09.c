/**
 * Advent of Code 2017
 * Day 9: Stream Processing
 * https://adventofcode.com/2017/day/9
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 09.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 09.c
 * Test with timer enabled, without a million lines of identical output:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output on stderr:
 *     ./a.out 2>&1 1>/dev/null
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 13.4 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :    ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 78.2 µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#ifdef TIMER
    #include "../startstoptimer.h"
    // With inner loop of ~14 µs, one timing run with 1e5 loops takes about 1.5 seconds
    #define LOOPS 100000
#endif

#define FNAME "../aocinput/2017-09-input.txt"
#define FSIZE 32768 // needed for my input: 30816 (incl. newline)

static char input[FSIZE];
static int group, score, count;

int main(void)
{
    if (isatty(fileno(stdin))) {
        FILE *f = fopen(FNAME, "rb");
        if (!f) {
            fprintf(stderr, "File not found: "FNAME"\n");
            return 1;
        }
        fread(input, 1, FSIZE, f);
        fclose(f);
    } else
        fread(input, 1, FSIZE, stdin);

#ifdef TIMER
    double mintime = 1e9;
    for (int loop = 0; loop < LOOPS; ++loop) {
        score = count = 0;  // re-init before every timing loop
        starttimer();
#endif

    const char *c = input;
clean:
    switch (*c++) {
        case '\0': /* fall-through */
        case '\n':                   goto done;
        case '<' :                   goto garbage;
        case '{' : group++;          goto clean;
        case '}' : score += group--; goto clean;
        default  :                   goto clean;
    }
garbage:
    switch (*c++) {
        case '!': c++;     goto garbage;
        case '>':          goto clean;
        default : count++; goto garbage;
    }
done:
    printf("%d %d\n", score, count);  // 20530 9978

#ifdef TIMER
        const double us = stoptimer_us();
        if (us < mintime)
            mintime = us;
    }
    fprintf(stderr, "Time: %.1f us\n", mintime);
#endif
}
