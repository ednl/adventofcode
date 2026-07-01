/**
 * Advent of Code 2021
 * Day 1:
 * https://adventofcode.com/2021/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native ../startstoptimer.c 01.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 0.83 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    : ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : ? µs
 */

#include <stdio.h>
#include <stdint.h>  // uint32_t
#include "../startstoptimer.h"

#define FNAME "../aocinput/2021-01-input.txt"
#define FSIZE 10000  // needed for my input: 9690
#define N 2000

static char input[FSIZE];
static uint32_t data[N];  // 4 bytes per entry

// Count how many sliding windows of measurements
// are increasing from one to the next
static int inc(const int windowsize)
{
    int count = 0;
    for (int i = windowsize; i < N; ++i)
        count += data[i] > data[i - windowsize];
    return count;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) return 1;
    fread(input, 1, FSIZE, f);  // read char-by-char to EOF or max size
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Count number of lines with 3-digit numbers
    int three = 0;
    for (const char *eol = input + 3; *eol == '\n'; three++, eol += 4);

    const char *c = input;
    // Parse 3-digit numbers: only convert byte order from big-endian
    // (=character stream order) to little-endian, leave ASCII values
    for (int i = 0; i < three; c += 4, i++)
        data[i] = *c << 16 | *(c + 1) << 8 | *(c + 2);
    // Parse 4-digit numbers, same deal
    for (int i = three; i < N; c += 5, i++)
        data[i] = *c << 24 | *(c + 1) << 16 | *(c + 2) << 8 | *(c + 3);

    // Count increasing (windows of) measurements
    printf("%d %d\n", inc(1), inc(3));  // 1466 1491

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
