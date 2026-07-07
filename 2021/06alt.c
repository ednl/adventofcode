/**
 * Advent of Code 2021
 * Day 6: Lanternfish
 * https://adventofcode.com/2021/day/6
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 06alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native ../startstoptimer.c 06alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 377 ns
 *     Mac Mini 2020 (M1 3.2 GHz)    : 525 ns
 *     Raspberry Pi 5 (2.4 GHz)      : 518 ns
 */

#include <stdio.h>
#include <string.h>    // memcpy
#include <stdint.h>    // uint64_t, UINT64_C
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-06-input.txt"
#define FISH 300  // 300 numbers in input file
#define FSIZE (FISH * 2)  // CSV +newline
#define M 5  // max fish age

// state conversion matrix:
//   0 1 0 0 0 0 0 0 0
//   0 0 1 0 0 0 0 0 0
//   0 0 0 1 0 0 0 0 0
//   0 0 0 0 1 0 0 0 0
//   0 0 0 0 0 1 0 0 0
//   0 0 0 0 0 0 1 0 0
//   1 0 0 0 0 0 0 1 0
//   0 0 0 0 0 0 0 0 1
//   1 0 0 0 0 0 0 0 0
// Sums of columns 1..5 of state^80
#define A1 1401U
#define A2 1191U
#define A3 1154U
#define A4 1034U
#define A5  950U
// Sums of columns 1..5 of state^256
#define B1 UINT64_C(6206821033)
#define B2 UINT64_C(5617089148)
#define B3 UINT64_C(5217223242)
#define B4 UINT64_C(4726100874)
#define B5 UINT64_C(4368232009)

static char input[FSIZE];

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    const char *const end = input + fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Histogram of fish population count per age mod 9
    uint8_t fish[M + 1] = {0};  // only use index 1..5

    // Parse
    for (const char *c = input; c != end; c += 8) {
        uint64_t chunk;
        memcpy(&chunk, c, 8);  // safe way instead of UB from pointer alignment
        fish[chunk       & 0xf]++;
        fish[chunk >> 16 & 0xf]++;
        fish[chunk >> 32 & 0xf]++;
        fish[chunk >> 48 & 0xf]++;
    }

    // Closed form solution
    printf("%u %"PRIu64"\n",
        A1 * fish[1] + A2 * fish[2] + A3 * fish[3] + A4 * fish[4] + A5 * fish[5],   // 374927
        B1 * fish[1] + B2 * fish[2] + B3 * fish[3] + B4 * fish[4] + B5 * fish[5]);  // 1687617803407

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
