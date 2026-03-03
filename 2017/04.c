/**
 * Advent of Code 2017
 * Day 4: High-Entropy Passphrases
 * https://adventofcode.com/2017/day/4
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *     cc -std=c17 -Wall -Wextra -pedantic 04.c
 * Enable timer:
 *     cc -std=gnu17 -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 04.c
 * Get minimum runtime from timer output:
 *     m=99999999;for((i=0;i<20000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 274 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      :   ? µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <stdint.h>  // uint64_t
#include <string.h>  // memcpy
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-04-input.txt"
#define FSIZE (3 * 8 * 1024)  // my input: 22967
#define WORDS 16  // needed for my input: 11

static char input[FSIZE];
static uint64_t word[WORDS];

static int cmp_u64(const void *p, const void *q)
{
    const uint64_t a = *(const uint64_t *)p;
    const uint64_t b = *(const uint64_t *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

static int cmp_ch(const void *p, const void *q)
{
    const char a = *(const char *)p;
    const char b = *(const char *)q;
    if (a < b) return  1;
    if (a > b) return -1;
    return 0;
}

static int nodup(const int n)
{
    for (int i = 1; i < n; ++i)
        if (word[i - 1] == word[i])
            return 0;
    return 1;
}

int main(void)
{
    FILE *f = fopen(FNAME, "rb");
    if (!f) { fprintf(stderr, "File not found: "FNAME"\n"); return 1; }
    fread(input, 1, FSIZE, f);
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    int valid1 = 0, valid2 = 0;
    for (const char *a = input, *b; *a; ) {
        int n = 0;
        do {
            for (b = a + 1; *b >= 'a'; ++b);
            word[n] = 0;
            memcpy(&word[n++], a, b - a);
            a = b + 1;
        } while (*b == ' ');
        qsort(word, n, sizeof *word, cmp_u64);
        valid1 += nodup(n);
        for (int i = 0; i < n; ++i)
            qsort(&word[i], sizeof *word, 1, cmp_ch);
        qsort(word, n, sizeof *word, cmp_u64);
        valid2 += nodup(n);
    }
    printf("%d %d\n", valid1, valid2);  // 455 186

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
