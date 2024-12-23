/**
 * Advent of Code 2024
 * Day 23: LAN Party
 * https://adventofcode.com/2024/day/22
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -Wall -Wextra 23.c ../combperm.c
 *    gcc   -std=gnu17 -Wall -Wextra 23.c ../combperm.c
 * Enable timer:
 *    clang -DTIMER -O3 -march=native 23.c ../combperm.c ../startstoptimer.c
 *    gcc   -DTIMER -O3 -march=native 23.c ../combperm.c ../startstoptimer.c
 * Get minimum runtime from timer output:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Mac Mini 2020 (M1 3.2 GHz)                       : ? ms
 *     Raspberry Pi 5 (2.4 GHz)                         : ? ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) : ? ms
 *     Raspberry Pi 4 (1.8 GHz)                         : ? ms
 */

#include <stdio.h>
#include <stdbool.h>
#include "../combperm.h"
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define EXAMPLE 0
#if EXAMPLE == 1
    #define FNAME "../aocinput/2024-23-example.txt"
    #define N 32
#else
    #define FNAME "../aocinput/2024-23-input.txt"
    #define N 3380
#endif
#define FSIZE (N * 6)
#define M (26 * 26)

static char input[FSIZE];
static bool edge[M][M];
static bool found[M];
static int id[M];
static int idlen;

int main(void)
{
    FILE *f = fopen(FNAME, "rb");  // binary mode needed for fread
    if (!f) { fprintf(stderr, "File not found: %s\n", FNAME); return 1; }
    fread(input, sizeof input, 1, f);  // read whole file in one go
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    const char *c = input;
    for (int n = 0; n < N; ++n) {
        const int a = (*c       - 'a') * 26 + *(c + 1) - 'a';
        const int b = (*(c + 3) - 'a') * 26 + *(c + 4) - 'a';
        edge[a][b] = edge[b][a] = true;
        found[a] = found[b] = true;
        c += 6;
    }
    for (int i = 0; i < M; ++i)
        if (found[i])
            id[idlen++] = i;
    printf("len=%d\n", idlen);

#ifdef TIMER
    printf("Time: %.0f ms\n", stoptimer_ms());
#endif
    return 0;
}
