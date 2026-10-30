/**
 * Advent of Code 2025
 * Day 4: Printing Department
 * https://adventofcode.com/2025/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 31.4 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // uint8_t, UINT8_MAX
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2025-04-input.txt"
#define N 140      // square grid dimension in input file
#define PAPER '@'  // paper roll
#define BOXED  4   // 4 or more neighbours = unreachable for the forklift

#define COLS  (N + 1)  // +newline
#define FSIZE (N * COLS)
#define BEG   (COLS + 1)  // +top border row +left border col
#define END   (BEG + FSIZE - 1)  // -last newline
#define GSIZE (END + COLS)

static const int ofs[] = {0, 1, 2, COLS, COLS + 2, COLS * 2, COLS * 2 + 1, COLS * 2 + 2};

static char grid[GSIZE];
static uint8_t nb[GSIZE];
static int q[N * N];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(grid + BEG, FSIZE, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    memset(nb, 0, sizeof nb);
    for (int i = 0; i < 8; ++i) {
        const char *g = &grid[ofs[i]];
        for (int j = BEG; j < END; ++g, ++j)
            nb[j] += *g == PAPER;
    }
    for (int i = BEG; i < END; ++i)
        nb[i] = grid[i] == PAPER ? nb[i] : UINT8_MAX;

    // Part 1
    int head = 0, tail = 0;
    for (int i = BEG; i < END; ++i) {
        q[head] = i;
        head += nb[i] < BOXED;
    }
    printf("%u ", head);  // 1495

    // Part 2
    while (tail < head) {
        const int base = q[tail++] - BEG;
        for (int i = 0; i < 8; ++i) {
            const int ix = base + ofs[i];
            q[head] = ix;
            head += nb[ix]-- == BOXED;
        }
    }
    printf("%u\n", head);  // 8768

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
