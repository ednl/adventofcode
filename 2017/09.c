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
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output on stderr:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 13.9 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <unistd.h>  // isatty, fileno
#ifdef TIMER
    #include "../startstoptimer.h"
    #define LOOPS 1000
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
    starttimer(); for (int loop = 0; loop < LOOPS; ++loop) {
    score = count = 0;
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
        case '!': c++;      goto garbage;
        case '>':           goto clean;
        default : count++;  goto garbage;
    }
done:
    printf("%d %d\n", score, count);  // 20530 9978

#ifdef TIMER
    } fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());
#endif
}
