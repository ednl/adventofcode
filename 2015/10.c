/**
 * Advent of Code 2015
 * Day 10: Elves Look, Elves Say
 * https://adventofcode.com/2015/day/10
 * By: E. Dronkert https://github.com/ednl
 *
 * Compile:
 *    cc -std=c17 -Wall -Wextra -pedantic 10.c -lm
 * Enable timer:
 *    cc -O3 -march=native -mtune=native -DTIMER ../startstoptimer.c 10.c -lm
 * Get minimum runtime from timer output in bash:
 *     m=999999;for((i=0;i<10000;++i));do t=$(./a.out|tail -n1|awk '{print $2}');((t<m))&&m=$t&&echo "$m ($i)";done
 * Minimum runtime measurements for parts 1-3:
 *     Macbook Pro 2024 (M4 4.4 GHz) : 17 µs
 *     Mac Mini 2020 (M1 3.2 GHz)    :  ? µs
 *     Raspberry Pi 5 (2.4 GHz)      : 43 µs
 */

#include <stdio.h>
#include <stdlib.h>    // qsort, bsearch
#include <string.h>    // strlen
#include <math.h>      // exp, log
#include <stdint.h>    // int64_t
#include <inttypes.h>  // PRId64
#ifdef TIMER
    #include "../startstoptimer.h"
#endif

// Personalised input (= element 'Po')
static const char input[] = "1113222113";

// Decay steps part 1 and 2, and one extra to show off
#define N1 40
#define N2 50
#define N3 150

// https://en.wikipedia.org/wiki/Look-and-say_sequence#Cosmological_decay
#define ELEMENTS 92
#define DECAYSIZE 6  // max. number of decay elements

// https://www.wolframalpha.com/input/?i=conway%27s+constant
#define CONWAY 1.303577269034

typedef struct pair {
    int key, val;
} Pair;

static Pair elname[ELEMENTS];  // element name+index
static int  decay [ELEMENTS][DECAYSIZE];  // element splits into which elements
static int  split [ELEMENTS];  // element splits into how many elements when decaying
static int  elsize[ELEMENTS];  // element size, e.g. "12311" = 5

// Elements don't interact, so order doesn't matter,
// in fact we only need a running count per element
static int64_t counta[ELEMENTS];
static int64_t countb[ELEMENTS];

// Qsort *and* bsearch helper
// First field of struct (Pair::key) must be int
static int name_asc(const void *p, const void *q)
{
    const int a = *(const int *)p;
    const int b = *(const int *)q;
    if (a < b) return -1;
    if (a > b) return  1;
    return 0;
}

// Decay elements from a to b, one step to generate the next sequence
// 'restrict' = must be non-overlapping arrays
static void onestep(const int64_t *restrict a, int64_t *restrict b)
{
    memset(b, 0, sizeof *b * ELEMENTS);  // reset b
    for (int i = 0; i < ELEMENTS; ++i)
        for (int j = 0; j < split[i]; ++j)
            b[decay[i][j]] += a[i];
}

// Decay sequence after even number of steps
// Return final sequence length
static int64_t sequence(const int steps)
{
    for (int i = 0; i < steps; i += 2) {
        onestep(counta, countb);
        onestep(countb, counta);
    }
    int64_t len = 0;
    for (int i = 0; i < ELEMENTS; ++i)
        len += counta[i] * elsize[i];
    return len;
}

int main(void)
{
    // Parse the elements table = text version of
    // https://en.wikipedia.org/wiki/Look-and-say_sequence#Cosmological_decay
    // without the last, unexplained column "abundance"
    FILE *f = fopen("10.txt", "r");
    if (!f) return 1;
    char snum[4], sname[4], satoms[48], sdecay[20];
    for (int i = 0; i < ELEMENTS && fscanf(f, "%2s %2s %42s %16s", snum, sname, satoms, sdecay) == 4; ++i) {
        // Element name (1 or 2 chars) is int in the same byte order
        // so for 1-char names, terminating '\0' is fine as second byte
        elname[i] = (Pair){.key = sname[0] | sname[1] << 8, .val = i};
        int j = 0;
        for (const char *s = sdecay; j < DECAYSIZE; ++s) {  // read decay elements string
            int x = *s++;  // shorter like above impossible b/c of '.' separator characters
            if (*s && *s != '.')
                x |= *s++ << 8;
            decay[i][j++] = x;
            if (!*s)
                break;
        }
        split [i] = j;  // element i splits into j elements
        elsize[i] = strlen(satoms);  // element size (atomic sequence length)
        counta[i] = !strcmp(satoms, input);  // starting element (assumes input is element)
    }
    fclose(f);

#ifdef TIMER
    starttimer();
#endif

    // Replace element names with indexes
    // Assumes all elements can be found, which is true for the Wikipedia table
    qsort(elname, ELEMENTS, sizeof *elname, name_asc);
    for (int i = 0; i < ELEMENTS; ++i)
        for (int j = 0; j < split[i]; ++j)
            decay[i][j] = ((const Pair *)bsearch(&decay[i][j], elname, ELEMENTS, sizeof *elname, name_asc))->val;

    const int64_t p1 = sequence(N1);
    const int64_t p2 = sequence(N2 - N1);
    const int64_t p3 = sequence(N3 - N2);
    printf("%3d: %"PRId64"\n", N1, p1);  //  40: 252594
    printf("%3d: %"PRId64"\n", N2, p2);  //  50: 3579328
    printf("%3d: %"PRId64"\n", N3, p3);  // 150: 1168021999835586648

#ifdef TIMER
    double us = stoptimer_us();
#endif

    // True in the limit: p2 = p1 * pow(CONWAY, N2 - N1)
    // => log(p2 / p1) = (N2 - N1) * log(CONWAY)
    // => CONWAY = exp(log(p2 / p1) / (N2 - N1))
    // Approximation good to 5 decimals!
    printf("Conway's Constant : %.9f\n", CONWAY);
    printf("This approximation: %.9f\n", exp(log((double)p3 / p2) / (N3 - N2)));

#ifdef TIMER
    printf("Time: %.0f us\n", us);
#endif
}
