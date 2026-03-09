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
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 353 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-09-input.txt"

int main(void)
{
    FILE *f = isatty(fileno(stdin)) ? fopen(FNAME, "r") : stdin;
    if (!f)
        return 1;

#ifdef TIMER
    starttimer();
#endif

    int group = 0;
    int score = 0;
    int count = 0;
clean:
    switch (fgetc(f)) {
        case -1  : /* fall-through */
        case '\0': /* fall-through */
        case '\n':                   goto done;
        case '<' :                   goto garbage;
        case '{' : group++;          goto clean;
        case '}' : score += group--; goto clean;
        default  :                   goto clean;
    }
garbage:
    switch (fgetc(f)) {
        case '!': fgetc(f); goto garbage;
        case '>':           goto clean;
        default : count++;  goto garbage;
    }
done:
    printf("%d %d\n", score, count);  // 20530 9978

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif

    if (isatty(fileno(stdin)))
        fclose(f);
}
