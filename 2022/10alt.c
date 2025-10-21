/**
 * Advent of Code 2022
 * Day 10: Cathode-Ray Tube
 * https://adventofcode.com/2022/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=c17 -Wall -Wextra -pedantic 10alt.c
 *    gcc   -std=c17 -Wall -Wextra -pedantic 10alt.c
 * Enable timer:
 *    clang -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10alt.c
 *    gcc   -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10alt.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)            : 0.96 µs
 *     Mac Mini 2020 (M1 3.2 GHz)               : 1.75 µs
 *     Raspberry Pi 5 (2.4 GHz)                 : 2.46 µs
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz) : ?    µs
 */

#include <stdio.h>    // fopen, fread, fclose, fputc, printf
#include <stdlib.h>   // abs
#include <unistd.h>   // isatty, fileno
#include <stdbool.h>  // bool
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define HEIGHT 6
#define WIDTH 40
#define MID (WIDTH >> 1)

static char inp[1024];  // large enough to hold complete input file
static char crt[HEIGHT][WIDTH + 1];  // +newline

int main(void)
{
    if (isatty(fileno(stdin))) {
        // Read input file from disk
        FILE *f = fopen("../aocinput/2022-10-input.txt", "r");
        if (!f)
            return 1;
        fread(inp, sizeof inp, 1, f);
        fclose(f);
    } else
        // Read input or example file from pipe or redirected stdin
        fread(inp, sizeof inp, 1, stdin);

#ifdef TIMER
    starttimer();
#endif

    // CRT emulation, draw 1 pixel per cycle
    int regx = 1, cycle = 0, line = 0, beam = 0, part1 = 0;
    for (char *c = inp; *c; ) {
        crt[line][beam] = abs(regx - beam) < 2 ? '#' : ' ';  // beam hits 3px wide sprite?
        ++cycle;
        ++beam;
        if (beam == MID) {
            part1 += cycle * regx;       // sum of "signal strength" halfway scan line
        } else if (beam == WIDTH) {
            crt[line][WIDTH] = '\n';
            beam = 0;                    // CR
            line++;                      // LF
        }
        if (*c & 64) {                   // 'a' or 'n'
            c += 5;                      // skip opcode (4 chars) and space or newline
        } else {                         // '-' or '0'..'9'
            const bool neg = *c == '-';  // negative number?
            c += neg;                    // skip sign if present
            int n = *c++ & 15;           // manual conversion ascii->int
            while (*c != '\n')
                n = n * 10 + (*c++ & 15);
            regx += neg ? -n : n;
            c++;                         // skip newline
        }
    }

    // Output puzzle solutions using direct write
    char buf[8];
    size_t i = sizeof buf;
    buf[--i] = '\n';
    do {
        buf[--i] = '0' | part1 % 10;  // manual conversion int->ascii
        part1 /= 10;
    } while (part1);
    fwrite(buf + i, sizeof buf - i, 1, stdout);  // part 1: example=13140, input=15020
    fwrite(crt, sizeof crt, 1, stdout);          // part 2: EFUGLPAP

#ifdef TIMER
    printf("Time: %.0f ns\n", stoptimer_ns());
#endif
}
