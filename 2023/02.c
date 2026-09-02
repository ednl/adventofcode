/**
 * Advent of Code 2023
 * Day 2: Cube Conundrum
 * https://adventofcode.com/2023/day/2
 * By: E. Dronkert https://github.com/ednl
 * using an idea from /u/terje_wiig_mathisen via
 * https://old.reddit.com/r/adventofcode/comments/1w527yc/2023_day_2_in_review_cube_conundrum/p7c10w9/
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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.15 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 1.69 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 3.81 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-02-input.txt"
#define FSIZE 0x3000  // needed for my input: 10528
#define RMAX 12
#define GMAX 13
#define BMAX 14

static char input[FSIZE];

static int readnum(const char **s)
{
    int x = *(*s)++ & 15;
    while (**s & 16)
        x = x * 10 + (*(*s)++ & 15);
    return x;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) return 1;
    fread(input, 1, sizeof input, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    int part1 = 0, part2 = 0, game = 0;
    for (const char *c = input; *c; c++) {
        c += 6;            // skip "Game x" (1 digit)
        while (*c != ':')  // skip to colon
            c++;
        register int rmax = 0, gmax = 0, bmax = 0;  // maximum number of cubes per colour per game
        do {
            c += 2;                         // skip ": " or ", " or "; "
            const int cubes = readnum(&c);  // read number until space
            c++;
            if (*c == 'r') {
                rmax = cubes > rmax ? cubes : rmax;
                c += 3;
            } else if (*c == 'g') {
                gmax = cubes > gmax ? cubes : gmax;
                c += 5;
            } else if (*c == 'b') {
                bmax = cubes > bmax ? cubes : bmax;
                c += 4;
            }
        } while (*c != '\n');  // until newline
        game++;                // game numbers are consecutive & identical to line number, so no parsing
        if (rmax <= RMAX && gmax <= GMAX && bmax <= BMAX)
            part1 += game;
        part2 += rmax * gmax * bmax;
    }
    printf("%u %u\n", part1, part2);  // 2105 72422

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
