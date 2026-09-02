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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 2.99 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 4.32 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 8.62 µs
 */

#include <stdio.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-02-input.txt"
#define FSIZE 0x3000  // needed for my input: 10528
#define RLIM 12
#define GLIM 13
#define BLIM 14

static char input[FSIZE];

static inline unsigned max(const unsigned a, const unsigned b)
{
    return a > b ? a : b;
}

static unsigned readnum(const char** s)
{
    unsigned x = *(*s)++ & 15;
    while (**s & 16)  // until space
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

    unsigned part1 = 0, part2 = 0, game = 0;
    for (const char *c = input; *c; c++) {
        ++game;  // game numbers are consecutive & identical to line number, so no parsing
        unsigned rgbmax[3] = {0};  // maximum number of cubes per colour per game
        c += 7;
        while (*c != ' ')
            c++;
        do {
            c++;  // skip space
            const unsigned cubes = readnum(&c);  // read number until space
            switch (*++c) {  // skip space
                case 'r': rgbmax[0] = max(rgbmax[0], cubes); break;
                case 'g': rgbmax[1] = max(rgbmax[1], cubes); break;
                case 'b': rgbmax[2] = max(rgbmax[2], cubes); break;
            }
            while (*c != ' ' && *c != '\n')
                c++;  // skip to next space or newline
        } while (*c == ' ');
        if (rgbmax[0] <= RLIM && rgbmax[1] <= GLIM && rgbmax[2] <= BLIM)
            part1 += game;
        part2 += rgbmax[0] * rgbmax[1] * rgbmax[2];
    }
    printf("%u %u\n", part1, part2);  // 2105 72422

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
