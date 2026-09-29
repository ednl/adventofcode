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
 *     Mac Mini 2020 (M1 3.2 GHz)    :  61.8 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 142   µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort, abs
#include <stdint.h>  // uint8_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-01-input.txt"
#define N 1000  // number of lines in input file
#define M 100000  // greater than any input value
#define FSIZE (N * 14)  // 5+3+5+1 = 14

static char input[FSIZE];
static int a[N], b[N];  // two columns of values
static uint8_t freq[M];

static int cmp_int_asc(const void *p, const void *q)
{
    return *(const int *)p - *(const int *)q;  // safe because 10000 <= x < 100000
}

static int readnum(const char *const c)
{
    return *c * 10000 + *(c + 1) * 1000 + *(c + 2) * 100 + *(c + 3) * 10 + *(c + 4) - 0x82350;
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
        a[i] = readnum(c);           c += 8;
        freq[(b[i] = readnum(c))]++; c += 6;
    }

    // Sort columns separately
    qsort(a, N, sizeof *a, cmp_int_asc);
    qsort(b, N, sizeof *b, cmp_int_asc);

    int part1 = 0, part2 = 0;
    for (int i = 0; i < N; ++i) {
        part1 += abs(a[i] - b[i]);
        part2 += a[i] * freq[a[i]];
        freq[a[i]] = 0;  // reset for next timer loop; faster than memset of whole array
    }
    printf("%u %u\n", part1, part2);  // 1320851 26859182

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
