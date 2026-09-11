/**
 * Advent of Code 2023
 * Day 11: Cosmic Expansion
 * https://adventofcode.com/2023/day/11
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 11.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 11.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  7.8 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : 11.1 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 21.0 µs
 */

#include <stdio.h>
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include <string.h>  // memset
    #include "../startstoptimer.h"  // timer
#endif

#define FNAME "../aocinput/2023-11-input.txt"
#define N  140      // rows and columns in image map
#define F1 2        // empty space expansion factor part 1
#define F2 1000000  // empty space expansion factor part 2

static char image[N][N + 1];  // +newline
static uint8_t gx[N];  // galaxies per col

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(image, sizeof image, 1, f);  // read as one block
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
    memset(gx, 0, sizeof gx);
#endif

    // Parse and y-axis result
    int64_t part1 = 0, part2 = 0;
    register int64_t sum1 = 0, sum2 = 0, tot = 0;
    for (int i = 0; i < N; ++i) {
        register int64_t gy = 0;  // galaxies per row
        for (int j = 0; j < N; ++j)
            if (image[i][j] == '#') {
                gx[j]++;
                gy++;
            }
        if (gy) {
            part1 += gy * sum1;
            part2 += gy * sum2;
            tot += gy;  // running total of galaxies
            sum1 += tot;
            sum2 += tot;
        } else {
            sum1 += tot * F1;
            sum2 += tot * F2;
        }
    }

    // Sum of all pairwise distances on one axis
    // https://github.com/maneatingape/advent-of-code-rust/blob/main/src/year2023/day11.rs
    sum1 = sum2 = tot = 0;
    for (int i = 0; i < N; ++i)
        if (gx[i]) {
            part1 += gx[i] * sum1;
            part2 += gx[i] * sum2;
            tot += gx[i];  // running total of galaxies
            sum1 += tot;
            sum2 += tot;
        } else {
            sum1 += tot * F1;
            sum2 += tot * F2;
        }
    printf("%"PRId64" %"PRId64"\n", part1, part2);  // 9608724 904633799472

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
