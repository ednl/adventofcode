/**
 * Advent of Code 2020
 * Day 6: Custom Customs
 * https://adventofcode.com/2020/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic -Wno-char-subscripts 06.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 06.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.96 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  7.43 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 14.1  µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2020-06-input.txt"
#define FSIZE 20000  // needed for my input: 17234

// Enable: yes |= bit[*c++];
// Avoid : yes |= 1U << (*c++ - 'a');
static const unsigned bit[] = {
    ['a'] = 1U << ('a' - 'a'), ['b'] = 1U << ('b' - 'a'),
    ['c'] = 1U << ('c' - 'a'), ['d'] = 1U << ('d' - 'a'),
    ['e'] = 1U << ('e' - 'a'), ['f'] = 1U << ('f' - 'a'),
    ['g'] = 1U << ('g' - 'a'), ['h'] = 1U << ('h' - 'a'),
    ['i'] = 1U << ('i' - 'a'), ['j'] = 1U << ('j' - 'a'),
    ['k'] = 1U << ('k' - 'a'), ['l'] = 1U << ('l' - 'a'),
    ['m'] = 1U << ('m' - 'a'), ['n'] = 1U << ('n' - 'a'),
    ['o'] = 1U << ('o' - 'a'), ['p'] = 1U << ('p' - 'a'),
    ['q'] = 1U << ('q' - 'a'), ['r'] = 1U << ('r' - 'a'),
    ['s'] = 1U << ('s' - 'a'), ['t'] = 1U << ('t' - 'a'),
    ['u'] = 1U << ('u' - 'a'), ['v'] = 1U << ('v' - 'a'),
    ['w'] = 1U << ('w' - 'a'), ['x'] = 1U << ('x' - 'a'),
    ['y'] = 1U << ('y' - 'a'), ['z'] = 1U << ('z' - 'a'),
};
static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    char *end = input + fread(input, 1, FSIZE, f);
    *end++ = '\n';  // end last group
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int part1 = 0, part2 = 0;
    unsigned any = 0, all = -1;
    for (const char *c = input; c != end; ) {
        unsigned yes = 0;  // answers per person (= per line)
        while (*c != '\n')
            yes |= bit[*c++];
        any |= yes;  // union per group
        all &= yes;  // intersection per group
        if (*++c == '\n') {  // end of group?
            c++;  // skip empty line
            part1 += __builtin_popcount(any);
            part2 += __builtin_popcount(all);
            any = 0;
            all = -1;
        }
    }
    printf("%d %d\n", part1, part2);  // 6587 3235

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
