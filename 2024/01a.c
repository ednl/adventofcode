/**
 * Advent of Code 2024
 * Day 1: Historian Hysteria
 * https://adventofcode.com/2024/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01a.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01a.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  29.1 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  60.3 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 140.6 µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, abs
#include <stdint.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-01-input.txt"
#define N 1000  // number of lines in input file
#define M 100000  // greater than any input value
#define FSIZE (N * 14)  // 5+3+5+1 = 14
#define OFS ('0' * 11111)  // 5-digit number ascii offset

static char input[FSIZE];
static int col1[N];
static int col2[N];  // two columns of values
static uint8_t hist[M];

static int cmp_int_asc(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static int readnum(const char *const s)
{
    return *s * 10000 + *(s + 1) * 1000 + *(s + 2) * 100 + *(s + 3) * 10 + *(s + 4) - OFS;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fputs("File not found: "FNAME, stderr); return EXIT_FAILURE; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (int TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Read numbers from columns
    const char *c = input;
    for (int i = 0; i < N; ++i) {
        col1[i] = readnum(c);           c += 8;
        hist[(col2[i] = readnum(c))]++; c += 6;
    }

    // Sort columns separately
    qsort(col1, N, sizeof *col1, cmp_int_asc);
    qsort(col2, N, sizeof *col2, cmp_int_asc);

    int part1 = 0, part2 = 0;
    for (int i = 0; i < N; ++i) {
        part1 += abs(col1[i] - col2[i]);
        part2 += col1[i] * hist[col1[i]];
        hist[col1[i]] = 0;  // reset for next timer loop; faster than memset of whole array even for u8
    }
    printf("%u %u\n", part1, part2);  // 1320851 26859182

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
