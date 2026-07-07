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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 379 ns
 *     Mac Mini 2020 (M1 3.2 GHz)    : 525 ns
 *     Raspberry Pi 5 (2.4 GHz)      : 518 ns
 */

#include <stdio.h>
#include <string.h>    // memcpy
#include <stdint.h>    // uint64_t
#include <inttypes.h>  // PRIu64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2021-06-input.txt"
#define FISH 300  // 300 numbers in input file
#define FSIZE (FISH * 2)  // CSV +newline
#define M 5  // max fish age

typedef uint64_t u64;

// state conversion matrix s:
//   0 1 0 0 0 0 0 0 0
//   0 0 1 0 0 0 0 0 0
//   0 0 0 1 0 0 0 0 0
//   0 0 0 0 1 0 0 0 0
//   0 0 0 0 0 1 0 0 0
//   0 0 0 0 0 0 1 0 0
//   1 0 0 0 0 0 0 1 0
//   0 0 0 0 0 0 0 0 1
//   1 0 0 0 0 0 0 0 0
static const u64 a[M + 1] = {0, 1401, 1191, 1154, 1034, 950};  // colsum(s^80)
static const u64 b[M + 1] = {  // colsum(s^256)
    0, UINT64_C(6206821033), UINT64_C(5617089148),
    UINT64_C(5217223242), UINT64_C(4726100874), UINT64_C(4368232009),
};

static char input[FSIZE];

int main(void)
{
    FILE *fp = fopen(FNAME, "rb");
    if (!fp) return 1;
    const char *const end = input + fread(input, 1, FSIZE, fp);
    fclose(fp);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Histogram of fish population count per age mod 9
    u64 f[M + 1] = {0};  // only use index 1..5

    // Parse
    for (const char *c = input; c != end; c += 8) {
        u64 data;
        memcpy(&data, c, 8);  // safe way instead of UB from pointer alignment
        f[data       & 0xf]++;
        f[data >> 16 & 0xf]++;
        f[data >> 32 & 0xf]++;
        f[data >> 48 & 0xf]++;
    }

    // Closed form solution
    printf("%"PRIu64" %"PRIu64"\n",
        a[1] * f[1] + a[2] * f[2] + a[3] * f[3] + a[4] * f[4] + a[5] * f[5],   // 374927
        b[1] * f[1] + b[2] * f[2] + b[3] * f[3] + b[4] * f[4] + b[5] * f[5]);  // 1687617803407

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
