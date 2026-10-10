/**
 * Advent of Code 2024
 * Day 10: Hoof It
 * https://adventofcode.com/2024/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 10alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 11.9 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <string.h>  // memset
#include <stdint.h>  // uint64_t
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-10-input.txt"
#define N 45      // square grid dimension in input file
#define HEAD '0'  // trailhead
#define GOAL '9'  // end of the trail

// Derived values
#define COLS (N + 1)  // +newline
#define FSIZE (N * COLS)  // input file size
#define MAPSIZE ((N + 4) * COLS)  // plus 2 border rows top+bottom
#define BEG (2 * COLS)  // first grid location inside map
#define END (BEG + FSIZE - 1)  // last+1 grid location inside map

static char map[MAPSIZE];  // input file incl. newlines and border rows top+bottom
static int path[MAPSIZE];

static int step(const int ix, const int prev)
{
    if (map[ix - COLS] == prev) path[ix] += path[ix - COLS];
    if (map[ix - 1   ] == prev) path[ix] += path[ix - 1   ];
    if (map[ix + 1   ] == prev) path[ix] += path[ix + 1   ];
    if (map[ix + COLS] == prev) path[ix] += path[ix + COLS];
    return path[ix];
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(&map[BEG], FSIZE, 1, f);  // leave 2 blank rows at top (and bottom)
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    memset(path, 0, sizeof path);
    for (int i = BEG; i < END; ++i)
        if (map[i] == HEAD)
            path[i] = 1;  // base
    for (int height = HEAD + 1; height < GOAL; ++height)
        for (int i = BEG; i < END; ++i)
            if (map[i] == height)
                step(i, height - 1);  // accumulate
    int sum = 0;
    for (int i = BEG; i < END; ++i)
        if (map[i] == GOAL)
            sum += step(i, GOAL - 1);  // final path count
    printf("%u\n", sum);  // gives 3619, should be 1225 for my input

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
