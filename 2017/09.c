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
 * Test with timer enabled, without a thousand lines of identical output:
 *     ./a.out | head -n1
 * Get minimum runtime from timer output on stderr:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 25.7 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdbool.h>
#ifdef TIMER
#include "../startstoptimer.h"
#define LOOPS 1000  // 1000 timer loops (measuring ms means µs per loop)
#endif

#define FNAME "../aocinput/2017-09-input.txt"
#define FSIZE 32768 // needed for my input: 30816 (incl. newline)

static char input[FSIZE];

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen(FNAME, "rb");
        if (!f) {
            fprintf(stderr, "File not found: "FNAME"\n");
            return 1;
        }
        fread(input, 1, FSIZE, f);
        fclose(f);
    } else
        // Read input file from stdin pipe/redirection
        fread(input, 1, FSIZE, stdin);

#ifdef TIMER
starttimer(); for (int loop = 0; loop < LOOPS; ++loop) {
#endif

    int group = 0;
    int score = 0;
    int count = 0;
    bool garbage = false;
    for (const char *c = input; *c; ++c)
        switch (*c) {
            case '!': c += garbage; break;
            case '<': count += garbage; garbage = true; break;
            case '>': garbage = false; break;
            case '{': count += garbage; group += !garbage; break;
            case '}': if (garbage) count++; else score += group--; break;
            default : count += garbage; break;
        }
    printf("%d %d\n", score, count);  // 20530 9978

#ifdef TIMER
// 1000 loops means µs measurement is ns per loop
} fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
