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
 *     Macbook Pro 2024 (M4 4.4 GHz) : 226 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :   ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 980 µs
 */

#include <stdio.h>
#include <stdlib.h>  // qsort
#include <stdint.h>  // uint64_t
#include <string.h>  // memcpy
#include <stdbool.h>
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

#define FNAME "../aocinput/2017-04-input.txt"
#define FSIZE (3 * 8 * 1024)  // my input: 22967
#define WORDS 16  // needed for my input: 11

static char input[FSIZE];
static uint64_t word[WORDS];  // words have max 7 characters, MSByte = string terminator

// Sort characters descending (so '\0' stays at end)
static int chardesc(const void *p, const void *q)
{
    const char a = *(const char *)p;
    const char b = *(const char *)q;
    if (a < b) return  1;  // descending
    if (a > b) return -1;
    return 0;
}

// Check unsorted array for no duplicates
// true: no duplicates, false: at least one duplicate
static bool nodup(const int n)
{
    for (int i = 0; i < n - 1; ++i)
        for (int j = i + 1; j < n; ++j)
            if (word[i] == word[j])
                return false;
    return true;
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
        // Parse line
        int n = 0;  // word count per line
        do {
            for (b = a + 1; *b >= 'a'; ++b)  // find word end
                ;
            word[n] = 0;  // same word on previous line may have been longer
            memcpy(&word[n++], a, b - a);
            a = b + 1;
        } while (*b == ' ');  // last word if *b=='\n'

        // Part 1: no duplicate words
        valid1 += nodup(n);

        // Part 2: no anagrams
        for (int i = 0; i < n; ++i)
            qsort(&word[i], sizeof *word, 1, chardesc);
        valid2 += nodup(n);
    }
    printf("%d %d\n", valid1, valid2);  // 455 186

#ifdef TIMER
    printf("Time: %.0f us\n", stoptimer_us());
#endif
}
