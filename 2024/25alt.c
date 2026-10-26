/**
 * Advent of Code 2024
 * Day 25: Code Chronicle
 * https://adventofcode.com/2024/day/25
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 25alt.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 25alt.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 4.83 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>  // uint32_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-25-input.txt"
#define N  500  // patterns in input file
#define COLS 5  // every pattern has 5 columns

#define ROWS  (COLS + 2)           // +top/bottom rows
#define LEN   (ROWS * (COLS + 1))  // +newline
#define SIZE  (LEN + 1)            // +empty line
#define FSIZE (N * SIZE - 1)       // -no trailing empty line
#define COUNT (N >> 1)             // number of locks and keys

typedef uint32_t u32;
static char input[FSIZE];
static u32 lck[COUNT], key[COUNT];

static u32 getpat(const char **s)
{
    u32 pat = 0;  // full pattern, lock or key
    for (u32 i = 0; i < ROWS; ++(*s), ++i) {  // skip newline
        u32 row = 0;
        for (u32 j = 0; j < COLS; ++(*s), ++j)
            row |= (**s & 1) << (j * 4);  // 4 bits per col
        pat += row;
    }
    return pat;
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

    const char *s = input;
    for (u32 i = 0, n = 0, m = 0; i < N; ++s, ++i)  // skip empty line
        if (*s & 1)
            lck[n++] = getpat(&s);
        else
            key[m++] = getpat(&s);

    u32 fit = 0;
    for (u32 i = 0; i < COUNT; ++i)
        for (u32 j = 0; j < COUNT; ++j)
            fit += !((lck[i] + key[j]) & 0x88888u);
    printf("%u\n", fit);  // 3284

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
