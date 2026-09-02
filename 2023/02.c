/**
 * Advent of Code 2023
 * Day 2: Cube Conundrum
 * https://adventofcode.com/2023/day/2
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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 1.96 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 2.91 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 4.45 µs
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

static int readnum(const char** s)
{
    int x = *(*s)++ & 15;
    while (**s & 16)  // second digit or space
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
        int max[3] = {0};                   // maximum number of cubes per colour per game
        game++;                             // game numbers are consecutive & identical to line number, so no parsing
        c += 7;                             // skip "Game N:" (1 digit + colon, or 2 digits)
        while (*c != ' ')                   // skip to first space on line
            c++;
        do {
            c++;                            // skip space
            const int cubes = readnum(&c);  // read number until space
            const char rgb = *++c;          // skip space, first letter of colour
            int *const cur = max + ((rgb & 1) | (rgb >> 3 & 2));  // 'r'=2, 'g'=1, 'b'=0
            *cur = cubes > *cur ? cubes : *cur;
            c += 3;                         // minimum 3 letters to next decider
            while (*c > ' ')                // skip to next space or newline
                c++;
        } while (*c == ' ');
        if (max[2] <= RMAX && max[1] <= GMAX && max[0] <= BMAX)  // 'r'=2, 'g'=1, 'b'=0
            part1 += game;
        part2 += max[0] * max[1] * max[2];
    }
    printf("%u %u\n", part1, part2);  // 2105 72422

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
