/**
 * Advent of Code 2020
 * Day 6: Custom Customs
 * https://adventofcode.com/2020/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 11.5 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 22.3 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 38.7 µs
 */

#include <stdio.h>
#include <string.h>  // memset
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-06-input.txt"
#define FSIZE 18432  // needed for my input: 17234
#define QUESTIONS ('z' - 'a' + 1)

static char input[FSIZE];
static char yes[QUESTIONS];  // histogram

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    // Read char-by-char to unknown size, save size
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int any = 0, all = 0, people = 0;
    for (const char *c = input; c != end; ) {
        people++;  // start on non-empty line
        while (*c != '\n')
            yes[*c++ - 'a']++;
        c++;  // skip newline
        if (c == end || *c == '\n') {
            for (int i = 0; i < QUESTIONS; ++i) {
                any += yes[i] != 0;
                all += yes[i] == people;
            }
            if (c != end) {
                people = 0;
                memset(yes, 0, sizeof yes);
                c++;  // skip empty line
            }
        }
    }
    printf("%d %d\n", any, all);  // 6587 3235

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
