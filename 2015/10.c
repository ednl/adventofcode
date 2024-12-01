/**
 * Advent of Code 2015
 * Day 10: Elves Look, Elves Say
 * https://adventofcode.com/2015/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    clang -std=gnu17 -O3 -march=native -Wall -Wextra 10.c ../startstoptimer.c
 *    gcc   -std=gnu17 -O3 -march=native -Wall -Wextra 10.c ../startstoptimer.c -lm
 * Get minimum runtime:
 *     m=999999;for((i=0;i<5000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime:
 *     Mac Mini 2020 (M1 3.2 GHz)                       :    ? ms
 *     iMac 2013 (Core i5 Haswell 4570 3.2 GHz)         : 17.6 ms
 *     Raspberry Pi 5 (2.4 GHz)                         :    ? ms
 *     Macbook Air 2013 (Core i5 Haswell 4250U 1.3 GHz) :    ? ms
 *     Raspberry Pi 4 (1.8 GHz)                         :    ? ms
 */

// Sequence info: https://en.wikipedia.org/wiki/Look-and-say_sequence

#include <stdio.h>
#include <stdlib.h>  // calloc, free
#include <string.h>  // strcpy
#include <math.h>    // exp, log
#include "../startstoptimer.h"

// Problem parameters part 1 & 2
#define ITER1  40
#define ITER2  50

// https://www.wolframalpha.com/input/?i=conway%27s+constant
#define CONWAY 1.303577269034

// Personalised input from AoC
static const char *seed = "1113222113";

static void looksay(const char *const a, char *const b, size_t *const len)
{
    size_t i = 0, j, k = 0;
    while (i < *len) {
        j = i + 1;
        while (a[j] == a[i])  // a[j] == 0 != a[i] for all j >= len (assumes len < sizeof(a))
            ++j;
        b[k++] = (char)(j - i);  // assumes k < sizeof(b)
        b[k++] = a[i];
        i = j;
    }
    *len = k;
}

static size_t looksaytwice(char *const a, char *const b, size_t *const len, const size_t beg, const size_t end)
{
    for (size_t i = beg; i < end; i += 2) {
        looksay(a, b, len);
        looksay(b, a, len);
    }
    return *len;
}

int main(void)
{
    starttimer();
    size_t len = strlen(seed);
    // Smallest size enough for ITER2=50: floor(CC^57)=3654234
    // This version ceil(CC^58)=4763577, which is fine too
    // size_t maxlen = (size_t)(ceil(pow(CONWAY, ITER2 + floor(log(len) / log(CONWAY)))));
    size_t maxlen = 4 * 1024 * 1024;  // nice & round 4 MB = 4194304 > 3579328
    // printf("maxlen=%zu\n", maxlen);

    // make all zero to recognise end of sequence
    char *a = calloc(maxlen, sizeof *a);
    char *b = calloc(maxlen, sizeof *b);

    // Replace chars by numerical values
    strcpy(a, seed);
    for (size_t i = 0; i < len; ++i)
        a[i] -= '0';

    size_t p1 = looksaytwice(a, b, &len, 0, ITER1);
    printf("Part 1: %zu\n", p1);  // 252594
    size_t p2 = looksaytwice(a, b, &len, ITER1, ITER2);
    printf("Part 2: %zu\n", p2);  // 3579328

    // Approximation good to 5 decimals (rounded)
    // printf("Conway's Constant : %.9f\n", CONWAY);
    // printf("This approximation: %.9f\n", exp((log(p2) - log(p1)) / (ITER2 - ITER1)));

    free(a);
    free(b);
    printf("Time: %.0f us\n", stoptimer_us());
    return 0;
}
