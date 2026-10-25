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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 4.87 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <string.h>  // memcpy
#include <stdint.h>  // uint64_t, uint32_t, UINT64_C
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
#define MASK56 UINT64_C(0xfbefbef8)    // 0b11111011111011111011111011111000

typedef uint32_t u32;
typedef uint64_t u64;

static char input[FSIZE];
static u32 lock[N2], key[N2];

// Parse 4x8=32 bytes to 32 bits, discard every 6th and last 2
static u32 getpat(const char *s)
{
    u64 pat = 0, data;
    for (u32 i = 0; i < 4; s += 8, ++i) {
        memcpy(&data, s, 8);
        for (u32 j = 0; j < 8; data >>= 8, ++j)
            pat = pat << 1 | (data & 1);
    }
    return (u32)(pat & MASK56);
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
    for (u32 i = 0, nl = 0, nk = 0; i < N; s += PATLEN, ++i)
        if (*s & 1)
            lock[nl++] = getpat(s + ROWLEN);
        else
            key[nk++] = getpat(s + ROWLEN);

    // Try every lock/key combo
    u32 fit = 0;
    for (u32 i = 0; i < N2; ++i)
        for (u32 j = 0; j < N2; ++j)
            fit += !(lock[i] & key[j]);
    printf("%u\n", fit);  // 3284

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
