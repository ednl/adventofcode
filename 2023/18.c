/**
 * Advent of Code 2023
 * Day 18: Lavaduct Lagoon
 * https://adventofcode.com/2023/day/18
 * By: E. Dronkert https://github.com/ednl
 *
 * Theory:
 *     https://en.wikipedia.org/wiki/Shoelace_formula
 *     https://en.wikipedia.org/wiki/Pick%27s_theorem
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 18.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 18.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz)    :    ? µs
 *     Apple M1 Mac Mini 2020 (3.2 GHz) : 2.94 µs
 *     Raspberry Pi 5 (2.4 GHz)         :    ? µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2023-18-input.txt"
#define FSIZE ((1<<13)|(1<<12))  // 12288, needed for my input: 10759

typedef enum dir {R, D, L, U} Dir;  // R=0, D=1, L=2, U=3
static char input[FSIZE];

// Shoelace: A = 1/2 . sum((y[i] + y[i+1]).(x[i] - x[i+1]))
// https://en.wikipedia.org/wiki/Shoelace_formula
// For two points on horizontal line: y[i] = y[i+1], so y[i] + y[i+1] = 2y
// For two points on vertical line  : x[i] = x[i+1], so x[i] - x[i+1] = 0
// Bring the 1/2 in the sum: A = sum(y.dx) for horizontal lines only.
static void shoelace(int64_t *const restrict a, int64_t *const restrict b, int64_t *const restrict y, const int len, const int dir)
{
    switch (dir) {
        case R: *a -= *y * len; break;
        case D: *y +=      len; break;
        case L: *a += *y * len; break;
        case U: *y -=      len; break;
    }
    *b += len;
}

// Pick: i = A - b/2 + 1, but add border b
// https://en.wikipedia.org/wiki/Pick%27s_theorem
// A can be negative, depending on direction of Shoelace
static int64_t pick(const int64_t a, const int64_t b)
{
    return (a > 0 ? a : -a) + (b >> 1) + 1;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, 1, FSIZE, f);  // read single bytes until EOF
    fclose(f);

#ifdef TIMER
starttimer();
for (unsigned TIMERLOOP = 1000; TIMERLOOP--; ) {
#endif

    int64_t a1 = 0, b1 = 0, y1 = 0;
    int64_t a2 = 0, b2 = 0, y2 = 0;
    for (const char *c = input; *c; c += 8) {
        // Part 1
        const int dir1 = (*c * 143 + 69) >> 6 & 3;  // RDLU = 0123
        int len1;
        if (*(c + 3) == ' ') {
            len1 = *(c + 2) & 15;  // 2..9
            c += 6;
        } else {
            len1 = 10 | (*(c + 3) & 1);  // 10,11
            c += 7;
        }
        shoelace(&a1, &b1, &y1, len1, dir1);

        // Part 2
        const int len2 =
              (( *c      & 16 ?  *c      & 15 : ( *c      & 7) + 9) << 16)
            | ((*(c + 1) & 16 ? *(c + 1) & 15 : (*(c + 1) & 7) + 9) << 12)
            | ((*(c + 2) & 16 ? *(c + 2) & 15 : (*(c + 2) & 7) + 9) <<  8)
            | ((*(c + 3) & 16 ? *(c + 3) & 15 : (*(c + 3) & 7) + 9) <<  4)
            | ((*(c + 4) & 16 ? *(c + 4) & 15 : (*(c + 4) & 7) + 9));
        const int dir2 = *(c + 5) & 3;
        shoelace(&a2, &b2, &y2, len2, dir2);
    }
    printf("%"PRIu64" %"PRIu64"\n", pick(a1, b1), pick(a2, b2)); // 46334 102000662718092

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
