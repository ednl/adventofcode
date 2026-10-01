/**
 * Advent of Code 2024
 * Day 1: Historian Hysteria
 * https://adventofcode.com/2024/day/1
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 01.c
 * Enable timer:
 *     cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 01.c
 * Test output with timer enabled:
 *     ./a.out | tail -n1
 * Get minimum runtime from timer output in bash:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out 2>&1 1>/dev/null|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) :  29.6 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  62.4 µs
 *     Raspberry Pi 5 (2.4 GHz)      : 145   µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <stdint.h>  // uint32_t
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2024-01-input.txt"
#define N 1000U             // number of lines in input file
#define M 100000U           // bigger than any col1 value
#define FSIZE (N * 14U)     // 5+3+5+1 = 14
#define OFS ('0' * 11111U)  // 5-digit number ascii offset

typedef uint32_t u32;

static char input[FSIZE];
static u32 col1[N];
static u32 col2[N + 1];  // two columns of values, +sentinel

static int cmp_u32_asc(const void *p, const void *q)
{
    const u32 a = *(const u32 *)p;
    const u32 b = *(const u32 *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static u32 readnum(const char *const s)
{
    return *s * 10000U + *(s + 1) * 1000U + *(s + 2) * 100U + *(s + 3) * 10U + *(s + 4) - OFS;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // fread requires binary mode
    if (!f) { fputs("File not found: "FNAME, stderr); return EXIT_FAILURE; }
    fread(input, sizeof input, 1, f);  // read whole file at once
    fclose(f);

#ifdef TIMER
starttimer();
for (unsigned TIMERLOOP = 0; TIMERLOOP < 1000; ++TIMERLOOP) {
#endif

    // Read numbers in columns
    const char *c = input;
    for (u32 i = 0; i < N; ++i) {
        col1[i] = readnum(c); c += 8;
        col2[i] = readnum(c); c += 6;
    }
    col2[N] = M;  // avoid j<N checks

    // Sort columns separately
    qsort(col1, N, sizeof *col1, cmp_u32_asc);
    qsort(col2, N, sizeof *col2, cmp_u32_asc);

    u32 part1 = 0, part2 = 0;
    for (u32 i = 0, j = 0; i < N; ++i) {  // assume col1 values are unique in that col
        part1 += col1[i] > col2[i] ? col1[i] - col2[i] : col2[i] - col1[i];  // distance = absolute value of difference
        for (; col1[i] > col2[j]; ++j);  // col2[N] always bigger than any col1
        for (; col1[i] == col2[j]; ++j)
            part2 += col1[i];
    }
    printf("%u %u\n", part1, part2);  // 1320851 26859182

#ifdef TIMER
}
fprintf(stderr, "Time: %.0f ns\n", stoptimer_us());  // 1000 loops: µs=ns
#endif
}
