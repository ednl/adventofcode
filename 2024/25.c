/**
 * Advent of Code 2024
 * Day 25: Code Chronicle
 * https://adventofcode.com/2024/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 25.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 25.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  4.52 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  6.93 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 18.3  µs
 */

#include <stdio.h>
#include <stdint.h>  // uint32_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-25-input.txt"
#define N      (4000 >> 3)             // lines+1 in input file divided by 8 lines per pattern
#define N2     (N >> 1)                // half locks, half keys
#define M      5                       // 5x5 grid of '#' or '.': fits in 32-bit integer as bit pattern
#define ROWLEN (M + 1)                 // 5 chars +newline
#define PATLEN ((M + 2) * ROWLEN + 1)  // +2 rows at top/bottom, +empty line
#define FSIZE  (N * PATLEN - 1)        // -1: no trailing empty line
#define MASK56 0x1f7df7dfu             // 0001 1111 0111 1101 1111 0111 1101 1111

typedef uint32_t u32;
static char input[FSIZE];
static u32 lck[N2], key[N2];

// Parse 29 bytes to 29 bits
static u32 getpat(const char *s)
{
    u32 pat = 0;
    for (u32 i = 0; i < 29; ++i)  // or 32 if it helps vectorisation
        pat |= ((*(s + i) & 1) << i);
    return pat & MASK56;  // reset every 6th and last 2 bits
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fputs("File not found: "FNAME, stderr); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Parse
    const char *s = input;
    for (u32 i = 0, l = 0, k = 0; i < N; s += PATLEN, ++i)
        if (*s & 1)
            lck[l++] = getpat(s + ROWLEN);
        else
            key[k++] = getpat(s + ROWLEN);

    // Try every lock/key combo
    u32 fit = 0;
    for (u32 i = 0; i < N2; ++i)
        for (u32 j = 0; j < N2; ++j)
            fit += !(lck[i] & key[j]);
    printf("%u\n", fit);  // 3284

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
